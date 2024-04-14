import fontforge
import json

font = fontforge.open("faces.sfd")
cmap = json.load(open("../cmap.json"))
cmap2 = dict()
for entry in cmap:
	cmap2[entry['ident']] = entry

for name in font:
	glyph = font[name]
	if name in cmap2:
		codes = cmap2[name]['codes'][0]
		print(name, cmap2[name])
	#if name[0] 
	
	
	#anyway then we want to parse uh glyph.comment, split by comma, then each entry is formatted as <layer glyph name>#<color>. so we also need to build a table of colors as well.  now what scares me is converting our existing xml into this comment format. we can do this but it requires extra work
