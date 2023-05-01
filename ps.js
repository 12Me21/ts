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

class Elem {
	path = ""
	fill = null
	transform = []
	children = []
	toString() {
		let out = ""
		//if (this.children) {
		out += `<g`
		//}
		if (this.fill)
			out += ` fill="${this.fill}"`
		if (this.transform.length)
			out += ` transform="${this.transform.join(" ")}"`
		
		out += ">"
		if (this.path)
			out += `<path d="${this.path}"/>`
		out += this.children.join("\n")
		out += "</g>"
		return out
	}
}

let elem = new Elem()
let root = new Elem()
root.children.push(elem)
let estack = [root]

let viewbox

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
		'h'() { elem.path += "Z" },
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
				col = "rgb("+col.map(x=>Math.round(x*255))+")"
			elem.fill = col
		},
		'SC'() {
			elem.stroke = pop()
		},
		'cm'() {
			elem.transform.push("matrix("+pop().join(" ")+")")
		},
		'm'() {
			elem.path += "M "+pops(2)
		},
		'l'() {
			elem.path += "L "+pops(2)
		},
		'c'() {
			elem.path += "C "+pops(6)
		},
		're'() {
			let [x,y,w,h] = pops(4)
			elem.path += "M "+[x,y]+" h "+w+" v "+h+" h "+-w+" v "+-h+" Z"
		},
		'W'() { 'set clip path' },
		'w'() {
			pop() // stroke width
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
			assert(pop()==0, 'J')	
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

let out = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 36 36" transform="matrix(1 0 0 -1 0 0)">
${String(elem)}
</svg>`

if (stack.length)
	console.warn('stack not empty')//, stack)

Fs.writeFileSync(`../ai/${name}.svg`, out)
