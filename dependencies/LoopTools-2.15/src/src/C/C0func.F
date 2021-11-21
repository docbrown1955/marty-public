* C0.F
* the scalar three-point function
* this file is part of LoopTools
* last modified 1 Sep 14 th

#include "externals.h"
#include "types.h"

#define npoint 3
#include "defs.h"


	subroutine C0func(res, para)
	implicit none
	ComplexType res(0:2)
	RealType para(1,*)

#include "lt.h"

	external C0soft, C0coll, C0softDR, C0collDR

	res(0) = 0
	res(1) = 0
	res(2) = 0

	if( lambda .le. 0 ) then
	  call CDispatch(res, para, C0softDR, C0collDR)
	else
	  call CDispatch(res, para, C0soft, C0coll)
	endif
	end

************************************************************************

	subroutine CDispatch(res, para, soft, coll)
	implicit none
	ComplexType res(0:2)
	RealType para(1,*)
	external soft, coll

#include "lt.h"
#include "perm.h"

	integer i, z, c, s, perm, ier, key
	ComplexType alt

	ComplexType C0p3, C0p2, C0p1, C0p0
	external C0p3, C0p2, C0p1, C0p0

	integer paraperm(3)
	data paraperm /p123, p231, p312/

#define Px(j) P(ibits(perm,3*(3-j),3))
#define Mx(j) M(ibits(perm,3*(3-j),3))

	c = 0
555	z = 0
	s = 0
	do i = 1, 3
	  perm = paraperm(i)
	  if( abs(Mx(1)) .lt. zeroeps ) then
	    if( abs(Px(1) - Mx(2)) +
     &          abs(Px(3) - Mx(3)) .lt. diffeps ) then
	      if( DEBUGLEVEL .gt. 0 )
     &          print '("soft C0, perm = ",O3)', perm
	      s = perm
	      goto 556
	    endif
	    if( abs(Px(1)) + abs(Mx(2)) .lt. zeroeps ) c = perm
	  endif
	  if( abs(P(i)) .lt. zeroeps ) z = z + 1
	enddo

	if( c .ne. 0 ) then
	  if( DEBUGLEVEL .gt. 0 )
     &      print '("collinear C0, perm = ",O3)', perm
	  call coll(res, para, c)
	  if( c .eq. 0 ) goto 555
	  return
	endif

556	if( lambda .lt. 0 .or. (s .ne. 0 .and. lambda .eq. 0) ) then
	  res(0) = 0
	  if( s .ne. 0 ) call soft(res, para, s)
	  return
	endif

	key = ibits(versionkey, KeyC0, 2)

	if( key .ne. 1 ) then
	  ier = 0
	  call ffxc0(res(0), para, ier)
	  if( ier .gt. warndigits ) then
	    ier = 0
	    call ffxc0r(res(0), para, ier)
	    if( ier .gt. warndigits ) key = ior(key, 2)
	    if( ier .ge. errdigits ) key = ior(key, 3)
	  endif
	  if( key .eq. 0 ) return
	  alt = res(0)
	endif

	if( s .ne. 0 ) then
	  call soft(res, para, s)
	  goto 9
	endif

	goto (1, 2, 3) z

	res(0) = C0p3(para, p123) + C0p3(para, p231) + C0p3(para, p312)
	goto 9

1	res(0) = C0p2(para, p123) + C0p2(para, p231) + C0p2(para, p312)
	goto 9

2	res(0) = C0p1(para, p123) + C0p1(para, p231) + C0p1(para, p312)
	goto 9

3	res(0) = C0p0(para)

9	if( key .ne. 0 ) then
	  if( key .gt. 1 .and.
     &        abs(res(0) - alt) .gt. maxdev*abs(alt) ) then
	    print *, "Discrepancy in C0:"
	    print *, "  p1   =", P(1)
	    print *, "  p2   =", P(2)
	    print *, "  p1p2 =", P(3)
	    print *, "  m1   =", M(1)
	    print *, "  m2   =", M(2)
	    print *, "  m3   =", M(3)
	    print *, "C0 a   =", alt
	    print *, "C0 b   =", res(0)
	    if( ier .gt. errdigits ) alt = res(0)
	  endif
	endif

	if( .not. btest(key, 0) ) res(0) = alt
	end

