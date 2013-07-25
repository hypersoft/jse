
APPLICATION := bin/jse

SOURCE := main.c $(shell echo src/*.c)

PKGS := javascriptcoregtk-3.0

PKGCONFIG := $(shell pkg-config --silence-errors --cflags --libs ${PKGS})

OPTIMIZE := -O3 -march=native -mtune=generic

DYNCALL := inc/dyncall

BUILDNO := $(shell bin/buildnum -p)

REQUIRES := Makefile ${DYNCALL} $(shell find src -type f -o -wholename '*.h' -o -wholename '*.js' -o -wholename '*.inc') $(shell find inc -type f -o -wholename '*.inc' -o -wholename '*.h')

all: ${APPLICATION}

${DYNCALL}:
	@bin/dynhacker
	@echo ''

inc/JSNative.inc: src/JSNative/JSNative.js
	# JSNative.js is being reconstructed
	@bin/bin2inc JSNativeInit $< > $@;

inc/JSNativeClass.inc: src/JSNative/JSNativeClass.js
	# JSNativeClass.js is being reconstructed
	@bin/bin2inc JSNativeClassInit $< > $@;

inc/JSTools.inc: src/JSTools/JSTools.js
	# JSToolsSupport is being reconstructed
	@bin/bin2inc JSToolsSupport src/JSTools/JSTools.js > inc/JSTools.inc;


# This rule builds jse
${APPLICATION}: ${SOURCE} ${REQUIRES} inc/JSNativeClass.inc inc/JSNative*.inc inc/JSTools.inc
	@echo ''
	@echo 'Validating required packages...'
	@pkg-config --print-errors --exists ${PKGS}
	@echo ''
	@echo 'Building JSE' ${BUILDNO} ...
	@echo ''
	gcc -DBUILDNO=${BUILDNO} -I inc -I src -o "bin/jse" ${SOURCE} bin/*.a ${OPTIMIZE} -lpthread -ldl ${PKGCONFIG}
	@bin/buildnum -q;
	@echo ''

source:
	gcc -E -DBUILDNO=${BUILDNO} -I inc -I src -I src/JSNative ${SOURCE} ${OPTIMIZE} -lpthread -ldl ${PKGCONFIG}

clean:
	@rm -rf bin/jse bin/*.a inc/dyncall inc/JSNative.inc;
