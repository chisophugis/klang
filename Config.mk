KLANG_LLVM_PATH := /home/hum/Projects/build/Debug+Asserts/bin
CC	    := $(KLANG_LLVM_PATH)/clang
CXX	    := $(KLANG_LLVM_PATH)/clang++
LD	    := ld

LLVM_CONFIG := $(KLANG_LLVM_PATH)/llvm-config

INCLUDES    := -I.\
	-I$(TOPDIR)/include

DEFINES	    :=

CFLAGS	    := -O2 -W -Wall $(INCLUDES) $(DEFINES) \
	`$(LLVM_CONFIG) --cflags`
CXXFLAGS    := -O2 -W -Wall $(INCLUDES) $(DEFINES) \
	-std=c++0x `$(LLVM_CONFIG) --cxxflags`

LDFLAGS	    += `$(LLVM_CONFIG) --ldflags --libs core`

##################################################################
.SUFFIXES : .o .c .cpp

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

##################################################################



