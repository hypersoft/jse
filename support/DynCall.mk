DYNCALL_PKG = support/dyncall-0.7.tar.gz
DYNCALL_MAIN = support/dyncall-0.7
DYNCALL_HEAD = $(DYNCALL_INCLUDE)/dyncall.h
DYNCALL_BUILD_DIR = ../../../$(BUILD_OUTPUT)/dyncall
DYNCALL_INCLUDE = $(BUILD_DIST)/inc/dyncall

BUILD_DEFINES += -DJSE_DYNCALL
BUILD_LIBS += -ldl

ifeq (FALSE, $(shell $(dir) $(DYNCALL_MAIN)))
	TRUE != \
	tar --directory support -xf ${DYNCALL_PKG} && cd $(DYNCALL_MAIN) && ./configure;
endif

ifeq (FALSE, $(shell $(dir) $(DYNCALL_INCLUDE)))
	TRUE != mkdir -p $(DYNCALL_INCLUDE)
endif

ifeq (FALSE, $(shell $(file) $(DYNCALL_HEAD)))
	TRUE != \
	cp support/dyncall-0.7/dyncall/dyncall*.h $(DYNCALL_INCLUDE); \
	cp support/dyncall-0.7/dyncallback/dyncall*.h $(DYNCALL_INCLUDE); \
	cp support/dyncall-0.7/dynload/dynload*.h $(DYNCALL_INCLUDE);
endif

$(BUILD_OUTPUT)/dyncall/%o:
	@test -e $*.o || { cd $(DYNCALL_MAIN); ${MAKE} -s BUILD_DIR=$(DYNCALL_BUILD_DIR); rm -f $(DYNCAL_BUILD_DIR)/*_s*; }

