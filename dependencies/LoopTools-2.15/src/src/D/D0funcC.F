* D0funcC.F
* the scalar four-point function with complex masses
* this file is part of LoopTools
* last modified 16 Dec 19 th

#include "externals.h"
#include "types.h"

#define npoint 4 
#include "defs.h"


	subroutine D0funcC(res, res00, para)
	implicit none
	ComplexType res(0:2), res00(0:2), para(1,*)

#include "lt.h"

	external D0CsoftDR, D0CcollDR, D0Csoft, D0Ccoll

	res = 0
	res00 = 0

	if( lambda .le. 0 ) then
	  call DCDispatch(res, para, D0CsoftDR, D0CcollDR)
	else
	  call DCDispatch(res, para, D0Csoft, D0Ccoll)
	endif
	end

************************************************************************

	subroutine DCDispatch(res, para, soft, coll)
	implicit none
	ComplexType res(0:2), para(1,*)
	external soft, coll

#include "lt.h"
#include "perm.h"

	integer i, z, s, perm, ier, key
	ComplexType alt

	integer pperm(12)
	data pperm /
     &    p1234, p1243, p1324,
     &    p2341, p2431, p2314,
     &    p3412, p3142, p3421,
     &    p4123, p4132, p4213 /

* 0 1 1xxx  O'1234561234'
* 1 2 12xx  O'1234561234'
* 2 2 13xx  O'5264131324'
* 3 3 123x  O'1234561234'
* 4 2 14xx  O'4321561432'
* 5 3 124x  O'1635421243'
* 6 3 134x  O'5361421342'
* 7 4 xxxx  O'1234561234'

#define pj(p,j) ibits(p,3*(10-j),3)
#define mj(p,j) ibits(p,3*(4-j),3)

#define Pc(j) P(pj(perm,j))
#define Mc(j) M(mj(perm,j))
#define Px(j) Re(Pc(j))

	z = 0
	s = 0
	do i = 1, 12
	  perm = pperm(i)
c	PRINT '(I3,O12)', i, perm
c	PRINT '(6F14.2)', Px(1), Px(2), Px(3), Px(4), Px(5), Px(6)
c	PRINT '(8F14.2)', Mc(1), Mc(2), Mc(3), Mc(4)
	  if( abs(Mc(1)) .lt. zeroeps ) then
	    if( abs(Px(1)) + abs(Mc(2)) .lt. zeroeps ) then
	      if( DEBUGLEVEL .gt. 0 )
     &          print '("collinear D0C, perm = ",O10)', perm
	      call coll(res, para, perm)
	      if( perm .eq. 0 ) return
	    endif
	    if( s .eq. 0 .and.
     &        abs(Px(1) - Mc(2)) +
     &        abs(Px(4) - Mc(4)) .lt. diffeps ) s = perm
	    if( z .eq. 0 ) z = perm
	  endif
	enddo

	if( s .ne. 0 ) then
	  if( DEBUGLEVEL .gt. 0 )
     &      print '("soft D0C, perm = ",O10)', s
	  call soft(res, para, s)
	  return
	endif

	key = ibits(versionkey, KeyD0C, 2)

	if( key .ne. 1 ) then
	  call ffd0c(res(0), para, 0, ier)
	  if( key .eq. 0 ) return
	  alt = res(0)
	endif

	ier = 0
	call ffd0c(res(0), para, 1, ier)

	if( key .gt. 1 .and.
     &      abs(res(0) - alt) .gt. maxdev*abs(alt) ) then
	  print *, "Discrepancy in D0C:"
	  print *, "  p1   =", P(1)
	  print *, "  p2   =", P(2)
	  print *, "  p3   =", P(3)
	  print *, "  p4   =", P(4)
	  print *, "  p1p2 =", P(5)
	  print *, "  p2p3 =", P(6)
	  print *, "  m1   =", M(1)
	  print *, "  m2   =", M(2)
	  print *, "  m3   =", M(3)
	  print *, "  m4   =", M(4)
	  print *, "D0C a  =", alt
	  print *, "D0C b  =", res(0)
	  if( ier .le. errdigits ) res(0) = alt
	endif

	if( .not. btest(key, 1) ) res(0) = alt
	end

************************************************************************

	subroutine DCDump(s, para, perm)
	implicit none
	character*(*) s
	ComplexType para(1,*)
	integer perm

#include "lt.h"

	print '(A,", perm = ",O4)', s, iand(perm, O'7777')
	if( DEBUGLEVEL .gt. 1 ) then
	  print *, "p1   =", Px(1)
	  print *, "p2   =", Px(2)
	  print *, "p3   =", Px(3)
	  print *, "p4   =", Px(4)
	  print *, "p1p2 =", Px(5)
	  print *, "p2p3 =", Px(6)
	  print *, "m1   =", Mc(1)
	  print *, "m2   =", Mc(2)
	  print *, "m3   =", Mc(3)
	  print *, "m4   =", Mc(4)
	endif
	end

