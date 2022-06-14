INSTALLDIR=/home/laptopgva/Desktop/MARTY/marty-public/667527

CC  = gcc-7
CXX = g++-7
FC  = gfortran-7


LOOPTOOLSVERSION=2.15
LOOPTOOLSDIR=LoopTools-$(LOOPTOOLSVERSION)
LTINSTALLDIR=$(LOOPTOOLSDIR)/include

dep:
	cd $(LOOPTOOLSDIR)&& ./configure --quad --prefix=.&&\
		sed -i 's/FFLAGS = /FFLAGS = -fPIC /g' makefile;\
		sed -i 's/CFLAGS = /CFLAGS = -fPIC /g' makefile;\
		sed -i 's/CXXFLAGS = /CXXFLAGS = -fPIC /g' makefile;\
		make&&make install
	cd $(LOOPTOOLSDIR)/build-quad && $(FC) -shared -o libooptools-quad.so *.o
	cd $(LOOPTOOLSDIR)&& ./configure --prefix=.&&\
		sed -i 's/FFLAGS = /FFLAGS = -fPIC /g' makefile;\
		sed -i 's/CFLAGS = /CFLAGS = -fPIC /g' makefile;\
		sed -i 's/CXXFLAGS = /CXXFLAGS = -fPIC /g' makefile;\
		make&&make install
	cd $(LOOPTOOLSDIR)/build && $(FC) -shared -o libooptools.so *.o
install:
	cp $(LTINSTALLDIR)/clooptools.h  $(INSTALLDIR)/include
	cp $(LOOPTOOLSDIR)/build/libooptools*  $(INSTALLDIR)/lib
	cp $(LOOPTOOLSDIR)/build-quad/libooptools*  $(INSTALLDIR)/lib

clean:
	-rm -rf $(LOOPTOOLSDIR)/build*
