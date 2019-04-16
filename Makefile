DEBUG = -g3

HERE != pwd
CURL_PROGRAM != bash -c 'type -p wget || echo wget'

DYNCALL_VER = dyncall-0.8
DYNCALL_PKG = ${DYNCALL_VER}.tar.gz
DYNCALL_URI = http://www.dyncall.org/r0.8/${DYNCALL_PKG}
DYNCALL_DOC_URI = http://www.dyncall.org/r0.8/manual.pdf

all: share/doc/${DYNCALL_VER}.pdf share/license/dyncall.txt bin/bin2inc plugins bin/jse bin/ghtml

share/doc/${DYNCALL_VER}.pdf: ${CURL_PROGRAM}
	@echo downloading ${DYNCALL_VER}.pdf
	@mkdir -p share/doc
	@wget -O "share/doc/${DYNCALL_VER}.pdf" "${DYNCALL_DOC_URI}"

data/${DYNCALL_PKG}: ${CURL_PROGRAM}
	@mkdir -p data;
	@echo downloading ${DYNCALL_URI}
	@wget -O "data/${DYNCALL_PKG}" "${DYNCALL_URI}"

share/license/dyncall.txt: data/${DYNCALL_PKG}
	@tar -xf data/${DYNCALL_PKG} 
	@( \
		cd ${DYNCALL_VER}; ./configure --prefix="${HERE}"; \
		make -s install \
	)
	@mkdir -p include/dyncall || true;
	@mv include/dyn*h include/dyncall
	@cp ${DYNCALL_VER}/LICENSE share/license/dyncall.txt
	rm -vrf ${DYNCALL_VER}

JSE_CFLAGS != pkg-config --cflags javascriptcoregtk-4.0
JSE_CFLAGS := $(DEBUG) $(JSE_CFLAGS)

bin/bin2inc: src/bin2inc/bin2inc.c
	@mkdir -p `dirname $@`
	gcc $< -o $@

obj/main/%.o: src/main/%.c
	@mkdir -p `dirname $@`
	gcc ${JSE_CFLAGS} -fPIC -I include -c $< -o $@

obj/main/JseMain.o: src/main/JseMain.c src/main/JseFunctions.c

JSE_OBJS = obj/main/JseMain.o obj/main/JseString.o obj/main/JseValue.o obj/main/JseObject.o obj/main/JseException.o obj/main/JseConstructor.o obj/main/JseOptionParser.o

JSE_LIBS = $(shell pkg-config --libs javascriptcoregtk-4.0) $(shell echo -ldl lib/*.a)
bin/jse: ${JSE_OBJS}
	gcc ${JSE_OBJS} -o $@ ${JSE_LIBS} -Wl,--export-dynamic 

GHTML_SOURCES != echo src/ghtml/*.c
GHTML_CONFIG != pkg-config --cflags --libs webkit2gtk-4.0
bin/ghtml: ${GHTML_SOURCES}
	gcc ${JSE_CFLAGS} ${GHTML_SOURCES} ${JSE_LIBS} -o bin/ghtml ${GHTML_CONFIG}

plugins: share/plugin/JseWebKit.so share/plugin/Ghtml.jso \
	 share/plugin/Environment.jso \
	 share/plugin/DynCall.jso share/plugin/Fork.jso \
	 share/plugin/MachineType.jso share/plugin/Shell.jso

obj/main/plugin/%.o: src/main/plugin/%.c
	@mkdir -p obj/main/plugin;
	gcc ${DEBUG} -fPIC -I include -c $< -o $@ ${JSE_CFLAGS}

obj/main/plugin/Ghtml.o: src/main/plugin/Ghtml.c
	@mkdir -p obj/main/plugin;
	@gcc ${DEBUG} -fPIC -I include -I ghtml ${GHTML_CONFIG} -c $< -o $@ ${JSE_CFLAGS}

share/plugin/%.jso: obj/main/plugin/%.o
	@mkdir -p share/plugin;
	gcc $< -fPIC -shared -o $@ ${JSE_LIBS}

JSE_WEBKIT_FLAGS != pkg-config --cflags --libs webkit2gtk-web-extension-4.0
share/plugin/JseWebKit.so: ${JSE_OBJS} src/main/JseWebKit.c
	@mkdir -p share/plugin
	gcc -I include ${JSE_WEBKIT_FLAGS} ${JSE_OBJS} src/main/JseWebKit.c -o $@ ${JSE_LIBS} -Wl,--export-dynamic -shared -ldl -fPIC

GNU_READLINE_FLAGS != pkg-config --cflags javascriptcoregtk-4.0
obj/plugin/Shell.o: src/main/plugin/Shell.c
	@mkdir -p obj/main/plugin;
	gcc ${GNU_READLINE_FLAGS} -fPIC -I include -c $< -o $@ ${JSE_CFLAGS}

share/plugin/Shell.jso: obj/main/plugin/Shell.o
	@mkdir -p share/plugin;
	gcc $< -fPIC -shared -o $@ $(shell pkg-config --libs javascriptcoregtk-4.0) -lreadline

clean:
	@rm -vf bin/* share/plugin/*
	@rm -vrf obj

scrub:
	@rm -vf share/doc/dyn*pdf data/dyncall*gz share/license/dyncall.txt
	@rm -vrf lib

ARCHIVE != date +archive/jse-%b-%d-%Y.tar.gz | tr [:upper:] [:lower:]
package:
	@mkdir -p archive
	@tar -C .. -czf ${ARCHIVE} jse --exclude='dyncall*.pdf' --exclude='dyncall.txt' --exclude='lib/*' --exclude='obj/*' --exclude='bin/*' --exclude='share/plugin/*' --exclude='data/*' --exclude='*dep.inc' --exclude='include/dyncall' --exclude='archive' --exclude .vscode --exclude='nbproject/private'
	@echo ${ARCHIVE}
	
PKG_CONFIG_PATH != pkg-config --variable pc_path pkg-config | cut -d : -f1
#SYS_INCLUDE_DIR != pkg-config --variable includedir javascriptcoregtk-4.0

install:
	@install -v jse.pc ${PKG_CONFIG_PATH}
	@install -vd /usr/include/jse /usr/share/jse
	@install -v bin/jse /bin
	@install -v bin/ghtml /bin
	@cp -vr include/* /usr/include/jse
	@cp -vr share/* /usr/share/jse

remove:
	@rm -vrf /bin/ghtml /bin/jse /usr/include/jse /usr/share/jse ${PKG_CONFIG_PATH}/jse.pc
