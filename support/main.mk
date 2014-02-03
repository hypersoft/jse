NOTICE ?= TRUE
INFO ?= TRUE

ifneq (FALSE, $(NOTICE))
	NOTICE := $(shell echo >&2)
else ifneq (FALSE, $(INFO))
	INFO := $(shell echo >&2)
endif

ifdef DEBUG
	ifeq (FALSE, $(DEBUG))
		REQUIRE != echo FALSE > $(BUILD_HOME)make.dbg
	else
		REQUIRE != echo TRUE > $(BUILD_HOME)make.dbg
	endif
endif

ifeq (TRUE, $(shell cat $(BUILD_HOME)make.dbg))
	BUILD_DIST = $(BUILD_HOME)debug
	BUILD_DEFINES += -DDEBUG
	ifneq (FALSE, $(INFO))
		INFO := $(shell printf 'Debug Mode\n\n' >&2)
	endif
else
	BUILD_DIST = $(BUILD_HOME)release
	ifneq (FALSE, $(INFO))
		INFO := $(shell printf 'Release Mode\n\n' >&2)
	endif
endif

include $(BUILD_HOME)support/MakeFunction.mk

ifeq (FALSE, $(shell $(dir) $(BUILD_DIST)))
	TRUE != mkdir -p $(BUILD_DIST)
endif

BUILD_OUTPUT ?= $(BUILD_DIST)/obj

ifeq (FALSE, $(shell $(dir) $(BUILD_OUTPUT)))
	TRUE != mkdir -p $(BUILD_OUTPUT)
endif

ifeq (FALSE, $(shell $(dir) $(BUILD_DIST)/inc))
	TRUE != mkdir -p $(BUILD_DIST)/inc
endif

SYSTEM_HAS_PKG_CONFIG != { type pkg-config; } $(BOOLSTR)

SYSTEM_NAME = $(subst /,_,$(shell uname -s | sed /\ /s//_/ | tr [A-Z] [a-z]))
SYSTEM_CONFIG = $(BUILD_HOME)config/$(SYSTEM_NAME).mk

# Instead of relying on guess work, we use finite system configuration files
ifeq (TRUE, $(shell $(file) $(SYSTEM_CONFIG)))
	include $(SYSTEM_CONFIG)
else
	ERROR := $(error unable to locate build configuration for $(SYSTEM_NAME))
endif

# This file is common to ALL SYSTEMS, it sets up input and output files, based on
# the system configuration specs
include $(BUILD_HOME)config/build-objects.mk

