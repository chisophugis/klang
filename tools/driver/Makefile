##===- projects/sample/tools/sample/Makefile ---------------*- Makefile -*-===##

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

