
APPLICATION := bin/jse
SOURCE := main.c $(shell echo src/*.c)
PKGS := javascriptcoregtk-3.0
PKGCONFIG := $(shell pkg-config --silence-errors --cflags --libs ${PKGS})
OPTIMIZE := -O3 -march=native
DYNCALL := inc/dyncall
BUILDNO := $(shell bin/buildnum -p)
REQUIRES := Makefile ${DYNCALL}

all: ${APPLICATION}

${DYNCALL}:
	@bin/dynhacker
	@echo ''

src/JSTInit.inc: src/JSTInit.js
	@bin/bin2inc JSTInitScript $< > $@;

# This rule builds jse
${APPLICATION}: ${SOURCE} ${REQUIRES} $(shell echo src/*.*) $(shell echo src/JSTools/*.*)
	@echo ''
	@echo 'Validating required packages...'
	@pkg-config --print-errors --exists ${PKGS}
	@echo ''
	gcc -DJSE_CODENAME='"Brigadier"' -DJSE_BUILDNO='"${BUILDNO}"' -I inc -I src -o "bin/jse" ${SOURCE} bin/*.a ${OPTIMIZE} -lpthread -ldl ${PKGCONFIG}
	@bin/buildnum -q;
	@echo ''

install:
	@printf 'installing: '
	@cp -vt /bin bin/jse

source:
	gcc -E -DBUILDNO='"${BUILDNO}"' -I inc -I src main.c ${OPTIMIZE} -lpthread -ldl ${PKGCONFIG}

src/JSTools.s:
	gcc -S -fverbose-asm -masm=intel -DBUILDNO='"${BUILDNO}"' -I inc -I src -o src/JSTools.s src/JSTools.c ${OPTIMIZE} -lpthread -ldl ${PKGCONFIG}

main.s:
	gcc -S -fverbose-asm -masm=intel -DBUILDNO='"${BUILDNO}"' -I inc -I src -o main.s main.c ${OPTIMIZE} -lpthread -ldl ${PKGCONFIG}

asm: main.s src/JSTools.s

clean:
	@rm -rf bin/jse bin/*.a inc/dyncall *.s src/*.s
