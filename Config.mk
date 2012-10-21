CC	    := /home/hum/Projects/llvm31_pure/build/Debug+Asserts/bin/clang
CXX	    := /home/hum/Projects/llvm31_pure/build/Debug+Asserts/bin/clang++
LD	    := ld

LLVM_CONFIG := /home/hum/Projects/llvm31_pure/build/Debug+Asserts/bin/llvm-config

INCLUDES    := -I.\
	       -I$(TOPDIR)/include

DEFINES	    := 

CFLAGS	    := -O2 -W -Wall $(INCLUDES) $(DEFINES) `$(LLVM_CONFIG) --cflags`
CXXFLAGS    := -O2 -W -Wall $(INCLUDES) $(DEFINES) -std=c++0x `$(LLVM_CONFIG) --cxxflags`

LDFLAGS	    += `$(LLVM_CONFIG) --ldflags --libs core`

##################################################################
.SUFFIXES : .o .c .cpp

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@
##################################################################



