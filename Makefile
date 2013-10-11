
DYNCALL := inc/dyncall
PKGCONFIG := $(shell pkg-config --cflags --libs javascriptcoregtk-3.0 seed)

JSTDEPENDS := Makefile ${DYNCALL} src/JSTools.inc $(shell echo src/JST*.[^sh] src/JSTools/*.inc) src/JSTInit.inc src/JSTNative.inc bin/UTF.o src/UTF.h
JSEDEPENDS := Makefile main.c bin/JSTools.o src/JSTools.h inc/*.inc

BUILDCOMMON := -fno-strict-aliasing -ldl ${PKGCONFIG} -Isrc -Iinc -O3 -march=native -DJSE_CODENAME='"Brigadier"' -DJSE_BUILDNO='"$(shell bin/buildnum -p)"'
ASM := -S -fverbose-asm -masm=intel

all: bin/jse

${DYNCALL}:
	@bin/dynhacker
	@echo ''

src/JSTInit.inc: src/JSTInit.js
	@bin/bin2inc JSTInitScript $< > $@;

src/JSTNative.inc: src/JSTNative.js
	@bin/bin2inc JSTNativeScript $< > $@;

bin/UTF.o: src/UTF.c
	@echo compiling Unicode Inc. UTF Conversion Routines
	gcc  ${BUILDCOMMON} -c src/UTF.c -o $@
	@echo ''

bin/JSTools.o: ${JSTDEPENDS}
	@echo compiling JSTools '(Brigadier)'
	gcc  ${BUILDCOMMON} -c src/JSTools.c -o $@
	@echo ''

bin/jse: ${JSEDEPENDS}
	@echo compiling JSE
	gcc  ${BUILDCOMMON} bin/*.o bin/*.a main.c -o $@
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

clean:
	@rm -rf bin/jse bin/*.a bin/*.o inc/dyncall *.s src/*.s src/JSTInit.inc