************************************************************************

	subroutine CDump(s, para, perm)
	implicit none
	character*(*) s
	RealType para(1,*)
	integer perm

#include "lt.h"

	print '(A,", perm = ",O3)', s, perm
	if( DEBUGLEVEL .gt. 1 ) then
	  print *, "p1 =", Px(1)
	  print *, "p2 =", Px(2)
	  print *, "p3 =", Px(3)
	  print *, "m1 =", Mx(1)
	  print *, "m2 =", Mx(2)
	  print *, "m3 =", Mx(3)
	endif
	end

************************************************************************
* the following routines are adapted from Ansgar Denner's bcanew.f
* to the conventions of LoopTools;
* they are used for double-checking the results of FF

* all mom-squares != 0

	ComplexType function C0p3(para, perm)
	implicit none
	RealType para(1,*)
	integer perm

#include "lt.h"

	RealType m1, m2, m3, p1, p2, p3, pp1, pp2, pp3
	RealType m12, m13, m23, a2, n, n1, n2, n3, n123, s
	ComplexType a, b, c
	ComplexType y1, y2, y3, y4, x1, x2, x3, x4, z3, z4
	integer z3z4, x1z3, x3z3, x2z4, x4z4

	ComplexType spence
	integer eta
	external spence, eta

	if( DEBUGLEVEL .gt. 0 ) call CDump("C0p3", para, perm)

	m1 = Mx(1)
	m2 = Mx(2)
	m3 = Mx(3)
	p1 = Px(1)
	p2 = Px(2)
	p3 = Px(3)

	m12 = m1 - m2
	m13 = m1 - m3
	m23 = m2 - m3

	a2 = (p1 - p2 - p3)**2 - 4*p2*p3
	a = sqrt(ToComplex(a2))
	n = .5D0/p1
	c = (p1*(p1 - p2 - p3 - m13 - m23) - m12*(p2 - p3))/a

	n123 = p1*(p2*p3 + m13*m23) + m12*(m13*p2 - m23*p3)
	pp1 = p1*(p1 - p2 - p3)
	pp2 = p2*(p1 - p2 + p3)
	pp3 = p3*(p1 + p2 - p3)
	n1 = n123 - m23*pp1 - m12*pp2
	n2 = n123 - m13*pp1 + m12*pp3
	n3 = n123 + m3*pp1 - m1*pp2 - m2*pp3

	y1 = n*(c + (p1 - m12))
	y4 = n*(c - (p1 - m12))
	if( abs(y1) .lt. abs(y4) ) y1 = n1/(a2*p1*y4)

	y2 = n*(c - (p1 + m12))
	y4 = n*(c + (p1 + m12))
	if( abs(y2) .lt. abs(y4) ) y2 = n2/(a2*p1*y4)

	b = sqrt(ToComplex((p1 - m12)**2 - 4*p1*m2))
	y3 = n*(c + b)
	y4 = n*(c - b)
	if( abs(y3) .lt. abs(y4) ) then
	  y3 = n3/(a2*p1*y4)
	else
	  y4 = n3/(a2*p1*y3)
	endif
	s = Re(a*b)
	y3 = y3 + sign(abs(y3), s)*cIeps
	y4 = y4 - sign(abs(y4), s)*cIeps

	C0p3 = spence(0, y2/y3, 0D0) + spence(0, y2/y4, 0D0) -
     &    spence(0, y1/y3, 0D0) - spence(0, y1/y4, 0D0)

	if( Im(a) .ne. 0 ) then
	  c = cIeps
	  if( abs(b) .ne. 0 ) c = abs(b)/b*c
	  x1 = c - n*( p1 - m12 + b)
	  x2 = c - n*( p1 - m12 - b)
	  x3 = c - n*(-p1 - m12 + b)
	  x4 = c - n*(-p1 - m12 - b)
	  z3 = 1/y3
	  z4 = 1/y4
	  z3z4 = eta(z3, 0D0, z4, 0D0, 0D0)
	  x1z3 = eta(x1, 0D0, z3, 0D0, 0D0)
	  x3z3 = eta(x3, 0D0, z3, 0D0, 0D0)
	  x2z4 = eta(x2, 0D0, z4, 0D0, 0D0)
	  x4z4 = eta(x4, 0D0, z4, 0D0, 0D0)
	  c = log(y1)*(eta(x1, 0D0, x2, 0D0, 0D0) +
     &                 z3z4 - x1z3 - x2z4) -
     &        log(y2)*(eta(x3, 0D0, x4, 0D0, 0D0) +
     &                 z3z4 - x3z3 - x4z4) +
     &        log(y3)*(x1z3 - x3z3) +
     &        log(y4)*(x2z4 - x4z4)
	  if( Im(a) .gt. 0 .and. p1 .lt. 0 ) c = c - log(y1/y2)
	  C0p3 = C0p3 + c2ipi*c
	endif

	C0p3 = C0p3/a

	if( DEBUGLEVEL .gt. 1 ) print *, "C0p3 =", C0p3
	end

