* D0z.F
* D0, D00 for zero momenta, from arXiv:0811.2891
* this file is part of LoopTools
* last modified 16 Oct 18 th

#include "externals.h"
#include "types.h"

#define npoint 4
#include "defs.h"


	subroutine D0z(res, res00, para)
	implicit none
	ComplexType res, res00
	RealType para(1,*)

#include "lt.h"

	RealType m1, m2, m3, m4, mx, my, lm, l1, l3

	RealType safelog, d0, d00, x
	safelog(x) = merge(log(x), Re(nan), x .gt. zeroeps)
	d0(m1, m2, m3, m4) = m1*safelog(m1)/
     &    ((m2 - m1)*(m3 - m1)*(m4 - m1))
	d00(m1, m2, m3, m4) = m1/4*d0(m1, m2, m3, m4)

	if( abs(M(1) - M(2)) .lt. diffeps ) then
	  m1 = M(3)
	  m2 = M(4)
	  m3 = M(1)
	else if( abs(M(1) - M(3)) .lt. diffeps ) then
	  m1 = M(2)
	  m2 = M(4)
	  m3 = M(1)
	else if( abs(M(1) - M(4)) .lt. diffeps ) then
	  m1 = M(2)
	  m2 = M(3)
	  m3 = M(1)
	else if( abs(M(2) - M(3)) .lt. diffeps ) then
	  m1 = M(1)
	  m2 = M(4)
	  m3 = M(2)
	else if( abs(M(2) - M(4)) .lt. diffeps ) then
	  m1 = M(1)
	  m2 = M(3)
	  m3 = M(2)
	else if( abs(M(3) - M(4)) .lt. diffeps ) then
	  m1 = M(1)
	  m2 = M(2)
	  m3 = M(3)
	else
	  res =
     &      d0(M(1), M(2), M(3), M(4)) +
     &      d0(M(2), M(1), M(3), M(4)) +
     &      d0(M(3), M(1), M(2), M(4)) +
     &      d0(M(4), M(1), M(2), M(3))
	  res00 =
     &      d00(M(1), M(2), M(3), M(4)) +
     &      d00(M(2), M(1), M(3), M(4)) +
     &      d00(M(3), M(1), M(2), M(4)) +
     &      d00(M(4), M(1), M(2), M(3))
	  return
	endif

	if( abs(m1 - m2) .lt. diffeps ) then
	  if( abs(m1 - m3) .lt. diffeps ) then
* case D0z(m1, m1, m1, m1)
	    res = 1/6D0/m1**2
	    res00 = -1/12D0/m1
	    return
	  endif
* case D0z(m1, m1, m3, m3)
	  if( m3 .lt. zeroeps ) then
	    res = .5D0/m1**2
	    res00 = -.25D0/m1
	    return
	  endif
	  mx = 1/(m3 - m1)
	  lm = mx*log(m3/m1)
	  l1 = lm*m1 - 1
	  l3 = lm*m3 - 1
	  res = mx**2*(l1 + l3)
	  res00 = mx**2/4*(m1*l3 + m3*l1)
	  return
	endif

	if( abs(m3 - m1) .lt. diffeps ) then
* case D0z(m3, m1, m3, m3)
	  mx = m1
	  m1 = m2
	  m2 = mx
	endif

	mx = 1/(m3 - m1)

	if( abs(m3 - m2) .lt. diffeps ) then
* case D0z(m1, m3, m3, m3)
	  if( m1 .lt. zeroeps ) then
	    res = -.5D0/m3**2
	    res00 = -.125D0/m3
	    return
	  endif
	  lm = mx*log(m3/m1)
	  l1 = mx*(1 - m1*lm)
	  res = mx*(l1 + 1/m3)
	  res00 = .25D0*mx*(m1*l1 - .5D0)
	  return
	endif

* case D0z(m1, m2, m3, m3)
	my = 1/(m3 - m2)
	lm = log(m3)
	l1 = (m1*mx + m2*my)*lm - 1
	res = mx*my*(lm + l1) +
     &    d0(m1, m2, m3, m3) +
     &    d0(m2, m1, m3, m3)
	res00 = mx*my*l1*m3/4 +
     &    d00(m1, m2, m3, m3) +
     &    d00(m2, m1, m3, m3)
	end