************************************************************************

	subroutine D0Csoft(res, para, perm)
	implicit none
	ComplexType res, para(1,*)
	integer perm

#include "lt.h"

	RealType p1, p2, p3, p4, p1p2, p2p3
	RealType r1, r4
	ComplexType m3, r3
	ComplexType xs, x2, x3, y, c, fac
	ComplexType lxs, lx2, lx3, l1x2, l1x3, ly, lm
	integer ier

	ComplexType spence, bdK, bdKC, zfflo1
	external spence, bdK, bdKC, zfflo1

	m3 = Mc(3)
	p1 = Px(1)
	p2 = Px(2)
	p3 = Px(3)
	p4 = Px(4)
	p1p2 = Px(5)
	p2p3 = Px(6)

	if( DEBUGLEVEL .gt. 0 ) call DCDump("D0Csoft", para, perm)

	ier = 0

	r1 = sqrt(p1)
	r4 = sqrt(p4)
	fac = .5D0/(r1*r4*(p1p2 - m3))
	xs = bdK(p2p3, r1, r4)
	lxs = -1
	if( xs .ne. 1 ) then
	  lxs = log(xs)
	  fac = 2*xs/((1 - xs)*(1 + xs))*fac
	endif

* massless case
* (should have been re-routed to real D0, keep here for safety)
	if( abs(m3) .lt. zeroeps ) then
	  if( abs(p1 - p2) + abs(p3 - p4) .lt. diffeps ) then
	    res = -2*ln(-lambda/p1p2, 1)*lxs*fac
	    return
	  endif
	  y = (r1*(p3 - p4 + cIeps))/(r4*(p2 - p1 + cIeps))
	  ly = log(y)
	  c = ln(lambda/(r1*r4), 0) +
     &      ln((p2 - p1)/p1p2, p1 - p2) +
     &      ln((p3 - p4)/p1p2, p4 - p3)
	  if( xs .eq. 1 ) then
	    res = fac*(c - 2 - (1 + y)/(1 - y)*ly)
	  else
	    res = fac*(pi6 -
     &        spence(0, xs/y, 0D0) -
     &        (lxs + log(1/y))*zfflo1(xs/y, ier) -
     &        spence(0, xs*y, 0D0) -
     &        (lxs + ly)*(zfflo1(xs*y, ier) + .5D0*(lxs - ly)) +
     &        spence(0, xs**2, 0D0) +
     &        lxs*(2*zfflo1(xs**2, ier) - c))
	  endif
	  return
	endif

* massive case
	r3 = sqrt(m3)
	x2 = bdKC(p2, r1, r3)
	x3 = bdKC(p3, r4, r3)
	lx2 = log(x2)
	lx3 = log(x3)
	l1x3 = log(1/x3)
	lm = 2*ln(r3*sqrt(lambda)/(m3 - p1p2), 1)
	if( xs .eq. 1 ) then
	  c = -2
	  if( abs(x2 - x3) .gt. diffeps ) then
	    c = (1 + x2/x3)/(1 - x2/x3)*(lx2 + l1x3) +
     &        (1 + x2*x3)/(1 - x2*x3)*(lx2 + lx3) + 2
	  else if( abs(x2 - 1) .gt. diffeps ) then
	    c = -2*(x2**2 + 1)/((x2 - 1)*(x2 + 1))*lx2
	  endif
	  res = fac*(lm - c)
	else
	  l1x2 = log(1/x2)
	  res = fac*( .5D0*pi**2 +
     &      lxs*(2*zfflo1(xs**2, ier) - lm) +
     &      spence(0, xs**2, 0D0) + lx2**2 + lx3**2 -
     &      spence(0, xs/(x2*x3), 0D0) -
     &      (lxs + l1x2 + l1x3)*zfflo1(xs/(x2*x3), ier) -
     &      spence(0, xs*x2/x3, 0D0) -
     &      (lxs + lx2 + l1x3)*zfflo1(xs*x2/x3, ier) -
     &      spence(0, xs/x2*x3, 0D0) -
     &      (lxs + l1x2 + lx3)*zfflo1(xs/x2*x3, ier) -
     &      spence(0, xs*x2*x3, 0D0) -
     &      (lxs + lx2 + lx3)*zfflo1(xs*x2*x3, ier) )
	endif
	end

************************************************************************

	ComplexType function bdKC(x, m1, m2)
* this is actually -K from the Beenakker/Denner paper for D0soft
	implicit none
	RealType x, m1
	ComplexType m2

#include "lt.h"

	ComplexType d, t

	d = x - (m1 - m2)**2
	if( abs(d) .lt. diffeps ) then
	  bdKC = 1
	else
	  t = 4*m1*m2/(d + cIeps)
	  bdKC = -t/(sqrt(1 - t) + 1)**2
	endif
	end

