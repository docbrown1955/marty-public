INSTALLDIR=/home/uhlrich/.local

CXX = g++-7
VERSION=1.0

CXXDEBUG          = $(CXX) -g -D DEBUG
CFLAGS           = -Wall -Wextra -Wpedantic -std=c++17 -O3 -fPIC #-DDEBUGLOG

SRCDIR     = src
HEADDIR    = include
OBJDIR     = obj

IPATH      = -I./$(HEADDIR) -I../../csl/include -I ../../marty/include \
						 -I../../jsonparser/include -I../../grafed/include
 
SRC      = $(wildcard $(SRCDIR)/*.cpp)
OBJ      = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o) 
OBJDEBUG = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%_debug.o) 
	
.PHONY: install clean

all: $(OBJ) $(OBJDEBUG) 

install:
	cp include/* $(INSTALLDIR)/include/marty

.PRECIOUS: $(OBJDIR)/%.o
.PRECIOUS: $(OBJDIR)/%_debug.o

# Création des différents *.o à partir des *.cpp
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -o $@ -c $< $(IPATH)

# Version avec deboggage 
$(OBJDIR)/%_debug.o: $(SRCDIR)/%.cpp
	$(CXXDEBUG) $(CFLAGS) -o $@ -c $< $(IPATH)

clean:
	-rm -f $(OBJDIR)/*.o;
