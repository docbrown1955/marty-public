INSTALLDIR=/home/laptopgva/.local

CC  = gcc-7
CXX = g++-7
FC  = gfortran-7
VERSION        = 1.0
STATICLIBRARY  = libmarty.a
DYNAMICLIBRARY = libmarty.so

LTVERSION = 2.15

CXXDEBUG         = $(CXX) -g 
CXXFLAGS 	     = -Wall -Wextra -Wpedantic -std=c++17 -O3 -fPIC $(MARTY_CXX_FLAGS) 
CXXNOOPTIFLAGS = -Wall -Wextra -Wpedantic -std=c++17     -fPIC $(MARTY_CXX_FLAGS) 

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

MODELINCDIR   = models/include
SGLINCDIR     = sgl/include
 
INCPATH    = -I.. -I./$(HEADDIR) -I$(MODELINCDIR) -I../csl/$(HEADDIR)\
	     -I../jsonparser/$(HEADDIR) -I../grafed/$(HEADDIR) -I$(SGLINCDIR)\
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

models: $(modelOBJ) $(modelOBJDEBUG)
sgl: $(sglOBJ) $(sglOBJDEBUG)

lib: $(OBJ) $(OBJDEBUG) sgl models \
	$(LIBDIR)/$(STATICLIBRARY) $(LIBDIR)/$(DYNAMICLIBRARY)
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
sgl/$(OBJDIR)/%.o: sgl/$(SRCDIR)/%.cpp 
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(INCPATH)

# Création des différents *.o à partir des *.cpp
models/$(OBJDIR)/%.o: models/$(SRCDIR)/%.cpp 
	$(CXX) $(CXXNOOPTIFLAGS) -o $@ -c $< $(INCPATH)

# Création des différents *.o à partir des *.cpp
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp 
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(INCPATH)

# Compilation des différents programmes main
$(PROGOBJDIR)/%.o: $(PROGDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(INCPATH)

%.x: $(PROGOBJDIR)/%.o $(OBJ) $(cslOBJ) $(jsonOBJ) $(drawerOBJ) $(modelOBJ) $(sglOBJ)
	$(CXX) $(CXXFLAGS) -o $(BINDIR)/$@ $^ $(LIBPATH) $(LIBS)


# Création des différents *.o à partir des *.cpp
sgl/$(OBJDIR)/%_debug.o: sgl/$(SRCDIR)/%.cpp 
	$(CXXDEBUG) $(CXXFLAGS) -o $@ -c $< $(INCPATH)

# Création des différents *.o à partir des *.cpp
models/$(OBJDIR)/%_debug.o: models/$(SRCDIR)/%.cpp 
	$(CXXDEBUG) $(CXXNOOPTIFLAGS) -o $@ -c $< $(INCPATH)

# Version avec deboggage 
$(OBJDIR)/%_debug.o: $(SRCDIR)/%.cpp
	$(CXXDEBUG) $(CXXFLAGS) -o $@ -c $< $(INCPATH)

$(PROGOBJDIR)/%_debug.o: $(PROGDIR)/%.cpp
	$(CXXDEBUG) $(CXXFLAGS) -o $@ -c $< $(INCPATH)

%_debug.x:$(PROGOBJDIR)/%_debug.o  $(OBJDEBUG) $(cslOBJDEBUG) $(jsonOBJDEBUG) \
	$(drawerOBJDEBUG) $(modelOBJDEBUG) $(sglOBJDEBUG)
	$(CXXDEBUG) $(CXXFLAGS) -o $(BINDIR)/$@ $^ $(LIBPATH) $(LIBS)

# Library compilations
$(LIBDIR)/$(STATICLIBRARY): $(OBJ) $(sglOBJ) $(modelOBJ)
	ar rcs $@ $(OBJ) $(cslOBJ) $(jsonOBJ) $(drawerOBJ) $(modelOBJ) $(sglOBJ)
$(LIBDIR)/$(DYNAMICLIBRARY): $(OBJ) $(sglOBJ) $(modelOBJ)
	$(CXX) $(CXXFLAGS) -shared -o $@ $(OBJ) $(cslOBJ) $(jsonOBJ) $(drawerOBJ)\
	   $(modelOBJ) $(sglOBJ) $(LIBPATH) $(LIBS)


clean:
	-rm -f sgl/$(OBJDIR)/*.o;
	-rm -f models/$(OBJDIR)/*.o;
	-rm -f $(OBJDIR)/*.o;
	-rm -f $(PROGOBJDIR)/*.o;
	-rm -f lib/*.a lib/*.so
	-rm -f bin/*.x