************************************************************************

	subroutine D0Ccoll(res, para, perm)
	implicit none
	ComplexType res, para(1,*)
	integer perm

#include "lt.h"

	logical ini
	data ini /.FALSE./

	if( DEBUGLEVEL .gt. 0 ) call DCDump("D0coll", para, perm)

	Pc(1) = max(minmass, 1D-14)

	if( ini ) return
	print *, "collinear-divergent D0C, using mass cutoff ", Px(1)
	ini = .TRUE.
	end

************************************************************************
* IR-divergent D0 in dim reg
* from W. Beenakker and A. Denner, NPB 338 (1990) 349

	subroutine D0CsoftDR(res, para, perm)
	implicit none
	ComplexType res(0:2), para(1,*)
	integer perm

#include "lt.h"

	RealType m2, m4, p2, p3, p2p3
	RealType r1, r4, m24, sy, q2, q3
	ComplexType m3, r3, t, fac
	ComplexType c, xs, x2, x3, lxs, lx2, lx3, lm, y

	ComplexType bdK, bdKC, Li2omx2, Li2omx3
	external bdK, bdKC, Li2omx2, Li2omx3

	if( DEBUGLEVEL .gt. 0 ) call DCDump("D0CsoftDR", para, perm)

	m3 = Mc(3)
	t = m3 - Px(5)
	p2p3 = Px(6)

	m2 = Px(1)
	p2 = Px(2)
	q2 = m2 - p2
	m4 = Px(4)
	p3 = Px(3)
	q3 = m4 - p3

	r1 = sqrt(m2)
	r4 = sqrt(m4)

	fac = .5D0/(r1*r4*t)
	xs = bdK(p2p3, r1, r4)
	lxs = -1
	if( xs .ne. 1 ) then
	  lxs = log(xs)
	  fac = 2*xs/((1 - xs)*(1 + xs))*fac
	endif

	res(1) = fac*lxs
	res(2) = 0

	if( abs(m3) .lt. zeroeps ) then
* (should have been re-routed to real D0, keep here for safety)
	  if( abs(q2) + abs(q3) .lt. diffeps ) then
* qlbox14: D0(m2, m2, m4, m4; p1p2, p2p3; 0, m2, 0, m4)
	    if( DEBUGLEVEL .gt. 1 ) print *, "D0CsoftDR: qlbox14"
	    res(1) = 2*res(1)
	    res(0) = res(1)*lnrat(mudim, t)
	    return
	  endif

* qlbox15: D0(m2, p2, p3, m4; p1p2, p2p3; 0, m2, 0, m4)
* Beenakker-Denner Eq. (2.11)
	  if( DEBUGLEVEL .gt. 1 ) print *, "D0CsoftDR: qlbox15"

	  if( abs(q2*q3) .lt. diffeps ) then
	    m24 = m2
	    if( abs(q2) .lt. diffeps ) m24 = m4
	    res(0) = fac*( lxs*(lxs + log(mudim/m24) +
     &        2*lnrat(q2 + q3, t)) +
     &        Li2omx2(xs, 1D0, xs, 1D0) )
	    return
	  endif

	  y = r1*q3/(r4*q2)
	  sy = sign(.5D0, r1*q3) - sign(.5D0, r4*q2)

	  if( xs .eq. 1 ) then
	    res(0) = fac*( -log(mudim/(r1*r4)) +
     &        lnrat(q2, t) + lnrat(q3, t) + 2 +
     &        (1 + y)/(1 - y)*ln(y, sy) )
	  else
	    res(0) = fac*( -.5D0*ln(y, sy)**2 +
     &        lxs*(.5D0*lxs + lnrat(q2, t) + lnrat(q3, t) +
     &          log(mudim/(r1*r4))) +
     &        Li2omx2(xs, 1D0, xs, 1D0) -
     &        Li2omx2(xs, 1D0, y, sy) -
     &        Li2omx2(xs, 1D0, 1/y, -sy) )
	  endif
	  return
	endif

* qlbox16: D0(m2, p2, p3, m4; p1p2, p2p3; 0, m2, m3, m4)
* Beenakker-Denner Eq. (2.9)
	if( DEBUGLEVEL .gt. 1 ) print *, "D0softDR: qlbox16"

	r3 = sqrt(m3)
	x2 = bdKC(p2, r1, r3)
	x3 = bdKC(p3, r4, r3)
	lx2 = log(x2)
	lx3 = log(x3)

	lm = 2*lnrat(sqrt(m3*mudim), t)

	if( xs .eq. 1 ) then
	  c = -2
	  if( abs(x2 - x3) .gt. diffeps ) then
	    c = (1 + x2/x3)/(1 - x2/x3)*(lx2 + log(1/x3)) +
     &        (1 + x2*x3)/(1 - x2*x3)*(lx2 + lx3) + 2
	  else if( abs(x2 - 1) .gt. diffeps ) then
	    c = -2*(x2**2 + 1)/((x2 - 1)*(x2 + 1))*lx2
	  endif
	  res(0) = fac*(c - lm)
	else
	  res(0) = fac*(lm*lxs - lx2**2 - lx3**2 +
     &      Li2omx2(xs, 1D0, xs, 1D0) -
     &      Li2omx3(xs, 1D0, x2, 1D0, x3, 1D0) -
     &      Li2omx3(xs, 1D0, 1/x2, -1D0, 1/x3, -1D0) -
     &      Li2omx3(xs, 1D0, x2, 1D0, 1/x3, -1D0) -
     &      Li2omx3(xs, 1D0, 1/x2, -1D0, x3, 1D0))
	endif
	end

