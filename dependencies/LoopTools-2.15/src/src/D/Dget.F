* Dget.F
* retrieve the four-point tensor coefficients
* this file is part of LoopTools
* improvements by M. Rauch
* last modified 16 Oct 18 th

#include "externals.h"
#include "types.h"

#define npoint 4
#include "defs.h"


	subroutine XDpara(para, p1, p2, p3, p4, p1p2, p2p3,
     &    m1, m2, m3, m4)
	implicit none
	ArgType para(1,*)
	ArgType p1, p2, p3, p4, p1p2, p2p3
	ArgType m1, m2, m3, m4

#include "lt.h"

	P(1) = p1
	P(2) = p2
	P(3) = p3
	P(4) = p4
	P(5) = p1p2
	P(6) = p2p3

	M(1) = m1
	if( abs(M(1)) .lt. minmass ) M(1) = 0
	M(2) = m2
	if( abs(M(2)) .lt. minmass ) M(2) = 0
	M(3) = m3
	if( abs(M(3)) .lt. minmass ) M(3) = 0
	M(4) = m4
	if( abs(M(4)) .lt. minmass ) M(4) = 0
	end

************************************************************************

	memindex function XDget(p1, p2, p3, p4, p1p2, p2p3,
     &    m1, m2, m3, m4)
	implicit none
	ArgType p1, p2, p3, p4, p1p2, p2p3
	ArgType m1, m2, m3, m4

#include "lt.h"

	memindex cacheindex
	external cacheindex, XDcoefx

#ifdef COMPLEXPARA
	memindex Dget
	external Dget
#endif

	ArgType para(1,Pdd+3)

#ifdef COMPLEXPARA
	if( abs(Im(p1)) + abs(Im(p2)) +
     &      abs(Im(p3)) + abs(Im(p4)) +
     &      abs(Im(p1p2)) + abs(Im(p2p3)) .gt. 0 )
     &    print *, "Complex momenta not implemented"
	if( abs(Im(m1)) + abs(Im(m2)) +
     &      abs(Im(m3)) + abs(Im(m4)) .eq. 0 ) then
	  XDget = Dget(p1, p2, p3, p4, p1p2, p2p3,
     &      m1, m2, m3, m4) - offsetC
	  return
	endif
#endif

	call XDpara(para, p1, p2, p3, p4, p1p2, p2p3, m1, m2, m3, m4)
	XDget = cacheindex(para, Dval(1,0), XDcoefx, RC*Pdd, Ndd, Dno)
	end

************************************************************************

	subroutine XDput(res, p1, p2, p3, p4, p1p2, p2p3,
     &    m1, m2, m3, m4)
	implicit none
	ComplexType res(*)
	ArgType p1, p2, p3, p4, p1p2, p2p3
	ArgType m1, m2, m3, m4

#include "lt.h"

	external XDcoefx

	ArgType para(1,Pdd+3)

#ifdef COMPLEXPARA
	if( abs(Im(p1)) + abs(Im(p2)) +
     &      abs(Im(p3)) + abs(Im(p4)) +
     &      abs(Im(p1p2)) + abs(Im(p2p3)) .gt. 0 )
     &    print *, "Complex momenta not implemented"
	if( abs(Im(m1)) + abs(Im(m2)) +
     &      abs(Im(m3)) + abs(Im(m4)) .eq. 0 ) then
	  call Dput(res, p1, p2, p3, p4, p1p2, p2p3, m1, m2, m3, m4)
	  return
	endif
#endif

	call XDpara(para, p1, p2, p3, p4, p1p2, p2p3, m1, m2, m3, m4)
	call cachecopy(res, para, Dval(1,0), XDcoefx, RC*Pdd, Ndd, Dno)
	end

************************************************************************

	subroutine XD0nocache(res, p1, p2, p3, p4, p1p2, p2p3,
     &    m1, m2, m3, m4)
	implicit none
	ComplexType res(*)
	ArgType p1, p2, p3, p4, p1p2, p2p3
	ArgType m1, m2, m3, m4

#include "lt.h"

	ArgType para(1,Pdd+3)
	ComplexType res00(0:2)

