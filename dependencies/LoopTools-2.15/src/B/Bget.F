* Bget.F
* retrieve the two-point tensor coefficients
* this file is part of LoopTools
* last modified 7 Apr 18 th

#include "externals.h"
#include "types.h"

#define npoint 2
#include "defs.h"


	subroutine XBpara(para, p, m1, m2)
	implicit none
	ArgType para(1,*)
	ArgType p, m1, m2

#include "lt.h"

	P(1) = p

	M(1) = m1
	if( abs(M(1)) .lt. minmass ) M(1) = 0
	M(2) = m2
	if( abs(M(2)) .lt. minmass ) M(2) = 0
	end

************************************************************************

	memindex function XBget(p, m1, m2)
	implicit none
	ArgType p, m1, m2

#include "lt.h"

	memindex cacheindex
	external cacheindex, XBcoeff

#ifdef COMPLEXPARA
	memindex Bget
	external Bget
#endif

	ArgType para(1,Pbb)

#ifdef COMPLEXPARA
	if( abs(Im(p)) .gt. 0 )
     &    print *, "Complex momenta not implemented"
	if( abs(Im(m1)) + abs(Im(m2)) .eq. 0 ) then
	  XBget = Bget(p, m1, m2) - offsetC
	  return
	endif
#endif

	call XBpara(para, p, m1, m2)
	XBget = cacheindex(para, Bval(1,0), XBcoeff, RC*Pbb, Nbb, Bno)
	end

************************************************************************

	subroutine XBput(res, p, m1, m2)
	implicit none
	ComplexType res(*)
	ArgType p, m1, m2

#include "lt.h"

	external XBcoeff

	ArgType para(1,Pbb)

#ifdef COMPLEXPARA
	if( abs(Im(p)) .gt. 0 )
     &    print *, "Complex momenta not implemented"
	if( abs(Im(m1)) + abs(Im(m2)) .eq. 0 ) then
	  call Bput(res, p, m1, m2)
	  return
	endif
#endif

	call XBpara(para, p, m1, m2)
	call cachecopy(res, para, Bval(1,0), XBcoeff, RC*Pbb, Nbb, Bno)
	end

************************************************************************

	subroutine XBputnocache(res, p, m1, m2)
	implicit none
	ComplexType res(*)
	ArgType p, m1, m2

#include "lt.h"

	external XBcoeff

	ArgType para(1,Pbb)

#ifdef COMPLEXPARA
	if( abs(Im(p)) .gt. 0 )
     &    print *, "Complex momenta not implemented"
	if( abs(Im(m1)) + abs(Im(m2)) .eq. 0 ) then
	  call Bputnocache(res, p, m1, m2)
	  return
	endif
#endif

	call XBpara(para, p, m1, m2)
	call XBcoeff(res, para)
	end

************************************************************************

	ComplexType function XB0i(i, p, m1, m2)
	implicit none
	integer i
	ArgType p, m1, m2

#include "lt.h"

	memindex XBget
	external XBget

	memindex b

	b = XBget(p, m1, m2)
	XB0i = Bval(i+epsi,b)
	end

************************************************************************

	ComplexType function XB0(p, m1, m2)
	implicit none
	ArgType p, m1, m2

#include "lt.h"

	ComplexType XB0i
	external XB0i

	XB0 = XB0i(bb0, p, m1, m2)
	end

************************************************************************

	ComplexType function XB1(p, m1, m2)
	implicit none
	ArgType p, m1, m2

#include "lt.h"

	ComplexType XB0i
	external XB0i

	XB1 = XB0i(bb1, p, m1, m2)
	end

************************************************************************

	ComplexType function XB00(p, m1, m2)
	implicit none
	ArgType p, m1, m2

#include "lt.h"

	ComplexType XB0i
	external XB0i

	XB00 = XB0i(bb00, p, m1, m2)
	end

************************************************************************

	ComplexType function XB11(p, m1, m2)
	implicit none
	ArgType p, m1, m2

#include "lt.h"

	ComplexType XB0i
	external XB0i

	XB11 = XB0i(bb11, p, m1, m2)
	end

************************************************************************

	ComplexType function XB001(p, m1, m2)
	implicit none
	ArgType p, m1, m2

#include "lt.h"

	ComplexType XB0i
	external XB0i

	XB001 = XB0i(bb001, p, m1, m2)
	end

************************************************************************

	ComplexType function XB111(p, m1, m2)
	implicit none
	ArgType p, m1, m2

#include "lt.h"

	ComplexType XB0i
	external XB0i

	XB111 = XB0i(bb111, p, m1, m2)
	end

************************************************************************

	ComplexType function XDB0(p, m1, m2)
	implicit none
	ArgType p, m1, m2

#include "lt.h"

	ComplexType XB0i
	external XB0i

	XDB0 = XB0i(dbb0, p, m1, m2)
	end

************************************************************************

	ComplexType function XDB1(p, m1, m2)
	implicit none
	ArgType p, m1, m2

#include "lt.h"

	ComplexType XB0i
	external XB0i

	XDB1 = XB0i(dbb1, p, m1, m2)
	end

************************************************************************

	ComplexType function XDB00(p, m1, m2)
	implicit none
	ArgType p, m1, m2

#include "lt.h"

	ComplexType XB0i
	external XB0i

	XDB00 = XB0i(dbb00, p, m1, m2)
	end

************************************************************************

	ComplexType function XDB11(p, m1, m2)
	implicit none
	ArgType p, m1, m2

#include "lt.h"

	ComplexType XB0i
	external XB0i

	XDB11 = XB0i(dbb11, p, m1, m2)
	end

************************************************************************

	ComplexType function XDB001(p, m1, m2)
	implicit none
	ArgType p, m1, m2

#include "lt.h"

	ComplexType XB0i
	external XB0i

	XDB001 = XB0i(dbb001, p, m1, m2)
	end