************************************************************************

	subroutine D0CcollDR(res, para, perm_)
	implicit none
	ComplexType res(0:2), para(1,*)
	integer perm_

#include "lt.h"
#include "perm.h"

	integer perm, z, s

* # of non-zero momenta
	integer nz1, nz2, nz3
	parameter (nz1 = 1073741824)	! O'10000000000'
	parameter (nz2 = -2147483648)	! O'20000000000'
	parameter (nz3 = -1073741824)	! O'30000000000'

	integer nz1p1234, nz2p1234, nz3p1234
	parameter (nz1p1234 = nz1 + p1234)
	parameter (nz2p1234 = nz2 + p1234)
	parameter (nz3p1234 = nz3 + p1234)
	integer nz1p1243, nz2p1243, nz3p1243
	parameter (nz1p1243 = nz1 + p1243)
	parameter (nz2p1243 = nz2 + p1243)
	parameter (nz3p1243 = nz3 + p1243)
	integer nz1p2134, nz2p2134, nz3p2134
	parameter (nz1p2134 = nz1 + p2134)
	parameter (nz2p2134 = nz2 + p2134)
	parameter (nz3p2134 = nz3 + p2134)
	integer nz1p2143, nz2p2143, nz3p2143
	parameter (nz1p2143 = nz1 + p2143)
	parameter (nz2p2143 = nz2 + p2143)
	parameter (nz3p2143 = nz3 + p2143)
	integer nz1p3214, nz2p3214, nz3p3214
	parameter (nz1p3214 = nz1 + p3214)
	parameter (nz2p3214 = nz2 + p3214)
	parameter (nz3p3214 = nz3 + p3214)
	integer nz1p4213, nz2p4213, nz3p4213
	parameter (nz1p4213 = nz1 + p4213)
	parameter (nz2p4213 = nz2 + p4213)
	parameter (nz3p4213 = nz3 + p4213)

	integer pperm(0:127)
	data pperm /
* 1ppppp12mm	0ppp	1
*         3m	0ppp	1
*         m4	0ppp	1432652143
*         34	0ppp	1
     &    nz3p1234, nz3p1234, nz3p2143, nz3p1234,
* 12pppp12mm	0ppp	1
*         3m	00pp	1
*         m4	0ppp	1432652143
*         34	00pp	1
     &    nz3p1234, nz2p1234, nz3p2143, nz2p1234,
* 1p3ppp12mm	0ppp	1
*         3m	0ppp	1
*         m4	0ppp	1432652143
*         34	0p0p	1
     &    nz3p1234, nz3p1234, nz3p2143, nz2p1234,
* 123ppp12mm	0ppp	1
*         3m	00pp	1
*         m4	0ppp	1432652143
*         34	000p	1
     &    nz3p1234, nz2p1234, nz3p2143, nz1p1234,
* 1pp4pp12mm	0ppp	1
*         3m	0ppp	1
*         m4	00pp	1432652143
*         34	00pp	1432652143
     &    nz3p1234, nz3p1234, nz2p2143, nz2p2143,
* 12p4pp12mm	0ppp	1
*         3m	00pp	1
*         m4	00pp	1432652143
*         34	000p	2143563214
     &    nz3p1234, nz2p1234, nz2p2143, nz1p3214,
* 1p34pp12mm	0ppp	1
*         3m	0ppp	1
*         m4	00pp	1432652143
*         34	000p	1432652143
     &    nz3p1234, nz3p1234, nz2p2143, nz1p2143,
* 1234pp12mm	0ppp	1
*         3m	00pp	1
*         m4	00pp	1432652143
*         34	0000	1
     &    nz3p1234, nz2p1234, nz2p2143, p1234,
* 1ppp5p12mm	0ppp	1
*         3m	00pp	1536242134
*         m4	0ppp	1432652143
*         34	00pp	1536242134
     &    nz3p1234, nz2p2134, nz3p2143, nz2p2134,
* 12pp5p12mm	0ppp	1
*         3m	00pp	1
*         m4	0ppp	1432652143
*         34	00pp	1
     &    nz3p1234, nz2p1234, nz3p2143, nz2p1234,
