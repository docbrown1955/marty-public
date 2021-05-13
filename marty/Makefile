INSTALLDIR=/home/uhlrich/.local

CC = gcc-7
CXX = g++-7
FC = gfortran-7
VERSION        = 1.0
STATICLIBRARY  = libmarty.a
DYNAMICLIBRARY = libmarty.so

LTVERSION = 2.15

CXXDEBUG         = $(CXX) -g # -D DEBUG
CXXFLAGS 	 = $(MARTY_CXX_FLAGS) -Wall -Wextra -Wpedantic -std=c++17 -O3 -fPIC -Wfatal-errors -DDEBUGLOG

SRCDIR     = src
HEADDIR    = include
OBJDIR     = obj
BINDIR     = bin
PROGDIR    = scripts
EXAMPLEDIR = examples
PROGOBJDIR = scripts/obj
EXAMPLEOBJDIR = examples/obj
LIBDIR     = lib
LOOPDIR    = dependencies/LoopTools-$(LTVERSION)/x86_64-Linux/include
LOOPLIBDIR = dependencies/LoopTools-$(LTVERSION)/x86_64-Linux/lib64
LOOPLIB    = lib/libooptools.a
PJFRYDIR   = dependencies/PJFry/src
PJFRYLIBDIR = dependencies/PJFry/src/.libs
PJFRYLIB   = lib/libpjfry.a
GSLINCDIR  = dependencies/gsl-2.6/gsl
GSLLIBDIR  = dependencies/gsl-2.6/.libs
GSLCBLASLIBDIR = dependencies/gsl-2.6/cblas/.libs
GSLLIB = lib/libgsl.a
GSLCBLASLIB = lib/libgslcblas.a
MAKE       = /usr/bin/make

MODELDIR   = models/include
SGLDIR     = sgl/include
 
INCPATH    = -I.. -I./$(HEADDIR) -I$(MODELDIR) -I../csl/$(HEADDIR)\
	     -I../jsonparser/$(HEADDIR) -I../grafed/$(HEADDIR) -I$(SGLDIR)\
	     -I../dependencies/LoopTools-2.15/include
LIBPATH    = -L../dependencies/LoopTools-2.15/build
INCLIBPATH = $(INCPATH) $(LIBPATH)
ifneq (,$(findstring clang,$(CXX)))
	LIBS       = -looptools -lgsl -lgslcblas -lquadmath 
else
	LIBS       = -looptools -lgsl -lgslcblas -lgfortran -lquadmath 
endif
 
