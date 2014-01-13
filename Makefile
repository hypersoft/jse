
DYNCALL := inc/dyncall
PKGCONFIG := $(shell pkg-config --cflags --libs javascriptcoregtk-3.0)

JSTDEPENDS := Makefile ${DYNCALL} src/JSTools.inc $(shell echo src/JST*.[^sh] src/JSTools/*.inc src/sys/*.inc src/*.inc) src/JSTInit.inc src/JSTError.inc
JSEDEPENDS := Makefile main.c bin/JSTools.o src/JSTools.h src/JSTNative.inc inc/*.inc

BUILDCOMMON := -fno-strict-aliasing -Wl,--export-dynamic -ldl ${PKGCONFIG} -Isrc -Iinc -O3 -march=native -DJSE_VENDOR='"Hypersoft Systems"' -DJSE_CODENAME='"Redstone"' -DJSE_BUILDNO='"$(shell bin/buildnum -p)"'
ASM := -S -fverbose-asm -masm=intel

all: bin/jse

${DYNCALL}:
	@bin/dynhacker
	@echo ''

src/JSTError.inc: src/JSTError.js
	@bin/bin2inc JSTErrorScript $< > $@;

src/JSTInit.inc: src/JSTInit.js
	@bin/bin2inc JSTInitScript $< > $@;

bin/JSTools.o: ${JSTDEPENDS}
	@echo compiling Hypersoft Systems JSTools
	gcc -c src/JSTools.c -o $@ ${BUILDCOMMON}
	@echo ''

bin/jse: ${JSEDEPENDS}
	@echo compiling Hypersoft Systems JSE
	gcc  bin/*.o bin/*.a main.c -o $@ ${BUILDCOMMON} 
	@bin/buildnum -q;
	@echo ''

install:
	@printf 'installing: '
	@cp -vt /bin bin/jse

source:
	gcc -E ${BUILDCOMMON} main.c

src/JSTools.s: ${JSTDEPENDS}
	gcc ${ASM} ${BUILDCOMMON} src/JSTools.c -o $@

main.s: ${JSEDEPENDS}
	gcc ${ASM} ${BUILDCOMMON} main.c -o $@

asm: main.s src/JSTools.s

kb:
	mkdir kb

kb/glib-html-2.39.0: kb
	wget -O - https://developer.gnome.org/glib/glib-html-2.39.0.tar.gz | tar zxf - -C kb

glib-html-kb: kb/glib-html-2.39.0

clean:
	@rm -rf bin/jse bin/*.a bin/*.o inc/dyncall *.s src/*.s src/JSTInit.inc
