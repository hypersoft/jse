
# MUST END IN BACKSLASH or == NULL
BUILD_HOME ?= .

BUILD_HOME != echo `basename $(BUILD_HOME)`/

ifeq (./, $(BUILD_HOME))
	BUILD_HOME = 
endif

include $(BUILD_HOME)support/main.mk

BUILD_DEFINES += $(SYSTEM_DEFINES) \
-DBUILD_DEVELOPER=$(USER) \
-DBUILD_VERSION_MAJOR=$(BUILD_MAJOR) \
-DBUILD_VERSION_MINOR=$(BUILD_MINOR) \
-DBUILD_VERSION_REVISION=$(THIS_BUILD_REVISION) \
-DBUILD_VERSION='"$(BUILD_MAJOR).$(BUILD_MINOR).$(THIS_BUILD_REVISION)"' \
-DBUILD_NUMBER=$(THIS_BUILD_NUMBER) \
-DBUILD_DATE=$(THIS_BUILD_DATE) \
-DBUILD_NAME="$(BUILD_NAME)" \

all: $(BUILD_DIST)/inc/jsapi.h library $(BUILD_SHELL)
	@echo DONE

SOURCES = $(shell cd $(BUILD_HOME)src; echo -n *.c)
-include $(SOURCES:%.c=$(BUILD_OUTPUT)/%.d)

$(BUILD_DIST)/inc/jsapi.h: $(BUILD_HOME)src/jsapi.h
	@cp $(BUILD_HOME)src/jsapi.h $(BUILD_DIST)/inc

include $(BUILD_HOME)support/MakeStats.mk $(BUILD_HOME)support/DynCall.mk

ifndef BUILD_PROFILE
BUILD_PROFILE = $(BUILD_CFLAGS) -I$(BUILD_OUTPUT) -I$(BUILD_DIST)/inc
endif

ifndef LINK_PROFILE
LINK_PROFILE = $(BUILD_DEFINES) $(BUILD_LIBS) $(BUILD_STATIC_LIBRARY)
endif

$(BUILD_OUTPUT)/%: %c
	$(BUILD_CC) -o $@ $(BUILD_PROFILE) $(BUILD_HOME)src/$*.c
	@echo
	
$(BUILD_OBJECT_WILDCARD):
	$(BUILD_CC) -o $@ -c $(BUILD_PROFILE) $(BUILD_HOME)src/$*.c $(BUILD_DEFINES)
	@echo

$(BUILD_CPU_CONFIG_OBJECT):
	$(BUILD_CC) -o $@ -c $(BUILD_PROFILE) $(BUILD_HOME)src/jscpucfg.c
	@echo

$(BUILD_CPU_CONFIG): $(BUILD_CPU_CONFIG_OBJECT)
	$(BUILD_CC) -o $@ $(BUILD_CPU_CONFIG_OBJECT)
	@echo

$(BUILD_OUTPUT)/jsautocfg.h: $(BUILD_CPU_CONFIG)
	@rm -f $@
	$(BUILD_CPU_CONFIG) > $@
	@echo

$(BUILD_SCANNER_OBJECT): $(BUILD_OUTPUT)/jsautokw.h $(BUILD_HOME)src/jskeyword.tbl

$(BUILD_KEYWORD_GENERATOR_OBJECT): $(BUILD_HOME)src/jskwgen.c $(BUILD_HOME)src/jskeyword.tbl
	$(BUILD_CC) -o $@ -c $(BUILD_PROFILE) $<
	@echo

$(BUILD_KEYWORD_GENERATOR): $(BUILD_KEYWORD_GENERATOR_OBJECT)
	$(BUILD_CC) -o $@ $(BUILD_PROFILE) $(LINKER_FLAGS) $^
	@echo

$(BUILD_OUTPUT)/jsautokw.h: $(BUILD_KEYWORD_GENERATOR) $(BUILD_HOME)src/jskeyword.tbl
	$(BUILD_KEYWORD_GENERATOR) $@
	@echo

$(BUILD_INVOKE_OBJECT): $(BUILD_HOME)src/jsinterp.h $(BUILD_HOME)src/jsinterp.c

$(BUILD_STATIC_LIBRARY): $(BUILD_LIBRARY_OBJECTS)
	@$(AR) r $(BUILD_STATIC_LIBRARY) $(BUILD_LIBRARY_OBJECTS)
	@$(BUILD_RANLIB) $(BUILD_STATIC_LIBRARY)
	@echo

$(BUILD_SHELL): $(BUILD_OUTPUT)/jsautocfg.h $(BUILD_STATIC_LIBRARY)
	$(BUILD_CC) -o $(BUILD_SHELL) $(BUILD_PROFILE) $(BUILD_HOME)src/js.c $(LINK_PROFILE)
	@printf '\nTarget: '
	@ls -sh $(BUILD_SHELL)
	-@$(push-stats)
	@echo

$(BUILD_SHARED_LIBRARY):  $(BUILD_OUTPUT)/jsautocfg.h $(BUILD_LIBRARY_OBJECTS)
	$(LD) -shared -o $@ $(BUILD_LIBRARY_OBJECTS)
	@printf '\nTarget: '
	@ls -sh $(BUILD_SHARED_LIBRARY)
	@$(push-stats)
	@echo

static: $(BUILD_STATIC_LIBRARY)
library: $(BUILD_SHARED_LIBRARY)
shell: $(BUILD_SHELL)

view-build-profile:
	@echo '$(BUILD_PROFILE)'

view-link-profile:
	@echo '$(LINK_PROFILE)'

delete:
	@rm -rf $(BUILD_DIST)

clean:
	@rm -rf $(BUILD_SHARED_LIBRARY) $(BUILD_STATIC_LIBRARY) $(BUILD_SHELL) || true

build-marginal: $(BUILD_SHELL)
	@$(BUILD_STRIP) $(BUILD_SHELL);
	@printf 'Marginal: '
	@ls -sh $(BUILD_SHELL)
	@echo

