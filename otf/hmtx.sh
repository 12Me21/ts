#ttx -f -o hmtx-raw.ttx -t hmtx test2.ttf
#node hmtx.js hmtx-raw.ttx >hmtx.ttx

fontforge -script make-colr.py
# ^ creates colr2.xml and raw.ttf

ttx -v -b -m raw.ttf -o faces-colr.ttf colr2.xml

fonttools subset faces-colr.ttf '*' --no-subset-tables+=COLR,CPAL,GSUB,gasp,name --canonical-order --no-legacy-cmap --drop-tables+=BASE,GDEF --layout-features='*' --name-IDs='*' --flavor=woff2
# gasp,
#todo: why is it keeping the legacy cmap! fuck!

# hi
