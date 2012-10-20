ifndef TOPDIR
	TOPDIR := ../..
endif

include $(TOPDIR)/Config.mk

TARGET   := built-in.o
SUBDIRS	 :=
OBJS	   := $(patsubst %.c, %.o, $(wildcard *.c))\
	$(patsubst %.cpp, %.o, $(wildcard *.cpp))


all : compile $(OBJS)
	$(LD) -r $(addsuffix /$(TARGET),$(SUBDIRS)) $(OBJS) -o $(TARGET)

include $(TOPDIR)/Rules.mk