#ifdef COMPLEXPARA
	if( abs(Im(p1)) + abs(Im(p2)) +
     &      abs(Im(p3)) + abs(Im(p4)) +
     &      abs(Im(p1p2)) + abs(Im(p2p3)) .gt. 0 )
     &    print *, "Complex momenta not implemented"
	if( abs(Im(m1)) + abs(Im(m2)) +
     &      abs(Im(m3)) + abs(Im(m4)) .eq. 0 ) then
	  call D0nocache(res, p1, p2, p3, p4, p1p2, p2p3,
     &      m1, m2, m3, m4)
	  return
	endif
#endif

	call XDpara(para, p1, p2, p3, p4, p1p2, p2p3, m1, m2, m3, m4)
	call XD0func(res, res00, para)
	end

************************************************************************

	ComplexType function XD0i(i, p1, p2, p3, p4, p1p2, p2p3,
     &    m1, m2, m3, m4)
	implicit none
	integer i
	ArgType p1, p2, p3, p4, p1p2, p2p3
	ArgType m1, m2, m3, m4

#include "lt.h"

	memindex XDget
	external XDget

	memindex b

	b = XDget(p1, p2, p3, p4, p1p2, p2p3, m1, m2, m3, m4)
	XD0i = Dval(i+epsi,b)
	end

************************************************************************

	ComplexType function XD0(p1, p2, p3, p4, p1p2, p2p3,
     &    m1, m2, m3, m4)
	implicit none
	ArgType p1, p2, p3, p4, p1p2, p2p3
	ArgType m1, m2, m3, m4

#include "lt.h"

	ComplexType XD0i
	external XD0i

	XD0 = XD0i(dd0, p1, p2, p3, p4, p1p2, p2p3, m1, m2, m3, m4)
	end

************************************************************************

	subroutine XDcoefx(D, para)
	implicit none
	ComplexType D(*)
	ArgType para(1,*)

#include "lt.h"

	memindex c234, c134, c124, c123

	memindex XCget
	external XCget

	c234 = XCget(P(2), P(3), P(6), M(2), M(3), M(4))
	c134 = XCget(P(5), P(3), P(4), M(1), M(3), M(4))
	c124 = XCget(P(1), P(6), P(4), M(1), M(2), M(4))
	c123 = XCget(P(1), P(2), P(5), M(1), M(2), M(3))

	call XDcoeff(D, para,
     &    Cval(1,c234), Cval(1,c134), Cval(1,c124), Cval(1,c123))
	end

************************************************************************

	subroutine XDcoeff(D, para, C234, C134, C124, C123)
	implicit none
	ComplexType D(*), C234(*), C134(*), C124(*), C123(*)
	ArgType para(1,*)

#include "lt.h"

	ArgType p1, p2, p3, p4, p1p2, p2p3, m1, m2, m3, m4
	ArgType f1, f2, f3
	ArgQuad G(3,3)
	ComplexQuad c0sum(0:2), c1sum(0:2), c2sum(0:2), csum(0:2)
	ComplexQuad c00sum(0:2), c11sum(0:2), c12sum(0:2), c22sum(0:2)
	ComplexQuad in(0:2,3)
	logical dump

	integer perm(3)
