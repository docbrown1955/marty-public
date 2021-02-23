default: frontend mma$(ML)

all: frontend mma1

frontend: lib $(FE)

mma1: lib $(MFE)

mma0 lib: $(LIB) clooptools.h fcc


.SUFFIXES:

.SUFFIXES: .F .f90 .c


OBJS = $(OBJS-quad) \
  Aget.o AgetC.o \
  ffxa0.o ffca0.o \
  Bget.o BgetC.o Bcoeff.o BcoeffC.o \
  BcoeffAD.o BcoeffFF.o BcoeffFFC.o \
  ffxb0.o ffcb0.o ffxb1.o ffcb1.o \
  ffxb2p.o ffcb2p.o \
  ffxdb0.o ffcdb0.o ffxdb1.o \
  ffdel2.o ffcel2.o \
  C0func.o C0funcC.o Cget.o CgetC.o \
  ffxc0.o ffcc0.o ffxc0i.o ffxc0p0.o \
  ffxc0p.o ffcc0p.o ffdxc0.o ffdcc0.o \
  ffdel3.o ffcel3.o \
  D0func.o D0funcC.o D0z.o Dget.o DgetC.o \
  ffxd0.o ffxd0h.o ffxd0i.o ffxd0p.o \
  ffxd0m0.o ffxd0tra.o ffxdbd.o ffdel4.o ffd0c.o \
  ffTn.o ffT13.o ffS2.o ffS3n.o ffRn.o \
  E0func.o E0funcC.o Eget.o EgetC.o \
  Ecoeffa.o EcoeffaC.o Ecoeffb.o EcoeffbC.o \
  ffxe0.o ffdel5.o \
  ini.o auxCD.o solve.o solveC.o \
  Dump.o DumpC.o Li2.o Li2C.o Li2omx.o Li2omxC.o \
  cache.o ffinit.o \
  ffxli2.o ffcli2.o ffxxyz.o ffcxyz.o \
  ffcrr.o ffcxr.o fftran.o ffabcd.o ff2dl2.o \
  ffcxs3.o ffcxs4.o ffdcxs.o ffbndc.o

FFINC = ff.h $(OBJS-quad)

LTINC = defs.h lt.h $(FFINC)

CFC = $(XFC) $(DEF)COMPLEXPARA

qcomplex.o qcomplex.mod: qcomplex.f90
	$(F90) -O -c -o qcomplex.o qcomplex.f90

Aget.o: Aget.F $(LTINC)
	$(XFC) -c -o Aget.o Aget.F
AgetC.o: Aget.F $(LTINC)
	$(CFC) -c -o AgetC.o Aget.F
ffxa0.o: ffxa0.F $(FFINC)
	$(XFC) -c -o ffxa0.o ffxa0.F
ffca0.o: ffca0.F $(FFINC)
	$(XFC) -c -o ffca0.o ffca0.F

Bget.o: Bget.F $(LTINC)
	$(XFC) -c -o Bget.o Bget.F
BgetC.o: Bget.F $(LTINC)
	$(CFC) -c -o BgetC.o Bget.F
Bcoeff.o: Bcoeff.F $(LTINC)
	$(XFC) -c -o Bcoeff.o Bcoeff.F
BcoeffC.o: BcoeffC.F $(LTINC)
	$(XFC) -c -o BcoeffC.o BcoeffC.F
BcoeffAD.o: BcoeffAD.F $(LTINC)
	$(XFC) -c -o BcoeffAD.o BcoeffAD.F
BcoeffFF.o: BcoeffFF.F $(LTINC)
	$(XFC) -c -o BcoeffFF.o BcoeffFF.F
BcoeffFFC.o: BcoeffFF.F $(LTINC)
	$(CFC) -c -o BcoeffFFC.o BcoeffFF.F
ffxb0.o: ffxb0.F $(FFINC)
	$(XFC) -c -o ffxb0.o ffxb0.F
ffcb0.o: ffcb0.F $(FFINC)
	$(XFC) -c -o ffcb0.o ffcb0.F
