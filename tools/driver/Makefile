##===- klang/tools/Driver/Makefile -------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##
#
#  This implements the Driver part of executable for the Kaleidoscope front-end.
#
##===----------------------------------------------------------------------===##

#
# Indicate where we are relative to the top of the source tree.
#
LEVEL=../..

#
# Give the name of the tool.
#
TOOLNAME=klang

#
# List libraries that we'll need
# We use LIBS because sample is a dynamic library.
#
USEDLIBS = klangParse.a klangAST.a klangLex.a klangBuiltin.a
LINK_COMPONENTS = core jit native

#
# Include Makefile.common so we know what to do.
#
include $(LEVEL)/Makefile.common

