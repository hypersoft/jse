
# ==================================================================================
#
# This software is provided TO YOU without any warranty, guarantee, or liability
# resulting in use, misuse, abuse or distribution.
#
# YOU ARE FREE TO COPY, DISTRIBUTE OR MODIFY THIS CODE FREE OF CHARGE, PENALTY,
# OR DONATION PROVIDED THE FOLLOWING CONDITIONS ARE MET:
#
#  1) DO NOT REMOVE OR MODIFY THE PRECEDING NOTICE.
#  2) DO NOT REMOVE OR MODIFY THE FOLLOWING NOTICE.
#

ifneq (FALSE, $(NOTICE))
NOTICE := $(shell { \
	printf 'MakeFunction: \n\n'; \
	printf '\t%s\n' \
		'(C) 2014 Triston J. Taylor <pc.wiz.tt@gmail.com>' \
	; \
	echo; \
} >&2)
endif

#
# ==================================================================================
#
# You may modify the license in any way you wish provided the following conditions
# are met:
#
#  1) You obtain written permission from <pc.wiz.tt@gmail.com>.
#  2) You make a contribution of $5.00 or more to support the ORIGINAL works of THIS
#     copyright holder.
#
# PayPal: via e-mail or via web
# https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=DG3H6F8DSG4BC
#
# ==================================================================================
#    Please indicate "MakeFunction" as your purpose and provide ANY contact address.
# ==================================================================================

# caveats:
# can't single quote anything in function body,
# must terminate all statements
# must escape all shell variables: $$varname
# $0 doesn't refer to anything useful

# This "macro" returns boolean exit status as a string
BOOLSTR = 1>& /dev/null 2>&- && printf TRUE || printf FALSE

# This "macro" returns exit status
EXITNUM = 1>& /dev/null 2>&- && printf 0 || printf $$?


define Function
$(SHELL) -c '{ 
endef
endFunction = }' --

define math
$(Function)
	expr $$1 "$$2" $$3;
$(endFunction)
endef

# example: constant math calc using expr
# ANSWER != $(math) 6 "*" 2;

define file
$(Function)
	test -e "$$1" $(BOOLSTR);
$(endFunction)
endef

# example: constant TRUE OR FALSE, STATUS OF FILE EXISTED
# ANSWER != $(file) makefile.mk;

define dir
$(Function)
	test -d "$$1" $(BOOLSTR);
$(endFunction)
endef

# example: state TRUE OR FALSE, STATUS OF PATH IS A DIRECTORY
# ifeq (FALSE, $(shell $(dir) $(PATH))
# ...
# endif

define script
$(SHELL) "$(strip $(1))"
endef

# example:
# STDOUT != $(call script, some/on/disk) arg1 arg2 ...;

