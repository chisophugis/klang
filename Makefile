.EXPORT_ALL_VARIABLES:

TARGET	  := klang
TOPDIR	  := $(shell /bin/pwd)
SUBDIRS	  := lib/Driver lib/Lex lib/Parse

######################################

include $(TOPDIR)/Config.mk

all : compile $(OBJS)
	$(CXX) $(OBJS) $(addsuffix /built-in.o,$(SUBDIRS)) $(LDFLAGS) -o $(TARGET)

include $(TOPDIR)/Rules.mk


