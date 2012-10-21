CC	    := /home/hum/Projects/llvm31_pure/build/Debug+Asserts/bin/clang
CXX	    := /home/hum/Projects/llvm31_pure/build/Debug+Asserts/bin/clang++
LD	    := ld

INCLUDES    := -I.\
	       -I$(TOPDIR)/include

DEFINES	    := 

CFLAGS	    := -O2 -W -Wall $(INCLUDES) $(DEFINES) `/home/hum/Projects/llvm31_pure/build/Debug+Asserts/bin/llvm-config --cppflags --ldflags --libs core`
CXXFLAGS    := -O2 -W -Wall $(INCLUDES) $(DEFINES) -std=c++0x `/home/hum/Projects/llvm31_pure/build/Debug+Asserts/bin/llvm-config --cppflags --ldflags --libs core`

LDFLAGS	    += 

##################################################################
.SUFFIXES : .o .c .cpp

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@
##################################################################