* 1p3p5p12mm	0ppp	1
*         3m	00pp	1536242134
*         m4	0ppp	1432652143
*         34	000p	1536242134
     &    nz3p1234, nz2p2134, nz3p2143, nz1p2134,
* 123p5p12mm	0ppp	1
*         3m	00pp	1536242134
*         m4	0ppp	1432652143
*         34	000p	1
     &    nz3p1234, nz2p2134, nz3p2143, nz1p1234,
* 1pp45p12mm	0ppp	1
*         3m	00pp	1536242134
*         m4	00pp	1432652143
*         34	00pp	1432652143
     &    nz3p1234, nz2p2134, nz2p2143, nz2p2143,
* 12p45p12mm	0ppp	1
*         3m	00pp	1
*         m4	00pp	1432652143
*         34	000p	2143563214
     &    nz3p1234, nz2p1234, nz2p2143, nz1p3214,
* 1p345p12mm	0ppp	1
*         3m	00pp	1536242134
*         m4	00pp	1432652143
*         34	000p	1432652143
     &    nz3p1234, nz2p2134, nz2p2143, nz1p2143,
* 12345p12mm	0ppp	1
*         3m	00pp	1
*         m4	00pp	1432652143
*         34	0000	1
     &    nz3p1234, nz2p1234, nz2p2143, p1234,
* 1pppp612mm	0ppp	1
*         3m	0ppp	1
*         m4	00pp	1635421243
*         34	00pp	1635421243
     &    nz3p1234, nz3p1234, nz2p1243, nz2p1243,
* 12ppp612mm	0ppp	1
*         3m	00pp	1
*         m4	00pp	1635421243
*         34	00pp	1
     &    nz3p1234, nz2p1234, nz2p1243, nz2p1234,
* 1p3pp612mm	0ppp	1
*         3m	0ppp	1
*         m4	00pp	1635421243
*         34	000p	1635421243
     &    nz3p1234, nz3p1234, nz2p1243, nz1p1243,
* 123pp612mm	0ppp	1
*         3m	00pp	1
*         m4	00pp	1635421243
*         34	000p	1
     &    nz3p1234, nz2p1234, nz2p1243, nz1p1234,
* 1pp4p612mm	0ppp	1
*         3m	0ppp	1
*         m4	00pp	1635421243
*         34	00pp	1432652143
     &    nz3p1234, nz3p1234, nz2p1243, nz2p2143,
* 12p4p612mm	0ppp	1
*         3m	00pp	1
*         m4	00pp	1635421243
*         34	000p	2143563214
     &    nz3p1234, nz2p1234, nz2p1243, nz1p3214,
* 1p34p612mm	0ppp	1
*         3m	0ppp	1
*         m4	00pp	1635421243
*         34	000p	1432652143
     &    nz3p1234, nz3p1234, nz2p1243, nz1p2143,
* 1234p612mm	0ppp	1
*         3m	00pp	1
*         m4	00pp	1635421243
*         34	0000	1
     &    nz3p1234, nz2p1234, nz2p1243, p1234,
* 1ppp5612mm	0ppp	1
*         3m	00pp	1536242134
*         m4	00pp	1635421243
*         34	000p	6153424213
     &    nz3p1234, nz2p2134, nz2p1243, nz1p4213,
* 12pp5612mm	0ppp	1
*         3m	00pp	1
*         m4	00pp	1635421243
*         34	000p	6153424213
     &    nz3p1234, nz2p1234, nz2p1243, nz1p4213,
* 1p3p5612mm	0ppp	1
*         3m	00pp	1536242134
*         m4	00pp	1635421243
*         34	0000	6153424213
     &    nz3p1234, nz2p2134, nz2p1243, p4213,
* 123p5612mm	0ppp	1
*         3m	00pp	1
*         m4	00pp	1635421243
*         34	0000	6153424213
     &    nz3p1234, nz2p1234, nz2p1243, p4213,
* 1pp45612mm	0ppp	1
*         3m	00pp	1536242134
*         m4	00pp	1635421243
*         34	000p	6153424213
     &    nz3p1234, nz2p2134, nz2p1243, nz1p4213,
* 12p45612mm	0ppp	1
*         3m	00pp	1
*         m4	00pp	1635421243
*         34	0000	6254314231
     &    nz3p1234, nz2p1234, nz2p1243, p4231,
* 1p345612mm	0ppp	1
*         3m	00pp	1536242134
*         m4	00pp	1635421243
*         34	0000	6153424213
     &    nz3p1234, nz2p2134, nz2p1243, p4213,
