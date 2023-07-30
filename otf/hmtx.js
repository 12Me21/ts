import Fs from 'fs'

let data = String(Fs.readFileSync(process.argv[2]))

let margin = 24

data = data.replace(/<mtx name="([^"]*)" width="([^"]*)" lsb="([^"]*)"[/]>/g, (m,name,width,lsb)=>{
	width -= 0
	lsb -= 0
	if (/^[A-Z][A-Za-z]+$/.test(name)) {
		width = 864 + margin + margin
		lsb = margin
	} else if (width>0) {
		width = 864 + margin + margin
		lsb = lsb + 864/2 + margin
	} else {
		lsb = 0
		width = 0
	}
	return `<mtx name="${name}" width="${width}" lsb="${lsb}"/>`
})

process.stdout.write(data)