#define IN(i) in(:,perm(i))
#define OUT(i) in(:,i)
#define SOLVE_SETUP XDecomp(3, G,3, perm)
#define SOLVER(b) XSolve(3, G,3, b)

	integer finite(0:2)
	data finite /1, 0, 0/

	serial = serial + 1
	dump = ibits(debugkey, DebugD, 1) .ne. 0 .and.
     &    serial .ge. debugfrom .and. serial .le. debugto
	if( dump ) call XDumpPara(4, para, "Dcoeff")

	m1 = M(1)
	m2 = M(2)
	m3 = M(3)
	m4 = M(4)
	p1 = P(1)
	p2 = P(2)
	p3 = P(3)
	p4 = P(4)
	p1p2 = P(5)
	p2p3 = P(6)

	f1 = m2
	  f1 = f1 - m1
	  f1 = f1 - p1
	f2 = m3
	  f2 = f2 - m1
	  f2 = f2 - p1p2
	f3 = m4
	  f3 = f3 - m1
	  f3 = f3 - p4

	G(1,1) = 2*p1
	G(2,2) = 2*p1p2
	G(3,3) = 2*p4
	G(1,2) = p1
	  G(1,2) = G(1,2) + p1p2
	  G(1,2) = G(1,2) - p2
	G(2,1) = G(1,2)
	G(1,3) = p1
	  G(1,3) = G(1,3) + p4
	  G(1,3) = G(1,3) - p2p3
	G(3,1) = G(1,3)
	G(2,3) = p1p2
	  G(2,3) = G(2,3) - p3
	  G(2,3) = G(2,3) + p4
	G(3,2) = G(2,3)
	call SOLVE_SETUP

	c0sum = C234(CC0) + C234(CC1) + C234(CC2)
	c1sum = C234(CC1) + C234(CC11) + C234(CC12)
	c2sum = C234(CC2) + C234(CC12) + C234(CC22)
	csum = c0sum + c1sum + c2sum

	c00sum = C234(CC00) + C234(CC001) + C234(CC002)
	c11sum = C234(CC11) + C234(CC111) + C234(CC112)
	c12sum = C234(CC12) + C234(CC112) + C234(CC122)
	c22sum = C234(CC22) + C234(CC122) + C234(CC222)

	call XD0func(D(DD0), D(DD00), para)

	IN(1) = f1*D(DD0) - C234(CC0) + C134(CC0)
	IN(2) = f2*D(DD0) - C234(CC0) + C124(CC0)
	IN(3) = f3*D(DD0) - C234(CC0) + C123(CC0)
	call SOLVER(in)
	D(DD1) = OUT(1)
	D(DD2) = OUT(2)
	D(DD3) = OUT(3)

	if( Re(D(dd00)) .eq. -999 ) D(DD00) = m1*D(DD0) -
     &    .5D0*(D(DD1)*f1 + D(DD2)*f2 + D(DD3)*f3 - C234(CC0))

	IN(1) = f1*D(DD1) + c0sum - 2*D(DD00)
	IN(2) = f2*D(DD1) + c0sum + C124(CC1)
	IN(3) = f3*D(DD1) + c0sum + C123(CC1)
	call SOLVER(in)
	D(DD11) = OUT(1)
	D(DD12) = OUT(2)
	D(DD13) = OUT(3)

	IN(1) = f1*D(DD2) - C234(CC1) + C134(CC1)
	IN(2) = f2*D(DD2) - C234(CC1) - 2*D(DD00)
	IN(3) = f3*D(DD2) - C234(CC1) + C123(CC2)
	call SOLVER(in)
	D(DD12) = .5D0*(D(DD12) + OUT(1))
	D(DD22) = OUT(2)
	D(DD23) = OUT(3)

	IN(1) = f1*D(DD3) - C234(CC2) + C134(CC2)
	IN(2) = f2*D(DD3) - C234(CC2) + C124(CC2)
	IN(3) = f3*D(DD3) - C234(CC2) - 2*D(DD00)
	call SOLVER(in)
	D(DD13) = .5D0*(D(DD13) + OUT(1))
	D(DD23) = .5D0*(D(DD23) + OUT(2))
	D(DD33) = OUT(3)

	IN(1) = f1*D(DD00) - C234(CC00) + C134(CC00)
	IN(2) = f2*D(DD00) - C234(CC00) + C124(CC00)
	IN(3) = f3*D(DD00) - C234(CC00) + C123(CC00)
	call SOLVER(in)
	D(DD001) = OUT(1)
	D(DD002) = OUT(2)
	D(DD003) = OUT(3)

	IN(1) = f1*D(DD11) - csum - 4*D(DD001)
	IN(2) = f2*D(DD11) - csum + C124(CC11)
	IN(3) = f3*D(DD11) - csum + C123(CC11)
	call SOLVER(in)
	D(DD111) = OUT(1)
	D(DD112) = OUT(2)
	D(DD113) = OUT(3)

	IN(1) = f1*D(DD22) - C234(CC11) + C134(CC11)
	IN(2) = f2*D(DD22) - C234(CC11) - 4*D(DD002)
	IN(3) = f3*D(DD22) - C234(CC11) + C123(CC22)
	call SOLVER(in)
	D(DD122) = OUT(1)
	D(DD222) = OUT(2)
	D(DD223) = OUT(3)

	IN(1) = f1*D(DD33) - C234(CC22) + C134(CC22)
	IN(2) = f2*D(DD33) - C234(CC22) + C124(CC22)
	IN(3) = f3*D(DD33) - C234(CC22) - 4*D(DD003)
	call SOLVER(in)
	D(DD133) = OUT(1)
	D(DD233) = OUT(2)
	D(DD333) = OUT(3)

	IN(1) = f1*D(DD13) + c2sum - 2*D(DD003)
	IN(2) = f2*D(DD13) + c2sum + C124(CC12)
	IN(3) = f3*D(DD13) + c2sum - 2*D(DD001)
	call SOLVER(in)
	D(DD113) = .5D0*(D(DD113) + OUT(1))
	D(DD123) = OUT(2)
	D(DD133) = .5D0*(D(DD133) + OUT(3))

	D(DD0000) = 1/3D0*(m1*D(DD00) -
     &    .5D0*(f1*D(DD001) + f2*D(DD002) + f3*D(DD003) -
     &      C234(CC00) - finite/6D0))
	D(DD0011) = 1/3D0*(m1*D(DD11) -
     &    .5D0*(f1*D(DD111) + f2*D(DD112) + f3*D(DD113) - csum))
	D(DD0012) = 1/3D0*(m1*D(DD12) -
     &    .5D0*(f1*D(DD112) + f2*D(DD122) + f3*D(DD123) + c1sum))
	D(DD0013) = 1/3D0*(m1*D(DD13) -
     &    .5D0*(f1*D(DD113) + f2*D(DD123) + f3*D(DD133) + c2sum))
	D(DD0022) = 1/3D0*(m1*D(DD22) -
     &    .5D0*(f1*D(DD122) + f2*D(DD222) + f3*D(DD223) -
     &      C234(CC11)))
	D(DD0023) = 1/3D0*(m1*D(DD23) -
     &    .5D0*(f1*D(DD123) + f2*D(DD223) + f3*D(DD233) -
     &      C234(CC12)))
	D(DD0033) = 1/3D0*(m1*D(DD33) -
     &    .5D0*(f1*D(DD133) + f2*D(DD233) + f3*D(DD333) -
     &      C234(CC22)))

	c1sum = c1sum + c11sum + c12sum
	c2sum = c2sum + c12sum + c22sum
	csum = csum + c1sum + c2sum

	IN(1) = f1*D(DD111) + csum - 6*D(DD0011)
	IN(2) = f2*D(DD111) + csum + C124(CC111)
	IN(3) = f3*D(DD111) + csum + C123(CC111)
	call SOLVER(in)
	D(DD1111) = OUT(1)
	D(DD1112) = OUT(2)
	D(DD1113) = OUT(3)

	IN(1) = f1*D(DD113) - c2sum - 4*D(DD0013)
	IN(2) = f2*D(DD113) - c2sum + C124(CC112)
	IN(3) = f3*D(DD113) - c2sum - 2*D(DD0011)
	call SOLVER(in)
	D(DD1113) = .5D0*(D(DD1113) + OUT(1))
	D(DD1123) = OUT(2)
	D(DD1133) = OUT(3)

	IN(1) = f1*D(DD122) + c11sum - 2*D(DD0022)
	IN(2) = f2*D(DD122) + c11sum - 4*D(DD0012)
	IN(3) = f3*D(DD122) + c11sum + C123(CC122)
	call SOLVER(in)
	D(DD1122) = OUT(1)
	D(DD1222) = OUT(2)
	D(DD1223) = OUT(3)

	IN(1) = f1*D(DD222) - C234(CC111) + C134(CC111)
	IN(2) = f2*D(DD222) - C234(CC111) - 6*D(DD0022)
	IN(3) = f3*D(DD222) - C234(CC111) + C123(CC222)
	call SOLVER(in)
	D(DD1222) = .5D0*(D(DD1222) + OUT(1))
	D(DD2222) = OUT(2)
	D(DD2223) = OUT(3)

	IN(1) = f1*D(DD233) - C234(CC122) + C134(CC122)
	IN(2) = f2*D(DD233) - C234(CC122) - 2*D(DD0033)
	IN(3) = f3*D(DD233) - C234(CC122) - 4*D(DD0023)
	call SOLVER(in)
	D(DD1233) = OUT(1)
	D(DD2233) = OUT(2)
	D(DD2333) = OUT(3)

	IN(1) = f1*D(DD333) - C234(CC222) + C134(CC222)
	IN(2) = f2*D(DD333) - C234(CC222) + C124(CC222)
	IN(3) = f3*D(DD333) - C234(CC222) - 6*D(DD0033)
	call SOLVER(in)
	D(DD1333) = OUT(1)
	D(DD2333) = .5D0*(D(DD2333) + OUT(2))
	D(DD3333) = OUT(3)

	c00sum = c00sum +
     &    C234(CC001) + C234(CC0011) + C234(CC0012) +
     &    C234(CC002) + C234(CC0012) + C234(CC0022)
	c11sum = c11sum +
     &    C234(CC111) + C234(CC1111) + C234(CC1112) +
     &    C234(CC112) + C234(CC1112) + C234(CC1122)
	c12sum = c12sum +
     &    C234(CC112) + C234(CC1112) + C234(CC1122) +
     &    C234(CC122) + C234(CC1122) + C234(CC1222)
	c22sum = c22sum +
     &    C234(CC122) + C234(CC1122) + C234(CC1222) +
     &    C234(CC222) + C234(CC1222) + C234(CC2222)
	c1sum = c1sum + c11sum + c12sum
	c2sum = c2sum + c12sum + c22sum
	csum = csum + c1sum + c2sum

	IN(1) = f1*D(DD0000) - C234(CC0000) + C134(CC0000)
	IN(2) = f2*D(DD0000) - C234(CC0000) + C124(CC0000)
	IN(3) = f3*D(DD0000) - C234(CC0000) + C123(CC0000)
	call SOLVER(in)
	D(DD00001) = OUT(1)
	D(DD00002) = OUT(2)
	D(DD00003) = OUT(3)

	IN(1) = f1*D(DD0011) - c00sum - 4*D(DD00001)
	IN(2) = f2*D(DD0011) - c00sum + C124(CC0011)
	IN(3) = f3*D(DD0011) - c00sum + C123(CC0011)
	call SOLVER(in)
	D(DD00111) = OUT(1)
	D(DD00112) = OUT(2)
	D(DD00113) = OUT(3)

	IN(1) = f1*D(DD0022) - C234(CC0011) + C134(CC0011)
	IN(2) = f2*D(DD0022) - C234(CC0011) - 4*D(DD00002)
	IN(3) = f3*D(DD0022) - C234(CC0011) + C123(CC0022)
	call SOLVER(in)
	D(DD00122) = OUT(1)
	D(DD00222) = OUT(2)
	D(DD00223) = OUT(3)

	IN(1) = f1*D(DD0033) - C234(CC0022) + C134(CC0022)
	IN(2) = f2*D(DD0033) - C234(CC0022) + C124(CC0022)
	IN(3) = f3*D(DD0033) - C234(CC0022) - 4*D(DD00003)
	call SOLVER(in)
	D(DD00133) = OUT(1)
	D(DD00233) = OUT(2)
	D(DD00333) = OUT(3)

	IN(1) = f1*D(DD0023) - C234(CC0012) + C134(CC0012)
	IN(2) = f2*D(DD0023) - C234(CC0012) - 2*D(DD00003)
	IN(3) = f3*D(DD0023) - C234(CC0012) - 2*D(DD00002)
	call SOLVER(in)
	D(DD00123) = OUT(1)
	D(DD00223) = .5D0*(D(DD00223) + OUT(2))
	D(DD00233) = .5D0*(D(DD00233) + OUT(3))

	IN(1) = f1*D(DD1111) - csum - 8*D(DD00111)
	IN(2) = f2*D(DD1111) - csum + C124(CC1111)
	IN(3) = f3*D(DD1111) - csum + C123(CC1111)
	call SOLVER(in)
	D(DD11111) = OUT(1)
	D(DD11112) = OUT(2)
	D(DD11113) = OUT(3)

	IN(1) = f1*D(DD2222) - C234(CC1111) + C134(CC1111)
	IN(2) = f2*D(DD2222) - C234(CC1111) - 8*D(DD00222)
	IN(3) = f3*D(DD2222) - C234(CC1111) + C123(CC2222)
	call SOLVER(in)
	D(DD12222) = OUT(1)
	D(DD22222) = OUT(2)
	D(DD22223) = OUT(3)

	IN(1) = f1*D(DD3333) - C234(CC2222) + C134(CC2222)
	IN(2) = f2*D(DD3333) - C234(CC2222) + C124(CC2222)
	IN(3) = f3*D(DD3333) - C234(CC2222) - 8*D(DD00333)
	call SOLVER(in)
	D(DD13333) = OUT(1)
	D(DD23333) = OUT(2)
	D(DD33333) = OUT(3)

	IN(1) = f1*D(DD1122) - c11sum - 4*D(DD00122)
	IN(2) = f2*D(DD1122) - c11sum - 4*D(DD00112)
	IN(3) = f3*D(DD1122) - c11sum + C123(CC1122)
	call SOLVER(in)
	D(DD11122) = OUT(1)
	D(DD11222) = OUT(2)
	D(DD11223) = OUT(3)

	IN(1) = f1*D(DD1133) - c22sum - 4*D(DD00133)
	IN(2) = f2*D(DD1133) - c22sum + C124(CC1122)
	IN(3) = f3*D(DD1133) - c22sum - 4*D(DD00113)
	call SOLVER(in)
	D(DD11133) = OUT(1)
	D(DD11233) = OUT(2)
	D(DD11333) = OUT(3)

	IN(1) = f1*D(DD2233) - C234(CC1122) + C134(CC1122)
	IN(2) = f2*D(DD2233) - C234(CC1122) - 4*D(DD00233)
	IN(3) = f3*D(DD2233) - C234(CC1122) - 4*D(DD00223)
	call SOLVER(in)
	D(DD12233) = OUT(1)
	D(DD22233) = OUT(2)
	D(DD22333) = OUT(3)

	IN(1) = f1*D(DD1123) - c12sum - 4*D(DD00123)
	IN(2) = f2*D(DD1123) - c12sum - 2*D(DD00113)
	IN(3) = f3*D(DD1123) - c12sum - 2*D(DD00112)
	call SOLVER(in)
	D(DD11123) = OUT(1)
	D(DD11223) = .5D0*(D(DD11223) + OUT(2))
	D(DD11233) = .5D0*(D(DD11233) + OUT(3))

	IN(1) = f1*D(DD2223) - C234(CC1112) + C134(CC1112)
	IN(2) = f2*D(DD2223) - C234(CC1112) - 6*D(DD00223)
	IN(3) = f3*D(DD2223) - C234(CC1112) - 2*D(DD00222)
	call SOLVER(in)
	D(DD12223) = OUT(1)
	D(DD22223) = .5D0*(D(DD22223) + OUT(2))
	D(DD22233) = .5D0*(D(DD22233) + OUT(3))

	IN(1) = f1*D(DD2333) - C234(CC1222) + C134(CC1222)
	IN(2) = f2*D(DD2333) - C234(CC1222) - 2*D(DD00333)
	IN(3) = f3*D(DD2333) - C234(CC1222) - 6*D(DD00233)
	call SOLVER(in)
	D(DD12333) = OUT(1)
	D(DD22333) = .5D0*(D(DD22333) + OUT(2))
	D(DD23333) = .5D0*(D(DD23333) + OUT(3))

	if( dump ) call XDumpCoeff(4, D)
	end

