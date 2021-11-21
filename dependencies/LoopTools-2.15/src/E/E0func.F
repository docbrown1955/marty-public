* E0func.F
* the scalar four-point function
* this file is part of LoopTools
* written by M. Rauch
* last modified 9 Apr 18 th

#include "externals.h"
#include "types.h"

#define npoint 5
#include "defs.h"


#ifndef COMPLEXPARA
	subroutine E0func(res, para,
     &    D2345, D1345, D1245, D1235, D1234, dvalid)
	implicit none
	ComplexType res(0:2)
	RealType para(1,*)
	ComplexType D2345(*), D1345(*), D1245(*), D1235(*), D1234(*)
	integer dvalid

#include "lt.h"

	ComplexType d0(5), alt
	integer key, ier

	key = ibits(versionkey, KeyE0, 2)

	if( key .ne. 1 ) then
	  call E0funcb(res, para,
     &      D2345, D1345, D1245, D1235, D1234, dvalid)
	  if( key .eq. 0 ) return
	  alt = res(0)
	endif

	call ffxe0(res(0), d0, para, ier)
	res(1) = 0
	res(2) = 0

	if( key .gt. 1 .and.
     &      abs(res(0) - alt) .gt. maxdev*abs(alt) ) then
	  print *, "Discrepancy in E0:"
	  print *, "  p1   =", P(1)
	  print *, "  p2   =", P(2)
	  print *, "  p3   =", P(3)
	  print *, "  p4   =", P(4)
	  print *, "  p5   =", P(5)
	  print *, "  p1p2 =", P(6)
	  print *, "  p2p3 =", P(7)
	  print *, "  p3p4 =", P(8)
	  print *, "  p4p5 =", P(9)
	  print *, "  p5p1 =", P(10)
	  print *, "  m1   =", M(1)
	  print *, "  m2   =", M(2)
	  print *, "  m3   =", M(3)
	  print *, "  m4   =", M(4)
	  print *, "  m5   =", M(5)
	  print *, "E0 a   =", alt
	  print *, "E0 b   =", res(0)
	  if( ier .gt. errdigits ) alt = res(0)
	endif

	if( .not. btest(key, 0) ) res(0) = alt
	end
#endif

************************************************************************

#ifdef COMPLEXPARA
	subroutine E0funcC(
#else
	subroutine E0funcb(
#endif
     &    res, para, D2345, D1345, D1245, D1235, D1234, dvalid)
	implicit none
	ComplexType res(0:2)
	ArgType para(1,*)
	ComplexType D2345(*), D1345(*), D1245(*), D1235(*), D1234(*)
	integer dvalid

#include "lt.h"

	ArgType p1, p2, p3, p4, p5
	ArgType p1p2, p2p3, p3p4, p4p5, p5p1
	ArgType m1, m2, m3, m4, m5

	ArgQuad Y(5,5), Yi(5,5), eta(5), detY
	integer i

	m1 = M(1)
	m2 = M(2)
	m3 = M(3)
	m4 = M(4)
	m5 = M(5)
	p1 = P(1)
	p2 = P(2)
	p3 = P(3)
	p4 = P(4)
	p5 = P(5)
	p1p2 = P(6)
	p2p3 = P(7)
	p3p4 = P(8)
	p4p5 = P(9)
	p5p1 = P(10)

	Y(1,1) = 2*m1
	Y(2,2) = 2*m2
	Y(3,3) = 2*m3
	Y(4,4) = 2*m4
	Y(5,5) = 2*m5
	Y(1,2) = m1
	  Y(1,2) = Y(1,2) + m2
	  Y(1,2) = Y(1,2) - p1
	Y(2,1) = Y(1,2)
	Y(1,3) = m1
	  Y(1,3) = Y(1,3) + m3
	  Y(1,3) = Y(1,3) - p1p2
	Y(3,1) = Y(1,3)
	Y(1,4) = m1
	  Y(1,4) = Y(1,4) + m4
	  Y(1,4) = Y(1,4) - p4p5
	Y(4,1) = Y(1,4)
	Y(1,5) = m1
	  Y(1,5) = Y(1,5) + m5
	  Y(1,5) = Y(1,5) - p5
	Y(5,1) = Y(1,5)
	Y(2,3) = m2
	  Y(2,3) = Y(2,3) + m3
	  Y(2,3) = Y(2,3) - p2
	Y(3,2) = Y(2,3)
	Y(2,4) = m2
	  Y(2,4) = Y(2,4) + m4
	  Y(2,4) = Y(2,4) - p2p3
	Y(4,2) = Y(2,4)
	Y(2,5) = m2
	  Y(2,5) = Y(2,5) + m5
	  Y(2,5) = Y(2,5) - p5p1
	Y(5,2) = Y(2,5)
	Y(3,4) = m3
	  Y(3,4) = Y(3,4) + m4
	  Y(3,4) = Y(3,4) - p3
	Y(4,3) = Y(3,4)
	Y(3,5) = m3
	  Y(3,5) = Y(3,5) + m5
	  Y(3,5) = Y(3,5) - p3p4
	Y(5,3) = Y(3,5)
	Y(4,5) = m4
	  Y(4,5) = Y(4,5) + m5
	  Y(4,5) = Y(4,5) - p4
	Y(5,4) = Y(4,5)

	do i = 1, 5
	  Yi = Y
	  Yi(:,i) = 1
	  call XDet(5, Yi,5, eta(i))
	enddo

	call XDet(5, Y,5, detY)

	if( dvalid .eq. 0 ) then
	  call XD0nocache(D2345, p2, p3, p4, p5p1, p2p3, p3p4,
     &      m2, m3, m4, m5)
	  call XD0nocache(D1345, p1p2, p3, p4, p5, p4p5, p3p4,
     &      m1, m3, m4, m5)
	  call XD0nocache(D1245, p1, p2p3, p4, p5, p4p5, p5p1,
     &      m1, m2, m4, m5)
	  call XD0nocache(D1235, p1, p2, p3p4, p5, p1p2, p5p1,
     &      m1, m2, m3, m5)
	  call XD0nocache(D1234, p1, p2, p3, p4p5, p1p2, p2p3,
     &      m1, m2, m3, m4)
	endif

	res(0) = -(
     &    eta(1)*D2345(dd0) +
     &    eta(2)*D1345(dd0) +
     &    eta(3)*D1245(dd0) +
     &    eta(4)*D1235(dd0) +
     &    eta(5)*D1234(dd0)
     &  )/detY
	res(1) = 0
	res(2) = 0
	end

