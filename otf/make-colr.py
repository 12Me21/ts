import fontforge

font = fontforge.open("faces.sfd")

out = open('colr2.xml', 'w')

out.write('<?xml version="1.0" encoding="UTF-8"?>\n<ttFont sfntVersion="\\x00\\x01\\x00\\x00" ttLibVersion="4.34">\n\t<COLR>\n\t\t<version value="0"/>\n')

named = {
	"skin": ["#FFDC5D", "#F7DECE","#F3D2A2","#D5AB88","#AF7E57","#7C533E"],
	"dark": ["#F9CA55", "#EEC2AD","#E2C196","#CC9B7A","#9B6A49","#664131"],
	"line": ["#EF9645", "#E0AA94","#D2A077","#B78B60","#90603E","#583529"],
	"hair": ["#963b22", "#292F33","#FFE51E","#963B22","#60352A","#0B0200"], #"#FFAC33"
	"nose": ["#C1694F", "#C1694F","#C1694F","#C1694F","#915A34","#3D2E24"],
	"eyes": ["#662113", "#662113","#662113","#662113","#60352A","#000000"],
	"eyes1":["#662113", "#662113","#662113","#662113","#60352A","#000000"],
	"eyes2":["#662113", "#662113","#662113","#662113","#662113","#000000"],
	"lips": ["#DF1F32", "#DF1F32","#DF1F32","#DF1F32","#DF1F32","#DF1F32"],
}

palette = dict()

ref_usage = dict()

for name in font:
	glyph = font[name]
	# glyph types:
	# 1: layer. used as a layer in the COLR table. may also be a component
	# 2: component. used as a component in a layer
	# 3: character. mapped to a unicode character, or could replace one via a GSUB
	
	layers = glyph.comment.split("\n")
	if len(layers)>0 and '#' in layers[0]:
		out.write('\t\t<ColorGlyph name="'+name+'">\n')
		for layer in layers:
			a = layer.split('#')
			if len(a) != 2:
				continue
			lname = a[0]
			color = a[1]
			if color in named:
				color = named[color]
				color = color[0][1:] # temp
			if len(color)==6:
				color = color+"ff"
			color = color.upper()
			
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
