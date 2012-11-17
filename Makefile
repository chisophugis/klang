##===- Makefile --------------------------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

#
# Indicates our relative path to the top of the project's root directory.
#
LEVEL = .
DIRS = lib tools
EXTRA_DIST = include

#
# Include the Master Makefile that knows how to build all.
#
include $(LEVEL)/Makefile.common