************************************************************************
* one mom-square zero

	ComplexType function C0p2(para, perm)
	implicit none
	RealType para(1,*)
	integer perm

#include "lt.h"

	RealType m1, m2, m3, p1, p2, p3
	RealType m12, m23, m13, a, c, y1, y2
	ComplexType b, y3, y4

	ComplexType spence
	external spence

	if( DEBUGLEVEL .gt. 0 ) call CDump("C0p2", para, perm)

	if( abs(Px(1)) .lt. zeroeps ) then
	  C0p2 = 0
	  return
	endif

	m1 = Mx(1)
	m2 = Mx(2)
	m3 = Mx(3)
	p1 = Px(1)
	p2 = Px(2)
	p3 = Px(3)

	m12 = m1 - m2
	m23 = m2 - m3
	m13 = m1 - m3

	if( abs(p3) .lt. zeroeps ) then
	  a = p1 - p2
	  y1 = -2*p1*(m13 - a)
	  y2 = -2*p1*m13
	else
	  a = p3 - p1
	  y1 = -2*p1*m23
	  y2 = -2*p1*(m23 + a)
	endif

	c = p1*(p1 - p2 - p3 - m13 - m23) - m12*(p2 - p3)
	b = a*sqrt(ToComplex((p1 - m12)**2 - 4*p1*m2))
	y3 = c + b
	y4 = c - b
	c = 4*p1*(
     &    p1*((p1 - p2 - p3)*m3 + p2*p3 + m13*m23) +
     &    p2*((p2 - p3 - p1)*m1 + m12*m13) +
     &    p3*((p3 - p1 - p2)*m2 - m12*m23) )
	if( abs(y3) .lt. abs(y4) ) then
	  y3 = c/y4
	else
	  y4 = c/y3
	endif
	c = a/p1
	y3 = y3 + sign(abs(y3), c)*cIeps
	y4 = y4 - sign(abs(y4), c)*cIeps

	C0p2 = (spence(0, y2/y3, 0D0) + spence(0, y2/y4, 0D0) -
     &    spence(0, y1/y3, 0D0) - spence(0, y1/y4, 0D0))/a

	if( DEBUGLEVEL .gt. 1 ) print *, "C0p2 =", C0p2
	end

************************************************************************
* two mom-squares zero

	ComplexType function C0p1(para, perm)
	implicit none
	RealType para(1,*)
	integer perm

