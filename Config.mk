CC	    := gcc
CXX	    := g++
LD	    := ld

INCLUDES    := -I.\
	       -I$(TOPDIR)/include

DEFINES	    := 

CFLAGS	    := -O1 -W -Wall $(INCLUDES) $(DEFINES)
CXXFLAGS    := -O1 -W -Wall $(INCLUDES) $(DEFINES) -std=c++0x

LDFLAGS	    += 

##################################################################
.SUFFIXES : .o .c .cpp

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@
##################################################################