ffxb1.o: ffxb1.F $(FFINC)
	$(XFC) -c -o ffxb1.o ffxb1.F
ffcb1.o: ffcb1.F $(FFINC)
	$(XFC) -c -o ffcb1.o ffcb1.F
ffxb2p.o: ffxb2p.F $(FFINC)
	$(XFC) -c -o ffxb2p.o ffxb2p.F
ffcb2p.o: ffcb2p.F $(FFINC)
	$(XFC) -c -o ffcb2p.o ffcb2p.F
ffxdb0.o: ffxdb0.F $(FFINC)
	$(XFC) -c -o ffxdb0.o ffxdb0.F
ffcdb0.o: ffcdb0.F $(FFINC)
	$(XFC) -c -o ffcdb0.o ffcdb0.F
ffxdb1.o: ffxdb1.F $(FFINC)
	$(XFC) -c -o ffxdb1.o ffxdb1.F
ffdel2.o: ffdel2.F $(FFINC)
	$(XFC) -c -o ffdel2.o ffdel2.F
ffcel2.o: ffcel2.F $(FFINC)
	$(XFC) -c -o ffcel2.o ffcel2.F

C0func.o: C0func.F $(LTINC) perm.h
	$(XFC) -c -o C0func.o C0func.F
C0funcC.o: C0funcC.F $(LTINC)
	$(XFC) -c -o C0funcC.o C0funcC.F
Cget.o: Cget.F $(LTINC)
	$(XFC) -c -o Cget.o Cget.F
CgetC.o: Cget.F $(LTINC)
	$(CFC) -c -o CgetC.o Cget.F
ffxc0.o: ffxc0.F $(FFINC)
	$(XFC) -c -o ffxc0.o ffxc0.F
ffcc0.o: ffcc0.F $(FFINC)
	$(XFC) -c -o ffcc0.o ffcc0.F
ffxc0i.o: ffxc0i.F $(FFINC)
	$(XFC) -c -o ffxc0i.o ffxc0i.F
ffxc0p.o: ffxc0p.F $(FFINC)
	$(XFC) -c -o ffxc0p.o ffxc0p.F
ffxc0p0.o: ffxc0p0.F $(FFINC)
	$(XFC) -c -o ffxc0p0.o ffxc0p0.F
ffcc0p.o: ffcc0p.F $(FFINC)
	$(XFC) -c -o ffcc0p.o ffcc0p.F
ffdxc0.o: ffdxc0.F $(FFINC)
	$(XFC) -c -o ffdxc0.o ffdxc0.F
ffdel3.o: ffdel3.F $(FFINC)
	$(XFC) -c -o ffdel3.o ffdel3.F
ffcel3.o: ffcel3.F $(FFINC)
	$(XFC) -c -o ffcel3.o ffcel3.F

D0func.o: D0func.F $(LTINC) perm.h
	$(XFC) -c -o D0func.o D0func.F
D0funcC.o: D0funcC.F $(LTINC)
	$(XFC) -c -o D0funcC.o D0funcC.F
D0z.o: D0z.F $(LTINC)
	$(XFC) -c -o D0z.o D0z.F
Dget.o: Dget.F $(LTINC)
	$(XFC) -c -o Dget.o Dget.F
DgetC.o: Dget.F $(LTINC)
	$(CFC) -c -o DgetC.o Dget.F
ffxd0.o: ffxd0.F $(FFINC)
	$(XFC) -c -o ffxd0.o ffxd0.F
ffxd0h.o: ffxd0h.F $(FFINC)
	$(XFC) -c -o ffxd0h.o ffxd0h.F
ffxd0i.o: ffxd0i.F $(FFINC)
	$(XFC) -c -o ffxd0i.o ffxd0i.F
ffxd0p.o: ffxd0p.F $(FFINC)
	$(XFC) -c -o ffxd0p.o ffxd0p.F
ffxd0m0.o: ffxd0m0.F $(FFINC)
	$(XFC) -c -o ffxd0m0.o ffxd0m0.F
