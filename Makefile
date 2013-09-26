
DYNCALL := inc/dyncall
PKGCONFIG := $(shell pkg-config --silence-errors --cflags --libs javascriptcoregtk-3.0)
JSTDEPENDS := ${DYNCALL} $(shell echo src/JST*.[^sh] src/JSTools/*.inc) src/JSTInit.inc

BUILDCOMMON := -lpthread -ldl ${PKGCONFIG} -Isrc -Iinc -O3 -march=native -DJSE_CODENAME='"Brigadier"' -DJSE_BUILDNO='"$(shell bin/buildnum -p)"'

all: bin/jse

${DYNCALL}:
	@bin/dynhacker
	@echo ''

src/JSTInit.inc: src/JSTInit.js
	@bin/bin2inc JSTInitScript $< > $@;

bin/JSTools.o: ${JSTDEPENDS}
	@echo compiling JSTools '(Brigadier)'
	gcc  ${BUILDCOMMON} -c src/JSTools.c -o $@
	@echo ''

bin/jse: Makefile main.c bin/JSTools.o src/JSTools.h src/JSTools/*.inc inc/*.inc
	@echo compiling JSE
	gcc  ${BUILDCOMMON} -o "bin/jse" main.c bin/*.a bin/*.o
	@bin/buildnum -q;
	@echo ''

install:
	@printf 'installing: '
	@cp -vt /bin bin/jse

source:
	gcc -E ${BUILDCOMMON} main.c -ldl ${PKGCONFIG}

src/JSTools.s: ${JSTDEPENDS}
	gcc -S -fverbose-asm -masm=intel ${BUILDCOMMON} -o src/JSTools.s src/JSTools.c

main.s: main.c src/JSTools.h
	gcc -S -fverbose-asm -masm=intel ${BUILDCOMMON} -o main.s main.c

asm: main.s src/JSTools.s

clean:
	@rm -rf bin/jse bin/*.a bin/*.o inc/dyncall *.s src/*.s src/JSTInit.inc
