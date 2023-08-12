#ttx -f -o hmtx-raw.ttx -t hmtx test2.ttf
#node hmtx.js hmtx-raw.ttx >hmtx.ttx

ttx -v -b -m raw.ttf -o faces-colr.ttf colr.xml

# TODO: remove unnecessary tables like GDEF etc.