ffxd0tra.o: ffxd0tra.F $(FFINC)
	$(XFC) -c -o ffxd0tra.o ffxd0tra.F
ffxdbd.o: ffxdbd.F $(FFINC)
	$(XFC) -c -o ffxdbd.o ffxdbd.F
ffdcc0.o: ffdcc0.F $(FFINC)
	$(XFC) -c -o ffdcc0.o ffdcc0.F
ffdel4.o: ffdel4.F $(FFINC)
	$(XFC) -c -o ffdel4.o ffdel4.F
ffd0c.o: ffd0c.F $(FFINC) perm.h
	$(XFC) -c -o ffd0c.o ffd0c.F
ffTn.o: ffTn.F $(FFINC)
	$(XFC) -c -o ffTn.o ffTn.F
ffT13.o: ffT13.F $(FFINC)
	$(XFC) -c -o ffT13.o ffT13.F
ffS2.o: ffS2.F $(FFINC)
	$(XFC) -c -o ffS2.o ffS2.F
ffS3n.o: ffS3n.F $(FFINC)
	$(XFC) -c -o ffS3n.o ffS3n.F
ffRn.o: ffRn.F $(FFINC)
	$(XFC) -c -o ffRn.o ffRn.F

E0func.o: E0func.F $(LTINC)
	$(XFC) -c -o E0func.o E0func.F
E0funcC.o: E0func.F $(LTINC)
	$(CFC) -c -o E0funcC.o E0func.F
Eget.o: Eget.F $(LTINC)
	$(XFC) -c -o Eget.o Eget.F
EgetC.o: Eget.F $(LTINC)
	$(CFC) -c -o EgetC.o Eget.F
Ecoeffa.o: Ecoeffa.F $(LTINC)
	$(XFC) -c -o Ecoeffa.o Ecoeffa.F
EcoeffaC.o: Ecoeffa.F $(LTINC)
	$(CFC) -c -o EcoeffaC.o Ecoeffa.F
Ecoeffb.o: Ecoeffb.F $(LTINC)
	$(XFC) -c -o Ecoeffb.o Ecoeffb.F
EcoeffbC.o: Ecoeffb.F $(LTINC)
	$(CFC) -c -o EcoeffbC.o Ecoeffb.F
ffxe0.o: ffxe0.F $(FFINC)
	$(XFC) -c -o ffxe0.o ffxe0.F
ffdel5.o: ffdel5.F $(FFINC)
	$(XFC) -c -o ffdel5.o ffdel5.F

ini.o: ini.F $(LTINC)
	$(XFC) -c -o ini.o ini.F
auxCD.o: auxCD.F $(LTINC)
	$(XFC) -c -o auxCD.o auxCD.F
solve.o: solve.F $(LTINC)
	$(XFC) -c -o solve.o solve.F
solveC.o: solve.F $(LTINC)
	$(CFC) -c -o solveC.o solve.F
Dump.o: Dump.F $(LTINC)
	$(XFC) -c -o Dump.o Dump.F
DumpC.o: Dump.F $(LTINC)
	$(CFC) -c -o DumpC.o Dump.F
Li2.o: Li2.F defs.h
	$(XFC) -c -o Li2.o Li2.F
Li2C.o: Li2.F defs.h
	$(CFC) -c -o Li2C.o Li2.F
Li2omx.o: Li2omx.F defs.h
	$(XFC) -c -o Li2omx.o Li2omx.F
Li2omxC.o: Li2omx.F defs.h
	$(CFC) -c -o Li2omxC.o Li2omx.F
cache.o: cache.c $(LTINC)
	$(CC) $(CFLAGS) -c -o cache.o cache.c
ffinit.o: ffinit.F $(LTINC) fferr.h ffwarn.h
	$(XFC) -c -o ffinit.o ffinit.F
ffxli2.o: ffxli2.F $(FFINC)
	$(XFC) -c -o ffxli2.o ffxli2.F