SRC      = $(wildcard $(SRCDIR)/*.cpp)
INC      = $(wildcard $(HEADDIR)/*.h)
PROG     = $(wildcard $(PROGDIR)/*.cpp)
EXAMPLES = $(wildcard $(EXAMPLEDIR)/*.cpp)
OBJ      = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o) 
OBJDEBUG = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%_debug.o) 

cslSRC      = $(wildcard ../csl/$(SRCDIR)/*.cpp)
cslOBJ      = $(cslSRC:../csl/$(SRCDIR)/%.cpp=../csl/$(OBJDIR)/%.o) 
cslOBJDEBUG = $(cslSRC:../csl/$(SRCDIR)/%.cpp=../csl/$(OBJDIR)/%_debug.o) 

drawerSRC      = $(wildcard ../grafed/$(SRCDIR)/*.cpp)
drawerOBJ      = $(drawerSRC:../grafed/$(SRCDIR)/%.cpp=../grafed/$(OBJDIR)/%.o) 
drawerOBJDEBUG = $(drawerSRC:../grafed/$(SRCDIR)/%.cpp=../grafed/$(OBJDIR)/%_debug.o) 

jsonSRC      = $(wildcard ../jsonparser/$(SRCDIR)/*.cpp)
jsonOBJ      = $(jsonSRC:../jsonparser/$(SRCDIR)/%.cpp=../jsonparser/$(OBJDIR)/%.o) 
jsonOBJDEBUG = $(jsonSRC:../jsonparser/$(SRCDIR)/%.cpp=../jsonparser/$(OBJDIR)/%_debug.o) 

modelSRC      = $(wildcard models/src/*.cpp)
modelOBJ      = $(modelSRC:models/src/%.cpp=models/obj/%.o) 
modelOBJDEBUG = $(modelSRC:models/src/%.cpp=models/obj/%_debug.o) 

sglSRC      = $(wildcard sgl/src/*.cpp)
sglOBJ      = $(sglSRC:sgl/src/%.cpp=sgl/obj/%.o) 
sglOBJDEBUG = $(sglSRC:sgl/src/%.cpp=sgl/obj/%_debug.o) 
	
BIN      = $(subst .cpp,.x,      $(subst $(PROGDIR)/,,$(PROG)))
BINDEBUG = $(subst .cpp,_debug.x,$(subst $(PROGDIR)/,,$(PROG)))

.PHONY: install clean models sgl
all: models sgl lib release debug

marty: lib

models:
	cd models; $(MAKE) all
sgl:
	cd sgl;   $(MAKE) all

lib: $(OBJ) $(OBJDEBUG) $(LIBDIR)/$(STATICLIBRARY) $(LIBDIR)/$(DYNAMICLIBRARY)\
	$(modelOBJ) $(modelOBJDEBUG) $(sglOBJDEBUG)
install:
	mkdir -p $(INSTALLDIR)/include/marty
	echo "#include \"marty/marty.h\"" >> $(INSTALLDIR)/include/marty.h
	cp include/*.h $(INSTALLDIR)/include/marty
	cp models/include/* $(INSTALLDIR)/include/marty
	cp sgl/include/* $(INSTALLDIR)/include/marty
	cp lib/*.a lib/*.so $(INSTALLDIR)/lib

release: $(BIN)
debug:   $(BINDEBUG)

.PRECIOUS: $(OBJDIR)/%.o
.PRECIOUS: $(PROGOBJDIR)/%.o
.PRECIOUS: $(OBJDIR)/%_debug.o
.PRECIOUS: $(PROGOBJDIR)/%_debug.o
.PRECIOUS: models/obj/%.o
.PRECIOUS: models/obj/%_debug.o

# Création des différents *.o à partir des *.cpp
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp 
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(INCPATH)

# Compilation des différents programmes main
$(PROGOBJDIR)/%.o: $(PROGDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(INCPATH)

%.x: $(PROGOBJDIR)/%.o $(OBJ) $(cslOBJ) $(jsonOBJ) $(drawerOBJ) $(modelOBJ) $(sglOBJ)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/$@ $^ $(LIBPATH) $(LIBS)


# Version avec deboggage 
$(OBJDIR)/%_debug.o: $(SRCDIR)/%.cpp
	$(CXXDEBUG) $(CXXFLAGS) -o $@ -c $< $(INCPATH)

$(PROGOBJDIR)/%_debug.o: $(PROGDIR)/%.cpp
	$(CXXDEBUG) $(CXXFLAGS) -o $@ -c $< $(INCPATH)

%_debug.x:$(PROGOBJDIR)/%_debug.o  $(OBJDEBUG) $(cslOBJDEBUG) $(jsonOBJDEBUG) \
	$(drawerOBJDEBUG) $(modelOBJDEBUG) $(sglOBJDEBUG)
	$(CXXDEBUG) $(CXXFLAGS) -o $(BINDIR)/$@ $^ $(LIBPATH) $(LIBS)

# Library compilations
$(LIBDIR)/$(STATICLIBRARY): $(OBJ)
	ar rcs $@ $(OBJ) $(cslOBJ) $(jsonOBJ) $(drawerOBJ) $(modelOBJ) $(sglOBJ)
$(LIBDIR)/$(DYNAMICLIBRARY): $(OBJ)
	$(CXX) $(CXXFLAGS) -shared -o $@ $(OBJ) $(cslOBJ) $(jsonOBJ) $(drawerOBJ)\
	   $(modelOBJ) $(sglOBJ) $(LIBPATH) $(LIBS)


clean:
	cd models/&& $(MAKE) clean;
	cd sgl/&&    $(MAKE) clean;
	-rm -f $(OBJDIR)/*.o;
	-rm -f $(PROGOBJDIR)/*.o;
	-rm -f lib/*.a lib/*.so
	-rm -f bin/*.x
