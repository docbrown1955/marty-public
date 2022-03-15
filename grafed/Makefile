INSTALLDIR=/usr/local

CC  = gcc-7
CXX = g++-7
FC  = gfortran-7
VERSION=1.0
STATICLIBRARY  = libgrafed.a
DYNAMICLIBRARY = libgrafed.so

CXXDEBUG          = $(CXX) -g -D DEBUG
CFLAGS           = -Wall -Wextra -Wpedantic -Wfatal-errors -std=c++17 -O3 -fPIC

# CXX               = gcc -m32 -Wall -Wextra -std=c++17 -O3 -fPIC -D COMPILE_32_BITS
# CXXDEBUG          = $(CXX) -g -D DEBUG -D COMPILE_32_BITS
# CFLAGS           = -I$(HEADDIR) -lm -lstdc++ 

SRCDIR     = src
HEADDIR    = include
OBJDIR     = obj
BINDIR     = bin
PROGDIR    = scripts
PROGOBJDIR = scripts/obj
MAKE       = /usr/bin/make
LIBDIR     = lib
JSONDIR  = ../jsonparser

IPATH      = -I./$(HEADDIR) -I$(JSONDIR)/$(HEADDIR)
LIBS       = -lgsl -lgslcblas
 
SRC      = $(wildcard $(SRCDIR)/*.cpp)
PROG     = $(wildcard $(PROGDIR)/*.cpp)
OBJ      = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o) 
OBJDEBUG = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%_debug.o) 
jsonSRC       = $(wildcard $(JSONDIR)/$(SRCDIR)/*.cpp)
jsonOBJ       = $(jsonSRC:$(JSONDIR)/$(SRCDIR)/%.cpp=$(JSONDIR)/$(OBJDIR)/%.o) 
jsonOBJDEBUG  = $(jsonSRC:$(JSONDIR)/$(SRCDIR)/%.cpp=$(JSONDIR)/$(OBJDIR)/%_debug.o) 
	
BIN      = $(subst .cpp,.x,$(subst $(PROGDIR)/,,$(PROG)))
BINDEBUG = $(subst .x,_debug.x,$(BIN))

.PHONY: install clean

all: lib grafed
grafed: lib
	$(MAKE) release
	$(MAKE) debug
	cp src/*.cpp include/*.h grafed-gui
	cp $(JSONDIR)/src/*.cpp $(JSONDIR)/include/*.h grafed-gui
	mkdir -p build-grafed-gui-Desktop_Qt_5_12_1_GCC_64bit-Debug;
	cd build-grafed-gui-Desktop_Qt_5_12_1_GCC_64bit-Debug; \
		qmake -makefile -o Makefile "CONFIG+=debug" "QMAKE_CXX=$(CXX)" "QMAKE_LINK=$(CXX)" ../grafed-gui/grafed-gui.pro; \
		sed -i=.foo 's/-std=c++.[^7]//g' Makefile; \
		sed -i=.foo 's/-std=gnu++..//g' Makefile; \
		sed -i=.foo 's/[^ ^\n]*libc++//g' Makefile; \
		rm *.foo;\
		$(MAKE) clean;\
		$(MAKE)
	mkdir -p build-grafed-gui-Desktop_Qt_5_12_1_GCC_64bit-Release;
	cd build-grafed-gui-Desktop_Qt_5_12_1_GCC_64bit-Release; \
		qmake -makefile -o Makefile "QMAKE_CXX=$(CXX)" "QMAKE_LINK=$(CXX)" ../grafed-gui/grafed-gui.pro;\
		sed -i=.foo 's/-std=c++.[^7]//g' Makefile; \
		sed -i=.foo 's/-std=gnu++..//g' Makefile; \
		sed -i=.foo 's/[^ ^\n]*libc++//g' Makefile; \
		rm *.foo;\
		$(MAKE) clean;\
		$(MAKE) 

lib: $(OBJ) $(OBJDEBUG) $(LIBDIR)/$(STATICLIBRARY) $(LIBDIR)/$(DYNAMICLIBRARY)
install:
	cp lib/*.a lib/*.so $(INSTALLDIR)/lib
	cp include/*.h $(INSTALLDIR)/include/marty
	bash export_grafed.sh build-grafed-gui-Desktop_Qt_5_12_1_GCC_64bit-Release $(INSTALLDIR)/bin/grafed

$(LIBDIR)/$(STATICLIBRARY): $(OBJ)
	ar rcs $@ $(OBJ) $(jsonOBJ)
$(LIBDIR)/$(DYNAMICLIBRARY): $(OBJ)
	$(CXX) -shared -o $@ $(OBJ) $(jsonOBJ) $(LIBS)

release: $(BIN)
debug:   $(BINDEBUG)

.PRECIOUS: $(OBJDIR)/%.o
.PRECIOUS: $(PROGOBJDIR)/%.o
.PRECIOUS: $(OBJDIR)/%_debug.o
.PRECIOUS: $(PROGOBJDIR)/%_debug.o

# Création des différents *.o à partir des *.cpp
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -o $@ -c $< $(IPATH)

# Compilation des différents programmes main
$(PROGOBJDIR)/%.o: $(PROGDIR)/%.cpp
	$(CXX) $(CFLAGS) -o $@ -c $< $(IPATH)

%.x: $(PROGOBJDIR)/%.o $(OBJ) 
	$(CXX) $(CFLAGS) -o $(BINDIR)/$@ $(jsonOBJ) $^ $(IPATH) $(LIBS)


# Version avec deboggage 
$(OBJDIR)/%_debug.o: $(SRCDIR)/%.cpp
	$(CXXDEBUG) $(CFLAGS) -o $@ -c $< $(IPATH)

$(PROGOBJDIR)/%_debug.o: $(PROGDIR)/%.cpp
	$(CXXDEBUG) $(CFLAGS) -o $@ -c $< $(IPATH)
 
%_debug.x:$(PROGOBJDIR)/%_debug.o  $(OBJDEBUG)
	$(CXXDEBUG) $(CFLAGS) -o $(BINDIR)/$@ $(jsonOBJDEBUG) $^ $(IPATH) $(LIBS)

clean:
	-rm -f $(OBJDIR)/*.o;
	-rm -f $(PROGOBJDIR)/*.o;
	-rm -f $(LIBDIR)/*.a $(LIBDIR)/*.so;
	-rm -f $(BINDIR)/*.x;
	-rm -f build-grafed-gui-Desktop_Qt_5_12_1_GCC_64bit-Debug/*
	-rm -f build-grafed-gui-Desktop_Qt_5_12_1_GCC_64bit-Release/*