#include "lt.h"

	RealType m1, m2, m3, p1, p2, p3
	RealType m12, m23, m13, c, y1, y2
	ComplexType b, y3, y4

	ComplexType spence
	external spence

	if( DEBUGLEVEL .gt. 0 ) call CDump("C0p1", para, perm)

	if( abs(Px(1)) .lt. zeroeps ) then
	  C0p1 = 0
	  return
	endif

	m1 = Mx(1)
	m2 = Mx(2)
	m3 = Mx(3)
	p1 = Px(1)
	p2 = Px(2)
	p3 = Px(3)

	m12 = m1 - m2
	m23 = m2 - m3
	m13 = m1 - m3

	C0p1 = 0

	if( abs(m13) .gt. diffeps ) then
	  y1 = m23 - p1
	  y2 = m23
	  c = m23 + p1*m3/m13
	  y3 = c - sign(c, p1/m13)*cIeps
	  C0p1 = spence(0, y1/y3, 0D0) - spence(0, y2/y3, 0D0)
	endif

	y1 = -2*p1*m23
	y2 = -2*p1*(m23 - p1)

	c = p1*(p1 - m13 - m23)
	b = p1*sqrt(ToComplex((p1 - m12)**2 - 4*p1*m2))
	y3 = c - b
	y4 = c + b
	c = 4*p1**2*(p1*m3 + m13*m23)
	if( abs(y3) .lt. abs(y4) ) then
	  y3 = c/y4
	else
	  y4 = c/y3
	endif
	y3 = y3 - abs(y3)*cIeps
	y4 = y4 + abs(y4)*cIeps

	C0p1 = (C0p1 +
     &    spence(0, y1/y3, 0D0) + spence(0, y1/y4, 0D0) -
     &    spence(0, y2/y3, 0D0) - spence(0, y2/y4, 0D0))/p1

	if( DEBUGLEVEL .gt. 1 ) print *, "C0p1 =", C0p1
	end

************************************************************************

	ComplexType function C0p0(para)
	implicit none
	RealType para(1,*)

#include "lt.h"
#include "perm.h"

	RealType m1, m2, m3
	RealType m12, m23, m13

	if( DEBUGLEVEL .gt. 0 ) call CDump("C0p0", para, p123)

	m1 = M(1)
	m2 = M(2)
	m3 = M(3)

	m12 = m1 - m2
	m23 = m2 - m3
	m13 = m1 - m3

	if( abs(m23) .lt. diffeps ) then
	  if( abs(m13) .lt. diffeps ) then
	    C0p0 = -.5D0/m1
	  else
	    C0p0 = (m13 - m1*log(m1/m3))/m13**2
	  endif
	else
	  if( abs(m12) .lt. diffeps ) then
	    C0p0 = (-m23 + m3*log(m2/m3))/m23**2
	  else if( abs(m13) .lt. diffeps ) then
	    C0p0 = (m23 - m2*log(m2/m3))/m23**2
	  else
	    C0p0 = m3/(m13*m23)*log(m1/m3) - m2/(m12*m23)*log(m1/m2)
	  endif
	endif

	if( DEBUGLEVEL .gt. 1 ) print *, "C0p0 =", C0p0
	end

************************************************************************

	subroutine C0soft(res, para, perm)
	implicit none
	ComplexType res
	RealType para(1,*)
	integer perm

