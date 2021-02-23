* BcoeffC.F
* invoke the two-point tensor coefficients
* this file is part of LoopTools
* last modified 26 Jan 14 th

#include "externals.h"
#include "types.h"

#define npoint 2
#include "defs.h"


	subroutine BcoeffC(B, para)
	implicit none
	ComplexType B(*), para(1,*)

#include "lt.h"
#include "ltnames.h"

	integer ier(Nbb), i
	logical ini

	do i = 1, Nbb
	  B(i) = 0
	enddo

	if( lambda .le. 0 ) then
	  B(1+bb0) = 1
	  B(1+bb1) = -.5D0
	  B(1+bb00) = -(P(1) - 3*(M(1) + M(2)))/12D0
	  B(1+bb11) = 1/3D0
	  B(1+bb001) = (P(1) - 2*M(1) - 4*M(2))/24D0
	  B(1+bb111) = -.25D0
	  B(1+dbb00) = -1/12D0
	  B(1+dbb001) = 1/24D0
	endif

	call BcoeffFFC(B, para, ier)

	ini = .TRUE.
	do i = 1, Nbb
	  if( ier(i) .gt. warndigits ) then
	    if( ini ) then
	      print *, "Loss of digits in BgetC for:"
	      call DumpParaC(2, para, " ")
	      ini = .FALSE.
	    endif
	    print *, coeffname(i,2), " claims ", ier(i), "lost digits"
	  endif
	enddo
	end

