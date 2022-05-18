# --- variables defined by configure ---

SRC = ./src
QUADSUFFIX = 
PREFIX = .
LIBDIRSUFFIX = 64

EXE = 
DEF = -D
NOUNDERSCORE = 0
QUAD = 0
QUADSIZE = 16

FC = gfortran
FFLAGS = -fPIC -O3 -ffixed-line-length-none -fno-range-check \
  $(DEF)QUAD=$(QUAD) $(DEF)QUADSIZE=$(QUADSIZE) \
  $(DEF)U77EXT=0

CC = gcc-7
CFLAGS = -fPIC -O3 -g -fomit-frame-pointer -ffast-math -fPIC -Wall -Wno-unused-result -m64 \
  -DQUAD=$(QUAD) -DQUADSIZE=$(QUADSIZE) \
  -DNOUNDERSCORE=$(NOUNDERSCORE) \
  -DBIGENDIAN=0

CXX = g++-7
CXXFLAGS = -fPIC -O3 -g -fomit-frame-pointer -ffast-math -Wall -m64 -stdlib=libstdc++

ML = 0
MCC = mcc
MCFLAGS = -fPIC -st -b64
MCLIBS = -lpthread -lrt

LDFLAGS = -L/usr/lib/gcc/x86_64-linux-gnu/9/liblto_plugin.so -L/home/laptopgva/.local/lib/../lib -L/usr/lib/gcc/x86_64-linux-gnu/9 -L/usr/lib/gcc/x86_64-linux-gnu/9/../../../x86_64-linux-gnu -L/usr/lib/gcc/x86_64-linux-gnu/9/../../../../lib -L/lib/x86_64-linux-gnu -L/lib/../lib -L/usr/lib/x86_64-linux-gnu -L/usr/lib/../lib -L. -L/home/laptopgva/.local/lib -L/usr/lib/gcc/x86_64-linux-gnu/9/../../.. -lgfortran -lm -lgcc_s -lgcc -lquadmath -lm -lgcc_s -lgcc -lgcc_s -lgcc

PARALLEL = -j 4

# --- end defs by configure ---

BLD = build$(QUADSUFFIX)

LIBDIR = $(PREFIX)/lib$(LIBDIRSUFFIX)
INCLUDEDIR = $(PREFIX)/include
BINDIR = $(PREFIX)/bin

LIB = libooptools$(QUADSUFFIX).a
FE = lt$(QUADSUFFIX)$(EXE)
MFE = LoopTools$(QUADSUFFIX)$(EXE)
INCLUDE = $(BLD)/looptools.h $(BLD)/clooptools.h

ARGS = $(PARALLEL) \
  LIB="$(LIB)" \
  FE="$(FE)" \
  MFE="$(MFE)" \
  EXE="$(EXE)" \
  DEF="$(DEF)" \
  NOUNDERSCORE="$(NOUNDERSCORE)" \
  XFC="$(FC) $(FFLAGS) -I." \
  F90="$(F90)" \
  CC="$(CC)" \
  CFLAGS="$(CFLAGS)" \
  CXX="$(CXX)" \
  CXXFLAGS="$(CXXFLAGS)" \
  ML="$(ML)" \
  MCC="$(MCC)" \
  MCFLAGS="$(MCFLAGS)" \
  MCLIBS="$(MCLIBS)" \
  AR="$(AR)" \
  RANLIB="$(RANLIB)" \
  NM="$(NM)" \
  DLLTOOL="$(DLLTOOL)" \
  LDFLAGS="$(LDFLAGS)" \
  LIBPATH="$(LIBPATH)"


default all lib frontend mma: force
	cd $(BLD) && $(MAKE) $(ARGS) $@

install: lib frontend
	-mkdir $(PREFIX)
	-mkdir $(LIBDIR) $(BINDIR) $(INCLUDEDIR)
	cp -p $(BLD)/$(LIB) $(LIBDIR)
	cp -p $(INCLUDE) $(INCLUDEDIR)
	strip $(BLD)/$(FE)
	cp -p $(BLD)/fcc $(BLD)/$(FE) $(BINDIR)
	rm -f $(BINDIR)/f++
	ln -s fcc $(BINDIR)/f++
	test ! -f $(BLD)/$(MFE) || { \
	  test -z "$(EXE)" || ldd $(BLD)/$(MFE) | awk '/\/usr\/bin\/cyg/ { system("cp -p " $$3 " $(BINDIR)/"); }' ; \
	  strip $(BLD)/$(MFE) ; \
	  cp -p $(BLD)/$(MFE) $(BINDIR); \
	}

force: $(BLD)/timestamp

$(BLD)/timestamp:
	-mkdir $(BLD)
	find "`cd $(SRC) ; pwd`" -follow -exec ln -sf {} $(BLD) \;
	touch $(BLD)/timestamp

clean:
	rm -fr $(BLD)

