import Fs from 'fs'

let sfd = String(Fs.readFileSync('./faces.sfd'))

let parts = sfd.split(/^StartChar: /gm)
parts[parts.length-1] = parts[parts.length-1].replace(/EndChars\nEndSplineFont\n*$/, "")
//console.warn(parts.at(-1))
//process.exit(0)

let header = parts.shift()

let by_name = {}
let by_gid = {}

parts = parts.map(data=>{
	let [,name,e1,e2,e3,rest] = /^(.*)\nEncoding: (\S+) (\S+) (\S+)\n([^]*?)\n*$/.exec(data)
	let chr = {name,slot:e1,unicode:e2,gid:e3,data:rest}
	by_name[name] = chr
	by_gid[e3] = chr
	return chr
})

switch_refers(parts, true)

import cmap from '../cmap.js'

//cmap.push({"ident":"NoddingFace","codes":[["0x1FFFF"],"1ffff"]})

function score(chr) {
	if (chr.name==".notdef")
		return -100
	if (chr.name==".null")
		return -99
	if (chr.name=="nonmarkingreturn")
		return -98
	
	if (chr.name=="zerojoin")
		return -90
	if (chr.name=="varsel16")
		return -80
	if (chr.name.endsWith('.text')) {
		return -70
	}
	/*if (!chr.name.includes('.'))
		return 0*/
	let [base,num] = chr.name.split('.')
	let num2
	//console.warn(base)
	0,[,base,num2] = base.match(/^(.*?)(\d*)$/)
	num2 = +num2 || 0
	let cmi = cmap.findIndex(x=>x.ident==base)
	if (cmi==-1) {
		if (!chr.name.includes('.'))
			return 0
		return 10
	}
	let cm = cmap[cmi]
	if (!cm)
		console.warn('couldnt find', chr.name)
	let uni = (cm.codes ? cm.codes : cm.variants.neutral[0])[0]
	//console.warn(chr.name, uni)
	if (num!=undefined) // it's a layer
		return 20 + cmi + (num/16 + num2)/10
	if (uni.length==1) // one codepoint
		uni = Number(uni[0])
	else // multi-codepoint
		uni = Number(uni[0])+0x100000
	if (num==undefined)
		uni = uni + 0x1000000
	else
		uni = uni + (num/16 + num2)/10
	return uni
}
parts.sort((a,b)=>{
	return score(a)-score(b)
})

let i = 0
for (let chr of parts) {
	//console.log(chr.name, score(chr))
	//console.log(chr.gid, i)
	chr.gid = i
	chr.slot = i
	i++
}

switch_refers(parts, false)

process.stdout.write(header)

for (let chr of parts) {
	//console.log(chr.name)
	process.stdout.write(`
StartChar: ${chr.name}
Encoding: ${chr.slot} ${chr.unicode} ${chr.gid}
${chr.data}
`)
}

function switch_refers(parts, names) {
	for (let chr of parts) {
		chr.data = chr.data.replace(/^Refer: (\S+) (.*)$/mg, (m,id,rest)=>{
			if (names) {
				let c2 = by_gid[id]
				return "Refer: "+c2.name+" "+rest
			}
			let c2 = by_name[id]
			return "Refer: "+c2.gid+" "+rest
		})
	}
}

process.stdout.write(`EndChars
EndSplineFont
`)