#include "lt.h"

	ComplexType spence
	external spence

	RealType s, m1, m2
	RealType a, h1, h2, h3, ps
	ComplexType ls

	logical ini
	data ini /.FALSE./

	if( DEBUGLEVEL .gt. 0 ) call CDump("C0soft", para, perm)

	s = Px(2)
	m1 = Px(1)
	m2 = Px(3)

	a = sqrt(4*m1*m2)
	if( abs(a) .lt. zeroeps ) then
	  ps = max(minmass, 1D-14)
	  if( abs(m1) .lt. zeroeps ) m1 = ps
	  if( abs(m2) .lt. zeroeps ) m2 = ps
	  if( .not. ini ) then
	    print *, "collinear-divergent C0, using mass cutoff ", ps
	    ini = .TRUE.
	  endif
	endif

	if( abs(s) .lt. diffeps ) then
	  if( abs(m1 - m2) .lt. diffeps ) then
	    res = -.5D0*log(m1/lambda)/m1
	  else
	    res = -.25D0*log(m2*m1/lambda**2)*
     &        log(m1/m2)/(m1 - m2)
	  endif
	  return
	endif

	ps = s - m1 - m2
	a = (ps - a)*(ps + a)
	if( a .lt. 0 ) then
	  print *, "C0soft: complex square-root not implemented"
	  a = 0
	endif
	a = sqrt(a)
	if( ps .le. 0 ) then
	  h1 = .5D0*(a - ps)
	else
	  h1 = -2*m1*m2/(a + ps)
	endif
	ps = s - m1 + m2
	if( ps .le. 0 ) then
	  h2 = .5D0*(a - ps)
	else
	  h2 = -2*s*m2/(a + ps)
	endif
	ps = s + m1 - m2
	if( ps .le. 0 ) then
	  h3 = .5D0*(a - ps)
	else
	  h3 = -2*m1*s/(a + ps)
	endif

	ls = ln(-a/s, -1)
	res = (-pi6 +
     &    spence(0, ToComplex(h2/a), -1D0) +
     &    spence(0, ToComplex(h3/a), -1D0) -
     &    .5D0*(ln(-h2/s, -1)**2 + ln(-h3/s, -1)**2) +
     &    .25D0*(ln(-m1/s, -1)**2 + ln(-m2/s, -1)**2) -
     &    ls*(ln(-h1/s, -1) - ls) +
     &    ln(-lambda/s, -1)*ln(h1/sqrt(m1*m2), 1))/a

	if( DEBUGLEVEL .gt. 1 ) print *, "C0soft =", res
	end

************************************************************************

	subroutine C0coll(res, para, perm)
	implicit none
	ComplexType res
	RealType para(1,*)
	integer perm

#include "lt.h"

	logical ini
	data ini /.FALSE./

	if( DEBUGLEVEL .gt. 0 ) call CDump("C0coll", para, perm)

	Px(1) = max(minmass, 1D-14)
	if( ini ) then
	  print *, "collinear-divergent C0, using mass cutoff ", Px(1)
	  ini = .TRUE.
	endif

	perm = 0
	end

************************************************************************

	subroutine C0softDR(res, para, perm)
	implicit none
	ComplexType res(0:2)
	RealType para(1,*)
	integer perm

#include "lt.h"

	RealType s, m1, m2
	RealType m, dm, r, f
	ComplexType root, fac, ls, lm, mK, lmK

	ComplexType Li2omx2, spence
	external Li2omx2, spence

	if( DEBUGLEVEL .gt. 0 ) call CDump("C0softDR", para, perm)

	s = Px(2)
	m1 = Px(1)
	m2 = Px(3)

	m = sqrt(m1*m2)
	if( abs(m) .lt. zeroeps ) then
	  if( abs(m1) .lt. zeroeps ) then
	    m1 = m2
	    if( abs(m1) .lt. zeroeps ) then
	      if( abs(s) .lt. zeroeps ) then
	        print *, "C0softDR: all scales zero"
	        res(0) = nan
	        res(1) = nan
	        res(2) = nan
	        goto 9
	      endif
* qltri1
	      if( DEBUGLEVEL .gt. 1 ) print *, "C0softDR: qltri1"
	      lm = lnrat(mudim, -s)
	      res(2) = 1/s
	      res(1) = lm/s
	      res(0) = .5D0*lm*res(1)
	      goto 9
	    endif
	  endif
	  if( abs(s - m1) .lt. diffeps ) then
* qltri5
	    if( DEBUGLEVEL .gt. 1 ) print *, "C0softDR: qltri5"
	    f = -.5D0/m1
	    res(1) = f
	    res(0) = f*(lnrat(mudim, m1) - 2)
	    res(2) = 0
	    goto 9
	  endif

* qltri4
	  if( DEBUGLEVEL .gt. 1 ) print *, "C0softDR: qltri4"
	  ls = lnrat(m1, m1 - s)
	  lm = lnrat(mudim, m1)
	  f = .5D0/(s - m1)
	  res(2) = f
	  res(1) = f*(lm + 2*ls)
	  res(0) = f*((ls + lm*(1 - 1/sqrt2))*(ls + lm*(1 + 1/sqrt2)) +
     &      pi6 - 2*spence(0, ToComplex(s/(s - m1)), 0D0))
	  goto 9
	endif

