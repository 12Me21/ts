#ttx -f -o hmtx-raw.ttx -t hmtx test2.ttf
#node hmtx.js hmtx-raw.ttx >hmtx.ttx

ttx -v -b -m raw.ttf -o faces-colr.ttf colr.xml

fonttools subset faces-colr.ttf '*' --no-subset-tables+=COLR,CPAL,GSUB,gasp,name --canonical-order --no-legacy-cmap --drop-tables+=BASE,GDEF --flavor=woff2 --layout-features='*' --name-IDs='*'
# gasp,
#todo: why is it keeping the legacy cmap! fuck!

# hi
