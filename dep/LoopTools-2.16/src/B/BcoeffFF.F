* BcoeffFF.F
* the two-point tensor coefficients from FF
* this file is part of LoopTools
* last modified 8 Apr 18 th

#include "externals.h"
#include "types.h"

#define npoint 2
#include "defs.h"


	subroutine XBcoeffFF(B, para, ier)
	implicit none
	ComplexType B(*)
	ArgType para(1,*)
	integer ier(*)

#include "lt.h"

	ArgType p, m1, m2, dm
	ComplexType a0(2), b2(2), pdb0
	integer i
#ifdef COMPLEXPARA
	ArgType m1dm
	ComplexType dmp, d2mp
#endif

	m1 = M(1)
	m2 = M(2)
	p = P(1)

	do i = 1, Nbb
	  ier(i) = 0
	enddo

	ldot = .TRUE.
	i = 0
	call Xffb0(B(bb0), p, m1, m2, i)
	ier(bb0) = i
	call Xffa0(a0(1), m1, i)
	call Xffa0(a0(2), m2, i)
	ier(bb1) = i
	call Xffb1(B(bb1), B(bb0), a0, p, m1, m2, Xfpij2, ier(bb1))
	call Xffb2p(b2, B(bb1), B(bb0), a0, p, m1, m2, Xfpij2, i)
	ier(bb00) = i
	ier(bb11) = i
	B(bb11) = b2(1)
	B(bb00) = b2(2)
	ldot = .FALSE.

	dm = m1 - m2

	if( abs(p) .lt. diffeps ) then
	  if( abs(dm) .lt. diffeps ) then
	    B(bb001) = -.5D0*B(bb00)
	  else
	    B(bb001) = -( ((m1 + m2)/6D0)**2 +
     &        m1*m2/6D0 * (B(bb0) + 1/3D0) + 
     &        (dm - m2)/3D0 * B(bb00) )/dm
	  endif
	else
	  B(bb001) = .125D0*( 2*m1*B(bb1) - a0(2) +
     &      (p + dm)*(B(bb11) + 1/6D0) - .5D0*(m1 + m2) )
	endif

	call Xffdb0(B(dbb0), pdb0, p, m1, m2, ier(dbb0))

#ifdef COMPLEXPARA
	if( abs(p) .lt. diffeps ) then
	  if( abs(dm) .lt. diffeps ) then
	    B(bb111) = -.25D0*B(bb0)
	    B(dbb1) = -1/12D0/m1
	  else
	    m1dm = m1/dm
	    B(bb111)= 3/16D0 + .25D0*a0(2)/dm*(m1dm + 1) +
     &        .5D0*m1dm*(m1dm*(B(bb1) - .5D0) - 1/6D0)
	    B(dbb1) = (2*m2*B(bb1) + a0(2) - (m1 + 2*m2)/3D0)/dm**2
	  endif
	else
	  B(bb111) = -.25D0*( a0(2) + 2*m1*B(bb1) +
     &      (p + dm)*(3*B(bb11) + 1/6D0) - .5D0*(m1 + m2) )/p
	  B(dbb1) = .5D0/p*(
     &      (a0(2) - a0(1) + dm*B(bb0))/p -
     &      (p + dm)*B(dbb0) )
	endif

	dmp = (m1 - m2)/p
	d2mp = (m1 - 2*m2)/p
	B(dbb11) = 1/3D0*(
     &    ( (.5D0*(m1 + m2) +
     &        (2*dmp + 1)*a0(1) - (2*dmp + 2)*a0(2))/p -
     &      (d2mp + 2*dmp**2)*B(bb0) )/p +
     &    (d2mp + dmp**2 + 1)*B(dbb0) )
#else
	call ffxb111(B(bb111), p, m1, m2, ier(bb111))
	call ffxdb1(B(dbb1), p, m1, m2, ier(dbb1))
	call ffxdb11(B(dbb11), p, m1, m2, ier(dbb11))
#endif

	B(dbb00) = 1/6D0*( 2*m1*B(dbb0) + B(bb1) +
     &    (p + dm)*B(dbb1) - 1/3D0 )

	B(dbb001) = 1/8D0*( 2*m1*B(dbb1) + B(bb11) +
     &    (p + dm)*B(dbb11) + 1/6D0 )
	end

