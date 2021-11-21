INSTALLDIR=/home/uhlrich/.local

CC  = gcc-7
CXX = g++-7
FC  = gfortran-7
VERSION=1.0

CFLAGS  = -fPIC -Wall -Wextra -std=c++17 -O3
CXXDEBUG = $(CXX) -g -D DEBUG

SRCDIR     = src
HEADDIR    = include
OBJDIR     = obj
MAKE       = /usr/bin/make

IPATH = -I./$(HEADDIR)
 
SRC      = $(wildcard $(SRCDIR)/*.cpp)
OBJ      = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o) 
OBJDEBUG = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%_debug.o) 

.PHONY: install clean
all:
	$(MAKE) release
	$(MAKE) debug

install:
	cp include/* $(INSTALLDIR)/include/marty

release: $(OBJ)
debug:   $(OBJDEBUG)

.PRECIOUS: $(OBJDIR)/%.o
.PRECIOUS: $(PROGOBJDIR)/%.o
.PRECIOUS: $(OBJDIR)/%_debug.o
.PRECIOUS: $(PROGOBJDIR)/%_debug.o

# Création des différents *.o à partir des *.cpp
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -o $@ -c $< $(IPATH)


# Version avec deboggage 
$(OBJDIR)/%_debug.o: $(SRCDIR)/%.cpp
	$(CXXDEBUG) $(CFLAGS) -o $@ -c $< $(IPATH)

clean:
	-rm -f $(OBJDIR)/*.o;