* 12345612mm	0ppp	1
*         3m	00pp	1
*         m4	00pp	1635421243
*         34	0000	1
     &    nz3p1234, nz2p1234, nz2p1243, p1234 /

	if( DEBUGLEVEL .gt. 0 ) call DCDump("D0CcollDR", para, perm)

	perm = perm_
	perm_ = 0

	z = 0
	if( abs(Mc(3)) .lt. zeroeps ) z = 1
	if( abs(Mc(4)) .lt. zeroeps ) z = z + 2
	if( abs(Px(2)) .lt. zeroeps ) z = z + 4
	if( abs(Px(3)) .lt. zeroeps ) z = z + 8
	if( abs(Px(4)) .lt. zeroeps ) z = z + 16
	if( abs(Px(5)) .lt. zeroeps ) z = z + 32
	if( abs(Px(6)) .lt. zeroeps ) z = z + 64
	s = pperm(z)
	if( iand(s, O'7777777777') .ne. p1234 ) perm =
     &    pj(perm, pj(s, 1))*8**9 +
     &    pj(perm, pj(s, 2))*8**8 +
     &    pj(perm, pj(s, 3))*8**7 +
     &    pj(perm, pj(s, 4))*8**6 +
     &    pj(perm, pj(s, 5))*8**5 +
     &    pj(perm, pj(s, 6))*8**4 +
     &    mj(perm, mj(s, 1))*8**3 +
     &    mj(perm, mj(s, 2))*8**2 +
     &    mj(perm, mj(s, 3))*8**1 +
     &    mj(perm, mj(s, 4))*8**0

	goto (22,22,22,23, 22,22,22,23, 10,11,12,13)
     &    ibits(s, 30, 2) + ibits(z, 0, 2)*4 - 3

	call D0Cm2p3(res, para, perm)
	return

23	call D0Cm1p3(res, para, perm)
	return

22	call D0Cm1p2(res, para, perm)
	return

13	call D0m0p3(res, para,2, perm)
	return

12	call D0m0p2(res, para,2, perm)
	return

11	call D0m0p1(res, para,2, perm)
	return

10	call D0m0p0(res, para,2, perm)
	end

************************************************************************

	subroutine D0Cm1p2(res, para, perm)
	implicit none
	ComplexType res(0:2), para(1,*)
	integer perm

#include "lt.h"

	ComplexType s
	ComplexType m4, t, q3, q4, fac
	ComplexType lm, ls, lt, lq
	integer ir

	ComplexType Li2omrat, cLi2omrat, cLi2omrat2
	external Li2omrat, cLi2omrat, cLi2omrat2

	if( DEBUGLEVEL .gt. 1 ) call DCDump("D0Cm1p2", para, perm)

	m4 = Mc(4)
	s = -Px(5)
	t = m4 - Px(6)
	fac = 1/(s*t)
	q3 = m4 - Px(3)
	q4 = m4 - Px(4)

	ir = 0
	if( abs(q3) .lt. diffeps ) ir = 1
	if( abs(q4) .lt. diffeps ) then
	  ir = ir + 1
	  q4 = q3
	endif

	res(2) = .5D0*(2 + ir)*fac

	goto (1, 2) ir

* qlbox8: D0(0, 0, p3, p4; p1p2, p2p3; 0, 0, 0, m4)
	if( DEBUGLEVEL .gt. 1 ) print *, "D0Cm1p2: qlbox8"
	lm = lnrat(s, mudim)
	ls = lnrat(s, m4)
	res(0) = fac*(-2*(cLi2omrat(q3, t) + cLi2omrat(q4, t)) -
     &    cLi2omrat2(q3, s, q4, m4) - pi6 +
     &    .5D0*(lm - ls)*(lm + ls) + 2*lm*lnrat(t, m4) -
     &    lnrat(q3, mudim)*lnrat(q3, m4) -
     &    lnrat(q4, mudim)*lnrat(q4, m4))
	res(1) = fac*(lnrat(q3, t) + lnrat(q4, t) - lm)
	return

1	continue
* qlbox7: D0(0, 0, m4, p4; p1p2, p2p3; 0, 0, 0, m4)
* (should have been re-routed to real D0, keep here for safety)
	if( DEBUGLEVEL .gt. 1 ) print *, "D0Cm1p2: qlbox7"
	ls = lnrat(s, m4)
	lt = lnrat(t, m4)
	lm = lnrat(mudim, m4)
	lq = lnrat(q4, m4)
	res(0) = fac*(2*ls*lt - lq**2 - 5*pi12 +
     &    lm*(.75D0*lm - 2*lt - ls + lq) -
     &    2*Li2omrat(q4, t))
	res(1) = fac*(1.5D0*lm - 2*lt - ls + lq)
	return

2	continue
* qlbox6: D0(0, 0, m4, m4; p1p2, p2p3; 0, 0, 0, m4)
* (should have been re-routed to real D0, keep here for safety)
	if( DEBUGLEVEL .gt. 1 ) print *, "D0Cm1p2: qlbox6"
	ls = lnrat(s, m4)
	lt = lnrat(t, m4)
	lm = lnrat(mudim, m4)
	res(0) = fac*((lm - ls)*(lm - 2*lt) - .5D0*pi**2)
	res(1) = fac*(2*(lm - lt) - ls)
	end

