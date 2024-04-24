import fontforge

font = fontforge.open("faces.sfd")

out = open('colr2.xml', 'w')

out.write('<?xml version="1.0" encoding="UTF-8"?>\n<ttFont sfntVersion="\\x00\\x01\\x00\\x00" ttLibVersion="4.34">\n\t<COLR>\n\t\t<version value="0"/>\n')

palette = dict()

for name in font:
	glyph = font[name]
	layers = glyph.comment.split("\n")
	if len(layers)>0 and '#' in layers[0]:
		out.write('\t\t<ColorGlyph name="'+name+'">\n')
		for layer in layers:
			a = layer.split('#')
			if len(a) != 2:
				continue
			lname = a[0]
			color = a[1].upper()
			if not color in palette:
				palette[color] = len(palette)
			cid = palette[color]
			out.write('\t\t\t<layer colorID="'+str(cid)+'" name="'+lname+'"/>\n')
		out.write('\t\t</ColorGlyph>\n')

out.write('\t</COLR>\n')

out.write('\t<CPAL>\n\t\t<version value="0"/>\n\t\t<numPaletteEntries value="'+str(len(palette))+'"/>\n\t\t<palette index="0">\n')

bad_colors = {
	"#2A6797": "⚠ →#226699",
	"#3F7123": "⚠ →#3E721D",
	"#4289C1": "⚠ →#3B88C3",
	"#553986": "⚠ →#553788",
	"#5D9040": "⚠ →#5C913B",
	"#5DADEC": "⚠ →#55ACEE",
	"#642116": "⚠ →#662113",
	"#67757F": "⚠ →#66757F",
	"#7450A8": "⚠ →#744EAA",
	"#78B159": "⚠ →#77B255",
	"#8CCAF7": "⚠ →#88C9F9",
	"#9268CA": "⚠ →#9266CC",
	"#9AAAB4": "⚠ →#99AAB5",
	"#9D0522": "⚠ →#A0041E",
	"#A7D28B": "⚠ →#A6D388",
	"#AA8ED6": "⚠ →#AA8DD8",
	"#BB1A34": "⚠ →#BE1931",
	"#BDDDF4": "⚠ →#BBDDF5",
	"#BF6952": "⚠ →#C1694F",
	"#C6E4B5": "⚠ →#C6E5B3",
	"#CBB8E9": "⚠ →#CBB7EA",
	"#D79E84": "⚠ →#D99E82",
	"#DA2F47": "⚠ →#DD2E44",
	"#E4AAAB": "⚠ →#E6AAAA",
	"#E75A70": "⚠ →#EA596E",
	"#F18F26": "⚠ →#F4900C",
	"#F2ABBA": "⚠ →#F4ABBA",
	"#FCAB40": "⚠ →#FFAC33",
	"#FDCB58": "⚠ →#FFCC4D",
	"#FDD888": "⚠ →#FFD983",
	"#FEE7B8": "⚠ →#FFE8B6",
}

for color in palette:
	cid = palette[color]
	
	if "#"+color[0:6] in bad_colors:
		print("bad color", color, bad_colors["#"+color[0:6]])
	out.write('\t\t\t<color index="'+str(cid)+'" value="#'+color+'"/>\n')

out.write('\t\t</palette>\n\t</CPAL>\n')

out.write('</ttFont>\n')

font.generate('raw.ttf', flags=('opentype', 'omit-instructions'))
