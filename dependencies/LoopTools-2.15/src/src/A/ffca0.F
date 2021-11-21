* ffca0.F
* the one-point function for complex mass
* original code by G.J. van Oldenborgh
* this file is part of LoopTools
* last modified 7 Dec 10 th

#include "externals.h"
#include "types.h"

* Input:	cm	(complex) mass2, re > 0, im < 0.
* Output:	ca0	(complex) A0, the one-point function,
*		ier	0 (OK)

	subroutine ffca0(ca0, cm, ier)
	implicit none
	ComplexType ca0, cm
	integer ier

#include "ff.h"

	ComplexType cmu, clogm

	RealType absc
	ComplexType c
	absc(c) = abs(Re(c)) + abs(Im(c))

* the real case:
* adapted to log-and-pole scheme 25-mar-1992
	if( Im(cm) .eq. 0 .or. nschem .lt. 7 ) then
	  call ffxa0(ca0, cm, ier)
	  return
	endif

	cmu = cm
	if( mudim .ne. 0 ) cmu = cmu/mudim
	if( absc(cmu) .gt. xclogm ) then
	  clogm = log(cmu)
	else
	  clogm = 0
	  if ( cmu .ne. 0 ) call fferr(1, ier)
	endif
	ca0 = -cm*(clogm - 1 - delta)
	end
