
APPLICATION := bin/jse

SOURCE := main.c $(shell echo src/*.c)

PKGS := javascriptcoregtk-3.0

PKGCONFIG := $(shell pkg-config --silence-errors --cflags --libs ${PKGS})

OPTIMIZE := -O3 -march=native -mtune=generic

DYNCALL := inc/dyncall

REQUIRES := Makefile ${DYNCALL} $(shell find src -type f -o -wholename '*.h' -o -wholename '*.js' -o -wholename '*.inc') $(shell find inc -type f -o -wholename '*.inc' -o -wholename '*.h')

all: ${APPLICATION}

${DYNCALL}:
	@bin/dynhacker
	@echo ''

inc/JSNative.inc: src/JSNative.js
	# JSNativeSupport is being reconstructed
	@bin/bin2inc JSNativeSupport src/JSNative.js > inc/JSNative.inc;

# This rule builds jse
${APPLICATION}: ${SOURCE} ${REQUIRES} inc/JSNative.inc
	@echo 'Validating required packages...'
	@pkg-config --print-errors --exists ${PKGS}
	@echo ''
	@echo 'Building jse...'
	gcc -I inc -I src -o "$@" ${SOURCE} bin/*.a ${OPTIMIZE} -lpthread -ldl ${PKGCONFIG}
	@echo ''

clean:
	@rm -rf bin/jse bin/*.a inc/dyncall inc/JSNative.inc;
