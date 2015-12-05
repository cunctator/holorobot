#
# This Makefile is from:
# http://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
# I interprit it as being in public domain.
#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

# define the C compiler to use
CC = gcc
CXX = g++

# define any compile-time flags
CFLAGS = --pedantic -Wall -g -std=c11 -O2
CXXFLAGS = --pedantic -Wall -g -std=c++11 -O2

# define any directories containing header files other than /usr/include
#
INCLUDES = -I./include

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
#LFLAGS = -L../lib

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = -lm -lstdc++

CSRCS =		platform/fileop.c

# define the C++ source files
CXXSRCS =	platform/motor.cpp \
		src/main.cpp

ALLSRCS  = $(CSRCS)
ALLSRCS += $(CXXSRCS)

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(CXXSRCS:.cpp=.o)
OBJS += $(CSRCS:.c=.o)

# define the executable file 
MAIN = holorobot

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all:	$(MAIN)
	@echo  holorobot has been compiled!

$(MAIN): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .cpp's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .cpp file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) $(shell find . -name '*.o') $(shell find . -name '*~') $(MAIN)

depend: $(ALLSRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
