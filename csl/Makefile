INSTALLDIR=/home/laptopgva/.local2

CC  = gcc-7
CXX = g++-7
FC  = gfortran-7
VERSION        = 1.0
STATICLIBRARY  = libcsl.a
DYNAMICLIBRARY = libcsl.so

CXXDEBUG         = $(CXX) -g # -D DEBUG
CFLAGS           = -Wall -Wextra -Wpedantic -Wfatal-errors -std=c++17 -O3 -fPIC

SRCDIR           = src
HEADDIR          = include
OBJDIR           = obj
LIBDIR           = lib

INCLUDE          = -I$(HEADDIR) 
LIBS 		 = -lgsl -lgslcblas

 
SRC      = $(wildcard $(SRCDIR)/*.cpp)
OBJ      = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o) 

OBJDEBUG = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%_debug.o) 

all: csl
csl: $(OBJ) $(OBJDEBUG) $(LIBDIR)/$(STATICLIBRARY) $(LIBDIR)/$(DYNAMICLIBRARY)
install:
	mkdir -p $(INSTALLDIR)/include/csl
	echo "#include \"csl/csl.h\"" >> $(INSTALLDIR)/include/csl.h
	cp lib/*.so lib/*.a $(INSTALLDIR)/lib
	cp include/*.h $(INSTALLDIR)/include/csl

.PRECIOUS: $(OBJDIR)/%.o
.PRECIOUS: $(OBJDIR)/%_debug.o

# Création des différents *.o à partir des *.cpp
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -o $@ -c $<
$(LIBDIR)/$(STATICLIBRARY): $(OBJ)
	ar rcs $@ $(OBJ)
$(LIBDIR)/$(DYNAMICLIBRARY): $(OBJ)
	$(CXX) $(CFLAGS) -shared -o $@ $(OBJ) $(LIBS)


# Version avec debuggage gdb
$(OBJDIR)/%_debug.o: $(SRCDIR)/%.cpp
	$(CXXDEBUG) $(CFLAGS) $(INCLUDE) -o $@ -c $< 

clean:
	-rm -f $(OBJDIR)/*.o;
	-rm -f $(LIBDIR)/*.a $(LIBDIR)/*.so