ffcli2.o: ffcli2.F $(FFINC)
	$(XFC) -c -o ffcli2.o ffcli2.F
ffxxyz.o: ffxxyz.F $(FFINC)
	$(XFC) -c -o ffxxyz.o ffxxyz.F
ffcxyz.o: ffcxyz.F $(FFINC)
	$(XFC) -c -o ffcxyz.o ffcxyz.F
ffcrr.o: ffcrr.F $(FFINC)
	$(XFC) -c -o ffcrr.o ffcrr.F
ffcxr.o: ffcxr.F $(FFINC)
	$(XFC) -c -o ffcxr.o ffcxr.F
fftran.o: fftran.F $(FFINC)
	$(XFC) -c -o fftran.o fftran.F
ffabcd.o: ffabcd.F $(FFINC)
	$(XFC) -c -o ffabcd.o ffabcd.F
ff2dl2.o: ff2dl2.F $(FFINC)
	$(XFC) -c -o ff2dl2.o ff2dl2.F
ffcxs3.o: ffcxs3.F $(FFINC)
	$(XFC) -c -o ffcxs3.o ffcxs3.F
ffcxs4.o: ffcxs4.F $(FFINC)
	$(XFC) -c -o ffcxs4.o ffcxs4.F
ffdcxs.o: ffdcxs.F $(FFINC)
	$(XFC) -c -o ffdcxs.o ffdcxs.F
ffbndc.o: ffbndc.F $(FFINC)
	$(XFC) -c -o ffbndc.o ffbndc.F

f77290: f77290.c
	$(CC) $(CFLAGS) -o f77290 f77290.c
looptools.h90: looptools.h f77290
	f77290 looptools.h looptools.h90

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $(LIB) $?
	-$(RANLIB) $(LIB)


$(FE): lt.F $(LTINC) $(LIB)
	$(XFC) -o $(FE) lt.F $(LIB) $(LDFLAGS)
	-rm -f lt.o

clooptools.h: clooptools.h.in ftypes.h
	sed "s:NOUNDERSCORE:$(NOUNDERSCORE):" ftypes.h clooptools.h.in > clooptools.h

fcc: fcc.in
	sed -e 's|^fldflags=.*|fldflags="$(LDFLAGS) $(MCLIBS)"|' \
	    -e 's|^cc=.*|cc="$${REALCC:-$(CC)}"|' \
	    -e 's|^cxx=.*|cxx="$${REALCXX:-$(CXX)}"|' fcc.in > fcc
	chmod 755 fcc
	rm -f f++
	ln -s fcc f++

LoopTools$(EXE): LoopTools.tm clooptools.h fortranflush.o $(LIB) fcc
	test -d =. || ln -s . =.
	NM="$(NM)" DLLTOOL="$(DLLTOOL)" \
	CC="./fcc" REALCC="$(CC) $(CFLAGS)" \
	CXX="./f++" REALCXX="$(CXX) $(CXXFLAGS)" \
	PATH="$$PATH:." \
	  "$(MCC)" LoopTools.tm -o LoopTools$(EXE) $(MCFLAGS) \
	    fortranflush.o $(LIB)
	rm -f LoopTools.tm.c

LoopTools-quad$(EXE): LoopTools.tm clooptools.h fortranflush.o $(LIB) fcc
	sed '/:Begin:/,/:End:/ s/Real/Real128/g' LoopTools.tm > LoopTools-quad.tm
	NM="$(NM)" DLLTOOL="$(DLLTOOL)" \
	CC="./fcc" REALCC="$(CC) $(CFLAGS)" \
	CXX="./f++" REALCXX="$(CXX) $(CXXFLAGS)" \
	PATH="$$PATH:." \
	  "$(MCC)" LoopTools-quad.tm -o LoopTools-quad$(EXE) $(MCFLAGS) \
	    fortranflush.o $(LIB) -lpthread
	rm -f LoopTools.tm.c

fortranflush.o: fortranflush.F
	$(XFC) -c -o fortranflush.o fortranflush.F

