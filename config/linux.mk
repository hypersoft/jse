#
# If you don't support something, throw an error
# If something may not work out as intended, throw a warning
# If the developer may need to know something you've done, throw an info
# 
# COPY, AND EDIT, NEVER SYMLINK!
# =================================================================================

# In order to keep this nice and pretty, you may add a system namespace directory
# under this one, as:
#
# mkdir config/$(SYSTEM_NAME).d
#
# This will afford you some place to store any config files you need on a per system
# basis.
#
# You may then include your files as such:
# include config/$(SYSTEM_NAME).d/some-config-1.4.mk
#

ifneq (FALSE, $(NOTICE))
NOTICE := $(shell { \
	printf 'JSE Linux Build Configuration: \n\n'; \
	printf '\t%s\n' \
		'(C) 2014 Triston J. Taylor <pc.wiz.tt@gmail.com>' \
	; \
	echo; \
} >&2)
endif

# Some systems use differing object and executable extensions.
# issue the proper extension or suffix here....

#if you aren't a unix developer, look over the following file and copy to your config file
include $(BUILD_HOME)config/unix.d/build-file-extensions.mk

# Machine architecture of the form: iX86, alpha, arm, 6800K, etc.
# See: http://sourceforge.net/p/predef/wiki/Architectures/ FOR common names

# DO NOT HARDCODE THIS UNLESS TESTING OR PERSONAL USE
SYSTEM_CPU_ARCH := $(shell uname -m)

# Version string, leave blank if undetected
SYSTEM_VERSION := $(shell uname -v)

# Location of include files. This is where include files get installed
SYSTEM_INCLUDE_PATH += /usr/include

# Location of lib files. Same as above
SYSTEM_SHARED_LIB_PATH = /usr/lib

# Location where apps are installed
SYSTEM_APPLICATION_PATH = /usr/bin

# Public temp data; MUST BE SUPPLIED
SYSTEM_GLOBAL_TEMP_PATH = /tmp

SYSTEM_DEFINES = -DXP_UNIX \
	   -DSVR4 -DSYSV -D_BSD_SOURCE \
	 -DPOSIX_SOURCE -DHAVE_LOCALTIME_R

ifeq (64, $(findstring 64, $(SYSTEM_CPU_ARCH)))
	SYSTEM_DEFINES += -DHAVE_VA_COPY -DVA_COPY=va_copy -DPIC
	SYSTEM_FLAGS += -fPIC
else
	ifeq (86, $(findstring 86, $(SYSTEM_CPU_ARCH)))
		SYSTEM_CPU_ARCH = x86
		SYSTEM_DEFINES += -DX86_LINUX
	else ifeq (alpha, $(SYSTEM_CPU_ARCH))
		# Ask the C compiler on alpha linux to let us work with denormalized
		# double values, which are required by the ECMA spec.
		SYSTEM_FLAGS += -mieee
	endif
endif

# Executable name or path to assembler
BUILD_AS = as

# Executable name or path to ranlib
BUILD_RANLIB = ranlib

# Executable name or path to cpp compiler
BUILD_CPP = g++

# Executable name or path to c compiler
BUILD_CC = gcc

# Executable name or path to strip utility. Use null to void this
BUILD_STRIP = strip

# =================================================================================
# Note you will want to apply your default flags, and then parse the following
# BUILD_OPTION_XXX options
#
# Flags for your C compiler
  BUILD_CFLAGS += -Wall -Wno-format -Wl,--export-dynamic -fverbose-asm \
  -MMD -march=native
#
# Flags for your Assembler
#  BUILD_ASFLAGS += -x assembler-with-cpp
#
# ================================================================================

ifdef DEBUG
	BUILD_CFLAGS += -g3
else
	ifndef BUILD_OPTION_OPTIMIZE
		BUILD_OPTION_OPTIMIZE = 0
	endif
	ifeq ($(BUILD_OPTION_OPTIMIZE), 0)
		# Flags for your C compiler
		BUILD_CFLAGS += -O3
		# Flags for your Assembler
		# BUILD_ASFLAGS +=
		# Flags for your CPP
		# BUILD_CPPFLAGS +=
	else ifeq ($(BUILD_OPTION_OPTIMIZE), 1)
		# User wants optimization level 1
		# Flags for your C compiler
		BUILD_CFLAGS += -O1
		# Flags for your Assembler
		# BUILD_ASFLAGS +=
		# Flags for your CPP
		# BUILD_CPPFLAGS +=
	else ifeq ($(BUILD_OPTION_OPTIMIZE), 2)
		# User wants optimization level 2
		# Flags for your C compiler
		BUILD_CFLAGS += -O2
		# Flags for your Assembler
		# BUILD_ASFLAGS +=
		# Flags for your CPP
		# BUILD_CPPFLAGS +=
	else ifeq ($(BUILD_OPTION_OPTIMIZE), 3)
		# User wants optimization level 3
		# Flags for your C compiler
		BUILD_CFLAGS += -O3
		# Flags for your Assembler
		# BUILD_ASFLAGS +=
		# Flags for your CPP
		# BUILD_CPPFLAGS +=
	# You can add your own codes here after...
	else # and then...
		ERROR := $(error "Unknown BUILD optimization code" $(BUILD_OPTION_OPTIMIZE))
	endif
endif

ifeq (TRUE, $(SYSTEM_HAS_PKG_CONFIG))
	# Check for NSPR and set 'er up if available
	ifeq (TRUE, $(shell { pkg-config --exists nspr; } $(BOOLSTR)))
		BUILD_CFLAGS += $(shell pkg-config --cflags nspr)
		BUILD_DEFINES += -DJS_THREADSAFE -DJS_USE_ONLY_NSPR_LOCKS
		BUILD_LIBS != pkg-config --libs nspr
	endif
endif

BUILD_LIBS += -lm -lc