************************************************************************

	subroutine D0Cm1p3(res, para, perm)
	implicit none
	ComplexType res(0:2), para(1,*)
	integer perm

#include "lt.h"

	ComplexType s, q2
	ComplexType m4, m4mu, q3, q4, t, fac, ll

	ComplexType Li2omrat, cLi2omrat, Li2omrat2, cLi2omrat2
	external Li2omrat, cLi2omrat, Li2omrat2, cLi2omrat2

	if( DEBUGLEVEL .gt. 1 ) call DCDump("D0Cm1p3", para, perm)

	q2 = -Px(2)
	s = -Px(5)
	m4 = Mc(4)
	q3 = m4 - Px(3)
	q4 = m4 - Px(4)
	t = m4 - Px(6)

	if( abs(t) .lt. diffeps ) then
	  t = q4
	  q4 = 0
	  s = q2
	  q2 = -Px(5)
	endif

	m4mu = sqrt(m4*mudim)

* qlbox9: D0(0, p2, p3, m4; p1p2, p2p3; 0, 0, 0, m4)
* (should have been re-routed to real D0, keep here for safety)
	if( abs(q4) .lt. diffeps ) then
	  if( DEBUGLEVEL .gt. 1 ) print *, "D0Cm1p3: qlbox9"
	  fac = 1/(Re(s)*Re(t))
	  ll = lnrat(t, m4mu) + lnrat(s, q2)
	  res(0) = fac*(Li2omrat2(q3, q2, t, m4) +
     &      2*Li2omrat(s, q2) + ll**2 + pi12)
	  res(1) = -fac*ll
	  res(2) = .5D0*fac
	  return
	endif

* qlbox10: D0(0, p2, p3, p4; p1p2, p2p3; 0, 0, 0, m4)
	if( DEBUGLEVEL .gt. 1 ) print *, "D0Cm1p3: qlbox10"
	fac = 1/(Re(s)*t - Re(q2)*q4)
	ll = lnrat(q2, mudim) + lnrat(q4, mudim) -
     &    lnrat(s, mudim) - lnrat(t, mudim)
	res(0) = fac*(
     &    2*ll*lnrat(m4mu, t) +
     &    cLi2omrat2(q3, q2, t, m4) -
     &    cLi2omrat2(q3, s, q4, m4) +
     &    2*(cLi2omrat2(q2, s, q4, t) -
     &      cLi2omrat(q2, s) + cLi2omrat(t, q4)) )
	res(1) = fac*ll
	res(2) = 0
	end

************************************************************************

	subroutine D0Cm2p3(res, para, perm)
	implicit none
	ComplexType res(0:2), para(1,*)
	integer perm

#include "lt.h"

	RealType p3, m3mu
	ComplexType m3, m4, s, t, q3, q4, tmp, fac
	ComplexType p34, c, s3t, s4s
	ComplexType ls, lt, lq3, lq4, d
	ComplexType x43(4), r3t, r4s, r43p, r43m
	ComplexType logs, dilogs
	integer ir, case

	ComplexType minus1
	parameter (minus1 = -1)

	ComplexType Li2rat, cLi2omrat, cLi2omrat2
	external Li2rat, cLi2omrat, cLi2omrat2

	if( DEBUGLEVEL .gt. 1 ) call DCDump("D0Cm2p3", para, perm)

	m3 = Mc(3)
	s = m3 - Px(5)
	q3 = m3 - Px(2)
	m4 = Mc(4)
	t = m4 - Px(6)
	q4 = m4 - Px(4)

	if( abs(s) .lt. diffeps .or. abs(t) .lt. diffeps ) then
* switch from p1234 to p2134 = 1536242134
	  tmp = s
	  s = q3
	  q3 = tmp
	  tmp = t
	  t = q4
	  q4 = tmp
	endif

	fac = 1/(s*t - q3*q4)

	ir = 0
	if( abs(q3) .lt. diffeps ) ir = 1
	if( abs(q4) .lt. diffeps ) then
	  ir = ir + 1
	  q4 = q3
	  tmp = s
	  s = t
	  t = tmp
	  m4 = m3
	  m3 = Mc(4)
	endif

	res(2) = .5D0*fac*ir

	p3 = Px(3)
	if( abs(p3) .lt. zeroeps ) then
	  case = 1
	  logs = lnrat(m3, m4)**2
	else
	  p34 = p3 + m3 - m4
	  c = -4*p3*m3
	  d = sqrt(p34**2 + c)

	  x43(1) = -p34 - d
	  x43(2) =  p34 - d
	  if( abs(x43(1)) .lt. abs(x43(2)) ) then
	    x43(1) = c/x43(2)
	  else
	    x43(2) = c/x43(1)
	  endif

	  p34 = -p3 + m3 - m4
	  c = -4*p3*m4

	  x43(3) = -p34 - d
	  x43(4) =  p34 - d
	  if( abs(x43(3)) .lt. abs(x43(4)) ) then
	    x43(3) = c/x43(4)
	  else
	    x43(4) = c/x43(3)
	  endif

	  if( abs(Im(d)) .lt. zeroeps ) then
	    case = 2
	    logs = lnrat(x43(1), x43(3))**2 +
     &             lnrat(x43(2), x43(4))**2
	  else
	    case = 3
	    r43p = x43(1)/x43(3)
	    r43m = x43(2)/x43(4)
	    logs = ln(r43p, 0)**2 + ln(r43m, 0)**2
	  endif
	endif

	goto (1, 2) ir

