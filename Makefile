PKGCONFIG != pkg-config --cflags --libs javascriptcoregtk-3.0
BUILDNO != bash tool/buildnum -p

BUILDCOMMON := -Wl,--export-dynamic -ldl ${PKGCONFIG} -Isrc -Iinc -O3 -march=native -DJSE_VENDOR='"Hypersoft Systems"' -DJSE_CODENAME='"Hyperstone"' -DJSE_BUILDNO='"${BUILDNO}"'

#since this is a GCC/GNU/Linux file we only require headers for this spec.
JSTHeaders := $(shell echo inc/{jst,license,notice,jst-comp-gcc,jst-os-linux}.h)
JSTScripts := $(shell echo src/jst/{error,extend,flags,sys}.inc) # auto-built only
JSTSources := $(shell \
	echo src/jst/{sys/class,error,sys/memory}.c \
	src/jst.c \
)

all: bin/jse

/bin/bash:
	@printf "Attempting to create symlink to /bin/bash..\nEnter Root "
	@su root -c 'ln -s '`bash -c 'type -p bash'`' /bin/bash' && \
	printf "Success! You will need to run make again to satisfy make requirements\n" && false

bin:
	@printf '\nCreating output directory...\n'
	@mkdir bin

tool/bin2inc: tool/bin2inc.c
	@printf '\nBuilding Hypersoft Systems bin2inc...\n'
	gcc $< -o $@

inc/license.h: tool/bin2inc doc/license.txt
	@printf '\nBuilding Hypersoft Systems JSE License...\n'	
	tool/bin2inc SoftwareLicenseText < doc/license.txt > $@

inc/notice.h: tool/bin2inc doc/notice.txt
	@printf '\nBuilding Hypersoft Systems JSE Notice...\n'	
	tool/bin2inc SoftwareNoticeText < doc/notice.txt > $@

src/jst/type.inc: tool/bin2inc src/jst/script/type.js
	@printf '\nBuilding Hypersoft Systems JST Type Script...\n'	
	tool/bin2inc JSTTypeScript < src/jst/script/type.js > $@

src/jst/flags.inc: tool/bin2inc src/jst/script/flags.js
	@printf '\nBuilding Hypersoft Systems JST Flags Script...\n'	
	tool/bin2inc JSTFlagsScript < src/jst/script/flags.js > $@

src/jst/extend.inc: tool/bin2inc src/jst/script/extend.js
	@printf '\nBuilding Hypersoft Systems JST Extend Script...\n'	
	tool/bin2inc JSTExtendScript < src/jst/script/extend.js > $@

src/jst/error.inc: tool/bin2inc src/jst/script/error.js
	@printf '\nBuilding Hypersoft Systems JST Error Script...\n'	
	tool/bin2inc JSTErrorScript < src/jst/script/error.js > $@

src/jst/sys.inc: tool/bin2inc src/jst/script/sys.js
	@printf '\nBuilding Hypersoft Systems JST System Script...\n'	
	tool/bin2inc JSTSysScript < src/jst/script/sys.js > $@

bin/jst.o: ${JSTHeaders} ${JSTScripts} ${JSTSources}
	@printf '\nBuilding Hypersoft Systems JST...\n'
	gcc -c src/jst.c -o $@ ${BUILDCOMMON}

bin/jse: /bin/bash bin bin/jst.o src/jse.c
	@printf '\nBuilding Hypersoft Systems JSE...\n'
	gcc  bin/jst.o src/jse.c -o $@ ${BUILDCOMMON} 
	@tool/buildnum -q
	@echo ''

clean:
	@rm -vrf inc/license.h inc/notice.h tool/bin2inc ${JSTScripts} bin/*.o bin/jse

