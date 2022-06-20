* C0funcC.F
* the scalar three-point function for complex parameters
* this file is part of LoopTools
* last modified 20 Feb 14 th

#include "externals.h"
#include "types.h"

#define npoint 3
#include "defs.h"


	subroutine C0funcC(res, para)
	implicit none
	ComplexType res, para(1,*)

	integer ier

#include "lt.h"

	ier = 0
	call ffcc0(res, para, ier)

	if( ier .gt. warndigits ) then
	  ier = 0
	  call ffcc0r(res, para, ier)
	  if( ier .gt. warndigits ) then
	    print *, "C0C claims ", ier, " lost digits"
	    print *, "  p1   =", P(1)
	    print *, "  p2   =", P(2)
	    print *, "  p1p2 =", P(3)
	    print *, "  m1   =", M(1)
	    print *, "  m2   =", M(2)
	    print *, "  m3   =", M(3)
	  endif
	endif
	end

