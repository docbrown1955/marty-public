* Dump.F
* dumps the parameters and coefficients on screen
* this file is part of LoopTools
* last modified 7 Apr 18 th

#include "externals.h"
#include "types.h"

#include "defs.h"


	subroutine XDumpPara(npoint, para, origin)
	implicit none
	integer npoint
	ArgType para(1,*)
	character*(*) origin

#include "lt.h"
#include "ltnames.h"

	integer i

	integer npara(5)
	data npara /Paa, Pbb, Pcc, Pdd, Pee/

#ifdef COMPLEXPARA
	if( len(origin) .gt. 1 ) print *, origin, "C", serial
#else
	if( len(origin) .gt. 1 ) print *, origin, serial
#endif

	do i = npoint + 1, npara(npoint)
	  print *, "  ", paraname(i,npoint), "=", para(1,i)
	enddo
	do i = 1, npoint
	  print *, "  ", paraname(i,npoint), "=", para(1,i)
	enddo

	call flush(6)
	end

************************************************************************

	subroutine XDumpCoeff(npoint, coeff)
	implicit none
	integer npoint
	ComplexType coeff(*)

#include "lt.h"
#include "ltnames.h"

	integer i

	integer ncoeff(5)
	data ncoeff /Naa, Nbb, Ncc, Ndd, Nee/

	do i = 1, ncoeff(npoint)
	  if( coeff(i) .ne. 0 )
     &      print *, coeffname(i,npoint), "=", coeff(i)
	enddo

	print *, "===================================================="

	call flush(6)
	end

