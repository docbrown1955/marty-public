* Aget.F
* retrieve the one-point tensor coefficients
* this file is part of LoopTools
* last modified 7 Apr 18 th

#include "externals.h"
#include "types.h"

#define npoint 1
#include "defs.h"


	memindex function XAget(m)
	implicit none
	ArgType m

#include "lt.h"

	memindex cacheindex
	external cacheindex, XAcoeff

	ArgType para(1,Paa)

	M(1) = m
	if( abs(M(1)) .lt. minmass ) M(1) = 0

	XAget = cacheindex(para, Aval(1,0), XAcoeff, RC*Paa, Naa, Ano)
	end

************************************************************************

	subroutine XAput(res, m)
	implicit none
	ComplexType res(*)
	ArgType m

#include "lt.h"

	external XAcoeff

	ArgType para(1,Paa)

	M(1) = m
	if( abs(M(1)) .lt. minmass ) M(1) = 0

	call cachecopy(res, para, Aval(1,0), XAcoeff, RC*Paa, Naa, Ano)
	end

************************************************************************

	subroutine XAputnocache(res, m)
	implicit none
	ComplexType res(*)
	ArgType m

#include "lt.h"

	ArgType para(1,Paa)

	M(1) = m
	if( abs(M(1)) .lt. minmass ) M(1) = 0

	call Acoeff(res, para)
	end

************************************************************************

	ComplexType function XA0i(i, m)
	implicit none
	integer i
	ArgType m

#include "lt.h"

	memindex XAget
	external XAget

	memindex b

	b = XAget(m)
	XA0i = Aval(i+epsi,b)
	end

************************************************************************

	ComplexType function XA0(m)
	implicit none
	ArgType m

#include "lt.h"

	ComplexType XA0i
	external XA0i

	XA0 = XA0i(aa0, m)
	end

************************************************************************

	ComplexType function XA00(m)
	implicit none
	ArgType m

#include "lt.h"

	ComplexType XA0i
	external XA0i

	XA00 = XA0i(aa00, m)
	end

************************************************************************

	subroutine XAcoeff(A, para)
	implicit none
	ComplexType A(*)
	ArgType para(1,*)

#include "lt.h"

	ArgType m
	ComplexType res(0:1)
	integer key, ier
	logical dump

	m = M(1)
	key = ibits(versionkey, KeyA0, 2)

	serial = serial + 1
	dump = ibits(debugkey, DebugA, 1) .ne. 0 .and.
     &    serial .ge. debugfrom .and. serial .le. debugto
	if( dump ) call XDumpPara(1, para, "Acoeff")

	if( key .ne. 1 ) then
	  ier = 0
	  call Xffa0(res(0), m, ier)
	  if( ier .gt. warndigits ) key = ior(key, 2)
	endif

	if( key .ne. 0 ) then
	  res(1) = 0
	  if( m .ne. 0 ) res(1) = m*(1 - log(m/mudim) + delta)
	  if( key .gt. 1 .and.
     &        abs(res(0) - res(1)) .gt. maxdev*abs(res(0)) ) then
#ifdef COMPLEXPARA
	    print *, "Discrepancy in CA0:"
	    print *, "    m =", m
	    print *, "A0C a =", res(0)
	    print *, "A0C b =", res(1)
#else
	    print *, "Discrepancy in A0:"
	    print *, "   m =", m
	    print *, "A0 a =", res(0)
	    print *, "A0 b =", res(1)
#endif
	  endif
	endif

	A(aa0) = res(iand(key, 1))
	A(aa00) = .25D0*m*(A(aa0) + .5D0*m)
	A(1+aa0) = 0
	A(1+aa00) = 0
	A(2+aa0) = 0
	A(2+aa00) = 0

	if( lambda .le. 0 ) then
	  A(1+aa0) = m*uvdiv
	  A(1+aa00) = .25D0*m**2*uvdiv
	endif

	if( dump ) call XDumpCoeff(1, A)
	end

