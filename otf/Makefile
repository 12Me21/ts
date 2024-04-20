all: faces-colr.subset.woff2

faces-colr.ttf: raw.ttf colr2.xml
	ttx -v -b -m raw.ttf -o $@ colr2.xml

colr2.xml raw.ttf &: faces.sfd make-colr.py
	fontforge -script make-colr.py

%.subset.woff2: %.ttf
	fonttools subset $< '*' --no-subset-tables+=COLR,CPAL,GSUB,gasp,name --canonical-order --no-legacy-cmap --drop-tables+=BASE,GDEF --layout-features='*' --name-IDs='*' --flavor=woff2
