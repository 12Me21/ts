import Fs from 'fs'

let name = process.argv[2].match(/[^/.]+(?=[.]ai$)/)[0]
if (/\b1f1[ef][0-9a-f]-|\b1f3f[bcdef]\b/.test(name))
	process.exit(0)
console.warn(name)

let token = /%.*|\[|\]|[/].*?}|[^\s\[\]]+/g
let text = Fs.readFileSync(`../ai/${name}.ai.ps`,'utf-8')
let start = text.indexOf('%%BeginPageSetup')
text = text.slice(start+'%%BeginPageSetup'.length)

function assert(cond, msg) {
	if (!cond)
		throw new Error(msg)
}

let stack0 = []
let array
let stack = stack0

let pop = ()=>stack.pop()
let pops = n=>stack.splice(-n,n)
let push = x=>stack.push(x)

function unflip(a) {
	for (let i=1; i<a.length; i+=2) {
		a[i] = -a[i]
	}
	return a
}

class Elem {
	path = ""
	fill = null
	transform = []
	children = []
	attr = {}
	toString() {
		let out = ""
		
		let g = !(!this.children.length && this.path)
		
		if (g)
			out += `<g`
		else
			out += `<path`
		
		if (this.fill && this.path)
			out += ` fill="${this.fill}"`
		if (this.transform.length)
			out += ` transform="${this.transform.join(" ")}"`
		for (let a in this.attr) {
			out += ` ${a}="${this.attr[a]}"`
		}
		
		if (g)
			out += ">\n"
		
		if (this.path) {
			if (g)
				out += "<path"
			out += ` d="${this.path}"/>\n`
		}
		if (g) {
			for (let c of this.children)
				if (!c.fill && this.fill)
					c.fill = this.fill
			out += this.children.join("")
			out += "</g>\n"
		}
		return out
	}
}

let elem = new Elem()
let root = new Elem()
root.children.push(elem)
let estack = [root]

let viewbox

function format_hex(col) {
	col = col.map(x=>Math.round(x*255).toString(16).toUpperCase().padStart(2,'0'))
	return "#"+col.join("")
}

for (let [x] of text.matchAll(token)) {
	if (x.startsWith("%"))
		continue
	let num = +x
	if (!isNaN(x)) {
		stack.push(x)
		continue
	}
	if (x=='false') {
		stack.push(false)
		continue
	}
	if (x=='true') {
		stack.push(true)
		continue
	}
	if (x=='{}') {
		stack.push(x)
		continue
	}
	
	let f = {
		'['() {
			stack = array = []
		},
		']'() {
			stack = stack0
			stack.push(array)
		},
		'h'() { elem.path += " Z" },
		'f'() { 'fill' },
		'q'() {
			let nw = new Elem()
			elem.children.push(nw)
			estack.push(elem)
			elem = nw
		},
		'Q'() {
			elem = estack.pop()
		},
		'sc'() {
			if (elem.path || elem.fill) {
				let nw = new Elem()
				estack[estack.length-1].children.push(nw)
				elem = nw
			}
			let col = pop()
			if (col.length==1 && col[0]=="0")
				col = "none"
			else
				col = format_hex(col)
			elem.fill = col
		},
		'SC'() {
			console.warn('stroke color')
			elem.attr.stroke = pop()
		},
		'cm'() {
			let matrix = pop()
			//matrix[3] *= -1
			matrix[5] = 36 - matrix[5] + 1
			matrix[4] -= 1
			if (matrix[0]==1 && matrix[1]==0 && matrix[2]==0 && matrix[3]==1)
				elem.transform.push("translate("+matrix[4]+" "+matrix[5]+")")
			else
				elem.transform.push("matrix("+matrix.join(" ")+")")
		},
		'm'() {
			elem.path += " M "+unflip(pops(2))
		},
		'l'() {
			elem.path += " L "+unflip(pops(2))
		},
		'c'() {
			elem.path += " C "+unflip(pops(6))
		},
		're'() {
			let [x,y,w,h] = pops(4)
			;[x,y] = unflip([x,y])
			h = -h
			elem.path += " M "+[x,y]+" h "+w+" v "+h+" h "+-w+" v "+-h+" Z"
		},
		'W'() {
			elem.attr['clip-path'] = "TODO"
		},
		'w'() {
			let c = pop()
			elem.attr['stroke-width'] = c
		},
		'd'() {
			assert(pop()==0,'d')
			assert(pop().length==0,'d')
		},
		'i'() {
			assert(pop()==1, 'i')
		},
		'j'() {
			assert(pop()==0, 'j')
		},
		'J'() {
			let c = pop()
			if (c)
				elem.attr['stroke-linecap'] = 'round'
			//assert(pop()==0, 'J')
		},
		'M'() {
			pop() // miter clip
		},
		
		'settransfer'() {
			assert(pop()=="{}", 'settransfer')
		},
		'pdfStartPage'() { },
		'pdfEndPage'() { },
		'end'() { },
		'showpage'() { },
		
		
		'/DeviceRGB {}'() { },
		'/DeviceGray {}'() { },
		
		'pdfSetupPaper'() {
			let [w,h] = pops(2)
			viewbox = `0 0 ${w} ${h}`
		},
		
		'cs'() { 'fill colorspace' },
		'CS'() { 'stroke colorspace' },
		'op'() {
			assert(pop()==false, 'op')
		},
		'OP'() {
			assert(pop()==false, 'OP')
		},
		
	}[x]
	if (!f)
		console.warn('what is', x)
	else
		f()
}

let out = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 36 36">
${String(elem)}
</svg>`

if (stack.length)
	console.warn('stack not empty')//, stack)

Fs.writeFileSync(`../ai/${name}.svg`, out)
