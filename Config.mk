CC	    := /home/abuild/Projects/llvm32/build/Release+Asserts/bin/clang
CXX	    := /home/abuild/Projects/llvm32/build/Release+Asserts/bin/clang++
LD	    := ld

LLVM_CONFIG := /home/abuild/Projects/llvm32/build/Release+Asserts/bin/llvm-config

INCLUDES    := -I.\
	-I$(TOPDIR)/include

DEFINES	    :=

CFLAGS	    := -O2 -W -Wall $(INCLUDES) $(DEFINES) \
	`$(LLVM_CONFIG) --cflags`
CXXFLAGS    := -O2 -W -Wall $(INCLUDES) $(DEFINES) \
	-std=c++0x `$(LLVM_CONFIG) --cxxflags`

LDFLAGS	    += -rdynamic `$(LLVM_CONFIG) --ldflags --libs core jit native`

##################################################################
.SUFFIXES : .o .c .cpp

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

##################################################################



