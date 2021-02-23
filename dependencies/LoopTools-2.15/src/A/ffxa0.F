* ffxa0.F
* the one-point function for real mass
* original code by G.J. van Oldenborgh
* this file is part of LoopTools
* last modified 7 Dec 10 th

#include "externals.h"
#include "types.h"

* Input:	xm	(real) mass2,
* Output:	ca0	(complex) A0, the one-point function,
*		ier	0 (ok)

	subroutine ffxa0(ca0, xm, ier)
	implicit none
	ComplexType ca0
	RealType xm
	integer ier

#include "ff.h"

	RealType xmu, xlogm

	xmu = xm
	if( mudim .ne. 0 ) xmu = xmu/mudim
	if( xmu .gt. xalogm ) then
	  xlogm = log(xmu)
	else
	  xlogm = 0
	  if( xmu .ne. 0 ) call fferr(2, ier)
	endif
	ca0 = -(xm*(xlogm - 1 - delta))
	end
