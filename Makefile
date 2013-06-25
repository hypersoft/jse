
APPLICATION := bin/jse

SOURCE := main.c

PKGS := javascriptcoregtk-3.0

PKGCONFIG := $(shell pkg-config --silence-errors --cflags --libs ${PKGS})

OPTIMIZE := -O3 -march=native -mtune=generic

DYNCALL := inc/dyncall

REQUIRES := ${DYNCALL} $(shell find src -type f -wholename '*.c' -o -wholename '*.h' -o -wholename '*.js' -o -wholename '*.inc') 

all: ${APPLICATION}

${DYNCALL}:
	@bin/dynhacker
	@echo ''

src/JSNative.inc: src/JSNative.js
	# JSNativeSupport is being reconstructed
	@bin/bin2inc JSNativeSupport src/JSNative.js > src/JSNative.inc;

# This rule builds jse
${APPLICATION}: ${SOURCE} ${REQUIRES} src/JSNative.inc
	@echo 'Validating required packages...'
	@pkg-config --print-errors --exists ${PKGS}
	@echo ''
	@echo 'Building jse...'
	gcc -I inc -o "$@" "${SOURCE}" bin/*.a ${OPTIMIZE} -lpthread -ldl ${PKGCONFIG}
	@echo ''

clean:
	@rm -rf bin/jse bin/*.a inc/dyncall src/JSNative.inc;
