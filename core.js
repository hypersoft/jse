#!bin/jse

for (item in sys.type) print.line('#define jse_type_'+item+' '+sys.type[item].flag);

//print.line(sys.type.bool.size)
