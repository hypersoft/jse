
DYNCALL := inc/dyncall
PKGCONFIG := $(shell pkg-config --silence-errors --cflags --libs javascriptcoregtk-3.0)
OPTIMIZE := -O3 -march=native
BUILDNO := $(shell bin/buildnum -p)
JSTDEPENDS := ${DYNCALL} $(shell echo src/JST*.[^sh] src/JSTools/*.inc) src/JSTInit.inc

all: bin/jse


${DYNCALL}:
	@bin/dynhacker
	@echo ''

src/JSTInit.inc: src/JSTInit.js
	@bin/bin2inc JSTInitScript $< > $@;

bin/JSTools.o: ${JSTDEPENDS}
	@echo compiling JSTools '(Brigadier)'
	gcc -DJSE_CODENAME='"Brigadier"' -DJSE_BUILDNO='"${BUILDNO}"' -Iinc -c src/JSTools.c `pkg-config --cflags --libs javascriptcoregtk-3.0` -o $@
	@echo ''

bin/jse: main.c bin/JSTools.o src/JSTools.h inc/*.inc
	@echo compiling JSE
	gcc  -I inc -I src -o "bin/jse" main.c bin/*.a bin/*.o ${OPTIMIZE} -lpthread -ldl ${PKGCONFIG}
	@bin/buildnum -q;
	@echo ''

install:
	@printf 'installing: '
	@cp -vt /bin bin/jse

source:
	gcc -E -DBUILDNO='"${BUILDNO}"' -I inc -I src main.c ${OPTIMIZE} -ldl ${PKGCONFIG}

src/JSTools.s: ${JSTDEPENDS}
	gcc -S -fverbose-asm -masm=intel -DBUILDNO='"${BUILDNO}"' -I inc -I src -o src/JSTools.s src/JSTools.c ${OPTIMIZE} -lpthread -ldl ${PKGCONFIG}

main.s: main.c src/JSTools.h
	gcc -S -fverbose-asm -masm=intel -DBUILDNO='"${BUILDNO}"' -I inc -I src -o main.s main.c ${OPTIMIZE} -lpthread -ldl ${PKGCONFIG}

asm: main.s src/JSTools.s

clean:
	@rm -rf bin/jse bin/*.a bin/*.o inc/dyncall *.s src/*.s src/JSTInit.inc
