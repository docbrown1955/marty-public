* Bcoeff.F
* invoke the two-point tensor coefficients
* this file is part of LoopTools
* last modified 3 May 21 th

#include "externals.h"
#include "types.h"

#define npoint 2
#include "defs.h"


	subroutine Bcoeff(B, para)
	implicit none
	ComplexType B(*)
	RealType para(1,*)

#include "lt.h"

	ComplexType cmp(Nbb)
	RealType p, m1, m2, div
	integer ier(Nbb), ierall, i
	logical dump

	serial = serial + 1
	dump = ibits(debugkey, DebugB, 1) .ne. 0 .and.
     &    serial .ge. debugfrom .and. serial .le. debugto
	if( dump ) call DumpPara(2, para, "Bcoeff")

	B(1:Nbb) = 0

	m1 = M(1)
	m2 = M(2)
	p = P(1)

	div = uvdiv
	if( abs(p) + abs(m1) + abs(m2) .lt. zeroeps ) div = div - 1
	if( div .ne. 0 ) then
	  B(1+bb0) = div
	  B(1+bb1) = -.5D0*div
	  B(1+bb00) = -(p - 3*(m1 + m2))/12D0*div
	  B(1+dbb00) = -1/12D0*div
	  B(1+bb11) = 1/3D0*div
	  B(1+bb001) = (p - 2*m1 - 4*m2)/24D0*div
	  B(1+dbb001) = 1/24D0*div
	  B(1+bb111) = -.25D0*div
	endif

	if( lambda .le. 0 ) then
	  if( m1*m2 .eq. 0 .and. abs(p - m1 - m2) .lt. diffeps ) then
	    B(1+dbb0) = nan
	    if( p .ne. 0 ) B(1+dbb0) = -.5D0/p
	  endif
	  if( m2 .eq. 0 .and. abs(p - m1) .lt. diffeps ) then
	    B(1+dbb1) = nan
	    if( p .ne. 0 ) B(1+dbb1) = .5D0/p
	  endif
	endif

	if( abs(p) + abs(m1) + abs(m2) .lt. zeroeps ) then
	  if( delta .ne. 0 ) then
	    B(bb0) = delta
	    B(bb1) = -.5D0*delta
	    B(bb11) = 1/3D0*delta
	    B(bb111) = -.25*delta
	    B(dbb00) = -1/12D0*delta
	    B(dbb001) = 1/24D0*delta
	    B(dbb0) = nan
	    B(dbb1) = nan
	  endif
	  goto 9
	endif

	goto (1, 2, 3) ibits(versionkey, KeyBget, 2)

	call BcoeffAD(B, para)
	goto 9

1	call BcoeffFF(B, para, ier)
	ierall = 0
	do i = 1, Nbb, 3
	  ierall = max(ierall, ier(i))
	enddo
	if( ierall .gt. warndigits ) then
	  call BcoeffAD(cmp, para)
	  call Bcheck(cmp, B, ier, para)
	endif
	goto 9

2	call BcoeffAD(B, para)
	call BcoeffFF(cmp, para, ier)
	call Bcheck(B, cmp, ier, para)
	goto 9

3	call BcoeffFF(B, para, ier)
	call BcoeffAD(cmp, para)
	call Bcheck(cmp, B, ier, para)

9	if( dump ) call DumpCoeff(2, B)
	end

************************************************************************

	subroutine Bcheck(Ba, Bb, ier, para)
	implicit none
	ComplexType Ba(*), Bb(*)
	integer ier(*)
	ArgType para(1,*)

#include "lt.h"
#include "ltnames.h"

	integer i
	logical ini

	ini = .TRUE.
	do i = 1, Nbb, 3
	  if( abs(Ba(i) - Bb(i)) .gt. maxdev*abs(Ba(i)) ) then
	    if( ini ) then
	      call DumpPara(2, para, "Discrepancy in Bget")
	      ini = .FALSE.
	    endif
	    print *, coeffname(i,2), " a =", Ba(i)
	    print *, coeffname(i,2), " b =", Bb(i)
	    if( ier(i) .gt. errdigits ) Bb(i) = Ba(i)
	  endif
	enddo
	end

