## See sim/Makefile.am
##
## Copyright (C) 1990-2023 Free Software Foundation, Inc.
## Written by Cygnus Support.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

%C%_libsim_a_SOURCES =
%C%_libsim_a_LIBADD = \
	$(common_libcommon_a_OBJECTS) \
	%D%/interp.o \
	$(patsubst %,%D%/%,$(SIM_NEW_COMMON_OBJS)) \
	$(patsubst %,%D%/dv-%.o,$(SIM_HW_DEVICES)) \
	%D%/modules.o \
	%D%/table.o
$(%C%_libsim_a_OBJECTS) $(%C%_libsim_a_LIBADD): %D%/hw-config.h

noinst_LIBRARIES += %D%/libsim.a

%D%/%.o: %D%/%.c
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

%D%/%.o: common/%.c
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

%C%_run_SOURCES =
%C%_run_LDADD = \
	%D%/nrun.o \
	%D%/libsim.a \
	$(SIM_COMMON_LIBS)

noinst_PROGRAMS += %D%/run

## List all generated headers to help Automake dependency tracking.
## NB: These .c files are only included by other .c files.  They are not
## compiled individually.  Hence they're really "header" files.
BUILT_SOURCES += \
	%D%/code.c \
	%D%/ppi.c
%C%_BUILD_OUTPUTS = \
	%D%/gencode$(EXEEXT) \
	%D%/table.c

## This makes sure build tools are available before building the arch-subdirs.
SIM_ALL_RECURSIVE_DEPS += $(%C%_BUILD_OUTPUTS)
%D%/modules.c: | $(%C%_BUILD_OUTPUTS)

%C%_gencode_SOURCES = %D%/gencode.c

# These rules are copied from automake, but tweaked to use FOR_BUILD variables.
%D%/gencode$(EXEEXT): $(%C%_gencode_OBJECTS) $(%C%_gencode_DEPENDENCIES) %D%/$(am__dirstamp)
	$(AM_V_CCLD)$(LINK_FOR_BUILD) $(%C%_gencode_OBJECTS) $(%C%_gencode_LDADD)

# gencode is a build-time only tool.  Override the default rules for it.
%D%/gencode.o: %D%/gencode.c
	$(AM_V_CC)$(COMPILE_FOR_BUILD) -c $< -o $@

%D%/code.c: %D%/gencode$(EXEEXT)
	$(AM_V_GEN)$< -x >$@

%D%/ppi.c: %D%/gencode$(EXEEXT)
	$(AM_V_GEN)$< -p >$@

%D%/table.c: %D%/gencode$(EXEEXT)
	$(AM_V_GEN)$< -s >$@

EXTRA_PROGRAMS += %D%/gencode
MOSTLYCLEANFILES += $(%C%_BUILD_OUTPUTS)