* qltri6
	if( DEBUGLEVEL .gt. 1 ) print *, "C0softDR: qltri6"
	dm = sqrt(m1) - sqrt(m2)
	r = s - dm**2
	root = sqrt(ToComplex((r - 4*m)/r))
	mK = -4*m/(r*(1 + root)**2)

	if( abs(mK - 1) .lt. diffeps ) then
	  r = 0
	  if( abs(m1 - m2) .gt. diffeps )
     &      r = 2 + .5D0*(sqrt(m1) + sqrt(m2))/dm*log(m2/m1)
	  f = .5D0/m
	  res(1) = f
	  res(0) = f*(log(mudim/m) - r)
	  res(2) = 0
	  goto 9
	endif

	lmK = ln(mK, 1)
	fac = 1/(r*root)
	res(0) = fac*( lmK*(.5D0*lmK + log(mudim/m)) -
     &    .125D0*log(m1/m2)**2 +
     &    Li2omx2(mK, 1D0, mK, 1D0) -
     &    Li2omx2(mK, 1D0, ToComplex(sqrt(m1/m2)), 0D0) -
     &    Li2omx2(mK, 1D0, ToComplex(sqrt(m2/m1)), 0D0) )
	res(1) = fac*lmK
	res(2) = 0

9	if( DEBUGLEVEL .gt. 1 ) then
	  print *, "C0softDR:0 =", res(0)
	  print *, "C0softDR:1 =", res(1)
	  print *, "C0softDR:2 =", res(2)
	endif
	end

************************************************************************

	subroutine C0collDR(res, para, perm)
	implicit none
	ComplexType res(0:2)
	RealType para(1,*)
	integer perm

#include "lt.h"

	RealType s1, s2, m
	RealType m1, m2, r
	ComplexType l1, l2, lm

	ComplexType Li2omrat
	external Li2omrat

	if( DEBUGLEVEL .gt. 0 ) call CDump("C0collDR", para, perm)

	m = Mx(3)
	s1 = Px(2)
	s2 = Px(3)

	if( abs(m) .lt. zeroeps ) then
* qltri2
	  if( DEBUGLEVEL .gt. 1 ) print *, "C0collDR: qltri2"
	  r = .5D0*(s2 - s1)/s1
	  if( abs(r) .lt. diffeps ) then
	    res(0) = (lnrat(mudim, -s1)*(1 - r) - r)/s1
	    res(1) = (1 - r*mudim/s1)/s1
	    res(2) = 0
	    goto 9
	  endif
	  l1 = lnrat(mudim, -s1)
	  l2 = lnrat(mudim, -s2)
	  res(1) = (l1 - l2)/(s1 - s2)
	  res(0) = .5D0*(l1 + l2)*res(1)
	  res(2) = 0
	  goto 9
	endif

* qltri3
	if( DEBUGLEVEL .gt. 1 ) print *, "C0collDR: qltri3"
	m1 = m - s1
	m2 = m - s2
	l2 = lnrat(m2, m)
	lm = lnrat(mudim, m)

	r = .5D0*(s1 - s2)/m1
	if( abs(r) .lt. diffeps ) then
	  m = m/s1
	  res(0) = (lm - (m + 1)*(l2 + r) -
     &      r*((m*(m - 2) - 1)*l2 + lm))/m1
	  res(1) = (1 - r)/m1
	  res(2) = 0
	  goto 9
	endif

	l1 = lnrat(m1, m)
	res(0) = ((lm - l1 - l2)*(l2 - l1) +
     &    Li2omrat(m1, m) - Li2omrat(m2, m))/(s1 - s2)
	res(1) = (l2 - l1)/(s1 - s2)
	res(2) = 0

9	if( DEBUGLEVEL .gt. 1 ) then
	  print *, "C0collDR:0 =", res(0)
	  print *, "C0collDR:1 =", res(1)
	  print *, "C0collDR:2 =", res(2)
	endif
	end

