import Fs from 'fs'

let data = String(Fs.readFileSync(process.argv[2]))

let margin = 24

data = data.replace(/<mtx name="([^"]*)" width="([^"]*)" lsb="([^"]*)"[/]>/g, (m,name,width,lsb)=>{
	width -= 0
	lsb -= 0
	let is_final = width==864
	let is_layer = /[.]\d+$/.test(name)
	if (is_layer) {
		lsb = lsb + 864/2 + margin
		width = 864 + margin + margin
	}
	else if (is_final) {
		width = 864 + margin + margin
		lsb = 864/2 + margin
	}
	else {
//		lsb = 0
		width = 864 + margin + margin
	}
	return `<mtx name="${name}" width="${width}" lsb="${lsb}"/>`
})

process.stdout.write(data)
