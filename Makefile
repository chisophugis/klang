.EXPORT_ALL_VARIABLES:

TARGET	  := klang
TOPDIR	  := $(shell /bin/pwd)
SUBDIRS	  := lib/Driver lib/Lex lib/Parse lib/AST
INCDIRS   := include/klang/AST include/klang/Driver include/klang/Lex include/klang/Parse

######################################

include $(TOPDIR)/Config.mk

all : compile $(OBJS)
	$(CXX) $(OBJS) $(addsuffix /built-in.o,$(SUBDIRS)) $(LDFLAGS) -o $(TARGET)

include $(TOPDIR)/Rules.mk


