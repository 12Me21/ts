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
			print(a, len(a))
			if len(a) != 2:
				continue
			lname = a[0]
			color = a[1]
			if not color in palette:
				palette[color] = len(palette)
			cid = palette[color]
			out.write('\t\t\t<layer colorID="'+str(cid)+'" name="'+lname+'"/>\n')
		out.write('\t\t</ColorGlyph>\n')

out.write('\t</COLR>\n')

out.write('\t<CPAL>\n\t\t<version value="0"/>\n\t\t<numPaletteEntries value="'+str(len(palette))+'"/>\n\t\t<palette index="0">\n')
for color in palette:
	cid = palette[color]
	out.write('\t\t\t<color index="'+str(cid)+'" value="#'+color+'"/>\n')

out.write('\t\t</palette>\n\t</CPAL>\n')

out.write('</ttFont>\n')

font.generate('raw.ttf', flags=('opentype', 'omit-instructions'))
