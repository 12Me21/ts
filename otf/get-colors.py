import fontforge
import json
import xml.parsers.expat

parser = xml.parsers.expat.ParserCreate()
colorglyphs = dict()
ccg = None
colors = dict()
def start_element(name, attrs):
	global ccg
	if name=='ColorGlyph':
		ccg = []
		colorglyphs[attrs['name']] = ccg
	elif name=='layer':
		ccg += [[attrs['name'], int(attrs['colorID'])]]
	elif name=='color':
		colors[int(attrs['index'])] = attrs['value']
	
def end_element(name):
	global ccg
	if name=='ColorGlyph':
		ccg = None
	
parser.StartElementHandler = start_element
parser.EndElementHandler = end_element
parser.ParseFile(open("colr.xml","rb"))

#print(colorglyphs)
#exit()

font = fontforge.open("faces.sfd")
#cmap = json.load(open("../cmap.json"))
#cmap2 = dict()
#for entry in cmap:
#	cmap2[entry['ident']] = entry

pal = dict()

for name in font:
	glyph = font[name]
	if name in colorglyphs:
		ccg = colorglyphs[name]
		glyph.comment = "\n".join([layer[0] + colors[layer[1]] for layer in ccg])
		print("name", glyph.comment)
	#if name in cmap2:
	#	codes = cmap2[name]['codes'][0]
	#	print(name, cmap2[name])
	#if name[0] 
	
font.save("faces-test.sfd")
	#anyway then we want to parse uh glyph.comment, split by comma, then each entry is formatted as <layer glyph name>#<color>. so we also need to build a table of colors as well.  now what scares me is converting our existing xml into this comment format. we can do this but it requires extra work