* qlbox13: D0(0, p2, p3, p4; p1p2, p2p3; 0, 0, m3, m4)
	if( DEBUGLEVEL .gt. 1 ) print *, "D0Cm2p3: qlbox13"

	ls = lnrat(s, mudim)
	lt = lnrat(t, mudim)
	lq3 = lnrat(q3, mudim)
	lq4 = lnrat(q4, mudim)

	if( case .eq. 1 ) then
	  dilogs = cLi2omrat2(q3, t, minus1, minus1) +
     &             cLi2omrat2(q3, t, m4, m3) +
     &             cLi2omrat2(q4, s, m3, m4) +
     &             cLi2omrat2(q4, s, minus1, minus1)
	else if( case .eq. 2 ) then
	  dilogs = cLi2omrat2(q3, t, x43(4), x43(2)) +
     &             cLi2omrat2(q3, t, x43(3), x43(1)) +
     &             cLi2omrat2(q4, s, x43(1), x43(3)) +
     &             cLi2omrat2(q4, s, x43(2), x43(4))
	else
	  r3t = q3/t
	  s3t = sign(.5D0, Re(q3)) - sign(.5D0, Re(t))
	  r4s = q4/s
	  s4s = sign(.5D0, Re(q4)) - sign(.5D0, Re(s))
	  dilogs = Li2rat(r3t,s3t, 1/r43m,0D0) +
     &             Li2rat(r3t,s3t, 1/r43p,0D0) +
     &             Li2rat(r4s,s4s, r43p,0D0) +
     &             Li2rat(r4s,s4s, r43m,0D0)
	endif

	res(0) = -fac*(dilogs + .5D0*logs + lq3**2 + lq4**2 +
     &    2*(cLi2omrat(q3, s) + cLi2omrat(q4, t) -
     &      cLi2omrat2(q3, s, q4, t) - ls*lt) +
     &    (lt - lq3)*log(m3/mudim) + (ls - lq4)*log(m4/mudim))
	res(1) = fac*(lq3 + lq4 - ls - lt)
	return

1	continue
* qlbox12: D0(0, m3, p3, p4; p1p2, p2p3; 0, 0, m3, m4)
	if( DEBUGLEVEL .gt. 1 ) print *, "D0Cm2p3: qlbox12"

	m3mu = sqrt(Re(m3)*mudim)
	ls = lnrat(s, m3mu)
	lt = lnrat(t, m3mu)
	lq4 = lnrat(q4, m3mu)

	if( case .eq. 1 ) then
	  dilogs = cLi2omrat2(q4, s, m3, m4) +
     &             cLi2omrat2(q4, s, minus1, minus1)
	else if( case .eq. 2 ) then
	  dilogs = cLi2omrat2(q4, s, x43(1), x43(3)) +
     &             cLi2omrat2(q4, s, x43(2), x43(4))
	else
	  r4s = q4/s
	  s4s = sign(.5D0, Re(q4)) - sign(.5D0, Re(s))
	  dilogs = Li2rat(r4s,s4s, r43p,0D0) +
     &             Li2rat(r4s,s4s, r43m,0D0)
	endif

	res(0) = -fac*(dilogs + .5D0*logs + pi12 +
     &    2*(cLi2omrat(q4, t) - ls*lt) +
     &    lq4**2 + (ls - lq4)*log(m4/m3))
	res(1) = fac*(lq4 - ls - lt)
	return

2	continue
* qlbox11: D0(0, m3, p3, m4; p1p2, p2p3; 0, 0, m3, m4)

* qlbox11a: D0(0, p2, p3, p4; m3, m4; 0, 0, m3, m4)
* (should have been re-routed to real D0, keep here for safety)
	if( DEBUGLEVEL .gt. 1 ) print *, "D0Cm2p3: qlbox11"

	ls = lnrat(s, sqrt(Re(m3)*mudim))
	lt = lnrat(t, sqrt(Re(m4)*mudim))

	res(0) = fac*(.25D0*log(Re(m3)/Re(m4))**2 -
     &    .5D0*(logs + pi**2) + 2*ls*lt)
	res(1) = -fac*(ls + lt)
	end

