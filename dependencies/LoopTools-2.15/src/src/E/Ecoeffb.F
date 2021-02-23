* Ecoeffb.F
* the five-point tensor coefficients via Passarino-Veltman decomposition
* this file is part of LoopTools
* written by M. Rauch
* last modified 9 Apr 18 th

#include "externals.h"
#include "types.h"

#define npoint 5
#include "defs.h"


	subroutine XEcoeffb(E, para, D2345, D1345, D1245, D1235, D1234)
	implicit none
	ComplexType E(*)
	ComplexType D2345(*), D1345(*), D1245(*), D1235(*), D1234(*)
	ArgType para(1,*)

#include "lt.h"

	memindex XDget
	external XDget

	ArgType p1, p2, p3, p4, p5
	ArgType p1p2, p2p3, p3p4, p4p5, p5p1
	ArgType m1, m2, m3, m4, m5

	ArgQuad f1, f2, f3, f4
	ComplexQuad di, d0sum
	ComplexQuad d1i, d1sum, d2i, d2sum, dii
	ComplexQuad d1ii, d2ii, diii
	ComplexQuad d00sum, d22sum, d33sum
	ComplexQuad in(4)
	logical dump

	ArgQuad G(4,4), Ginv(4,4)
	common /XInvGramE/ Ginv

	integer perm(4)
#define IN(i) in(perm(i))
#define OUT(i) in(i)
#define SOLVE_SETUP XInverse(4, G,4, Ginv,4, perm)
#define SOLVER(b) XSolve(4, G,4, b)

	serial = serial + 1
	dump = ibits(debugkey, DebugE, 1) .ne. 0 .and.
     &    serial .ge. debugfrom .and. serial .le. debugto
	if( dump ) call XDumpPara(5, para)

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

	f1 = m2
	  f1 = f1 - m1
	  f1 = f1 - p1
	f2 = m3
	  f2 = f2 - m1
	  f2 = f2 - p1p2
	f3 = m4
	  f3 = f3 - m1
	  f3 = f3 - p4p5
	f4 = m5
	  f4 = f4 - m1
	  f4 = f4 - p5

* build up G and calculate matrix decomposition and inverse Y
	G(1,1) = 2*p1
	G(2,2) = 2*p1p2
	G(3,3) = 2*p4p5
	G(4,4) = 2*p5
	G(1,2) = p1
	  G(1,2) = G(1,2) + p1p2
	  G(1,2) = G(1,2) - p2
	G(2,1) = G(1,2)
	G(1,3) = p1
	  G(1,3) = G(1,3) - p2p3
	  G(1,3) = G(1,3) + p4p5
	G(3,1) = G(1,3)
	G(1,4) = p1
	  G(1,4) = G(1,4) - p5p1
	  G(1,4) = G(1,4) + p5
	G(4,1) = G(1,4)
	G(2,3) = p1p2
	  G(2,3) = G(2,3) - p3
	  G(2,3) = G(2,3) + p4p5
	G(3,2) = G(2,3)
	G(2,4) = p1p2
	  G(2,4) = G(2,4) - p3p4
	  G(2,4) = G(2,4) + p5
	G(4,2) = G(2,4)
	G(3,4) = p5
	  G(3,4) = G(3,4) + p4p5
	  G(3,4) = G(3,4) - p4
	G(4,3) = G(3,4)
	call SOLVE_SETUP

	di = D2345(dd1) + D2345(dd2) + D2345(dd3)
	d0sum = di + D2345(dd0)

	d1i = D2345(dd11) + D2345(dd12) + D2345(dd13)
	d1sum = d1i + D2345(dd1)
	d2i = D2345(dd12) + D2345(dd22) + D2345(dd23)
	d2sum = d2i + D2345(dd2)
	dii = d1i + d2i + D2345(dd13) + D2345(dd23) + D2345(dd33)

	d1ii = D2345(dd111) + D2345(dd122) +
     &    D2345(dd133) + 2*(D2345(dd112) +
     &    D2345(dd113) + D2345(dd123))
	d2ii = D2345(dd112) + D2345(dd222) +
     &    D2345(dd233) + 2*(D2345(dd122) +
     &    D2345(dd123) + D2345(dd223))
	diii = d1ii + d2ii +
     &    D2345(dd113) + D2345(dd223) +
     &    D2345(dd333) + 2*(D2345(dd123) +
     &    D2345(dd133) + D2345(dd233))

	d00sum = D2345(dd00) +
     &    D2345(dd001) + D2345(dd002) + D2345(dd003)
	d22sum = D2345(dd22) +
     &    D2345(dd122) + D2345(dd222) + D2345(dd223)
	d33sum = D2345(dd33) +
     &    D2345(dd133) + D2345(dd233) + D2345(dd333)

	call XE0func(E(ee0), para,
     &    D2345, D1345, D1245, D1235, D1234, 1)

	IN(1) = f1*E(ee0) - D2345(dd0) + D1345(dd0)
	IN(2) = f2*E(ee0) - D2345(dd0) + D1245(dd0)
	IN(3) = f3*E(ee0) - D2345(dd0) + D1235(dd0)
	IN(4) = f4*E(ee0) - D2345(dd0) + D1234(dd0)
	call SOLVER(in)
	E(ee1) = OUT(1)
	E(ee2) = OUT(2)
	E(ee3) = OUT(3)
	E(ee4) = OUT(4)

	E(ee00) = 0

	IN(1) = f1*E(ee1) + d0sum
	IN(2) = f2*E(ee1) + d0sum + D1245(dd1)
	IN(3) = f3*E(ee1) + d0sum + D1235(dd1)
	IN(4) = f4*E(ee1) + d0sum + D1234(dd1)
	call SOLVER(in)
	E(ee11) = OUT(1)
	E(ee12) = OUT(2)
	E(ee13) = OUT(3)
	E(ee14) = OUT(4)

	IN(1) = f1*E(ee2) - D2345(dd1) + D1345(dd1)
	IN(2) = f2*E(ee2) - D2345(dd1)
	IN(3) = f3*E(ee2) - D2345(dd1) + D1235(dd2)
	IN(4) = f4*E(ee2) - D2345(dd1) + D1234(dd2)
	call SOLVER(in)
	E(ee12) = .5D0*(E(ee12) + OUT(1))
	E(ee22) = OUT(2)
	E(ee23) = OUT(3)
	E(ee24) = OUT(4)

	IN(1) = f1*E(ee3) - D2345(dd2) + D1345(dd2)
	IN(2) = f2*E(ee3) - D2345(dd2) + D1245(dd2)
	IN(3) = f3*E(ee3) - D2345(dd2)
	IN(4) = f4*E(ee3) - D2345(dd2) + D1234(dd3)
	call SOLVER(in)
	E(ee13) = .5D0*(E(ee13) + OUT(1))
	E(ee23) = .5D0*(E(ee23) + OUT(2))
	E(ee33) = OUT(3)
	E(ee34) = OUT(4)

	IN(1) = f1*E(ee4) - D2345(dd3) + D1345(dd3)
	IN(2) = f2*E(ee4) - D2345(dd3) + D1245(dd3)
	IN(3) = f3*E(ee4) - D2345(dd3) + D1235(dd3)
	IN(4) = f4*E(ee4) - D2345(dd3)
	call SOLVER(in)
	E(ee14) = .5D0*(E(ee14) + OUT(1))
	E(ee24) = .5D0*(E(ee24) + OUT(2))
	E(ee34) = .5D0*(E(ee34) + OUT(3))
	E(ee44) = OUT(4)

	E(ee001) = 0
	E(ee002) = 0
	E(ee003) = 0
	E(ee004) = 0

	d0sum = d0sum + di + dii
	IN(1) = f1*E(ee11) - d0sum -
     &    2*Ginv(1,1)*(D2345(dd00) - D1345(dd00))
	IN(2) = f2*E(ee11) - d0sum + D1245(dd11) -
     &    2*Ginv(1,1)*(D2345(dd00) - D1245(dd00))
	IN(3) = f3*E(ee11) - d0sum + D1235(dd11) -
     &    2*Ginv(1,1)*(D2345(dd00) - D1235(dd00))
	IN(4) = f4*E(ee11) - d0sum + D1234(dd11) -
     &    2*Ginv(1,1)*(D2345(dd00) - D1234(dd00))
	call SOLVER(in)
	E(ee111) = OUT(1)
	E(ee112) = OUT(2)
	E(ee113) = OUT(3)
	E(ee114) = OUT(4)

	IN(1) = f1*E(ee22) - D2345(dd11) + D1345(dd11) -
     &    2*Ginv(2,2)*(D2345(dd00) - D1345(dd00))
	IN(2) = f2*E(ee22) - D2345(dd11) -
     &    2*Ginv(2,2)*(D2345(dd00) - D1245(dd00))
	IN(3) = f3*E(ee22) - D2345(dd11) + D1235(dd22) -
     &    2*Ginv(2,2)*(D2345(dd00) - D1235(dd00))
	IN(4) = f4*E(ee22) - D2345(dd11) + D1234(dd22) -
     &    2*Ginv(2,2)*(D2345(dd00) - D1234(dd00))
	call SOLVER(in)
	E(ee122) = OUT(1)
	E(ee222) = OUT(2)
	E(ee223) = OUT(3)
	E(ee224) = OUT(4)

	IN(1) = f1*E(ee33) - D2345(dd22) + D1345(dd22) -
     &    2*Ginv(3,3)*(D2345(dd00) - D1345(dd00))
	IN(2) = f2*E(ee33) - D2345(dd22) + D1245(dd22) -
     &    2*Ginv(3,3)*(D2345(dd00) - D1245(dd00))
	IN(3) = f3*E(ee33) - D2345(dd22) -
     &    2*Ginv(3,3)*(D2345(dd00) - D1235(dd00))
	IN(4) = f4*E(ee33) - D2345(dd22) + D1234(dd33) -
     &    2*Ginv(3,3)*(D2345(dd00) - D1234(dd00))
	call SOLVER(in)
	E(ee133) = OUT(1)
	E(ee233) = OUT(2)
	E(ee333) = OUT(3)
	E(ee334) = OUT(4)

	IN(1) = f1*E(ee44) - D2345(dd33) + D1345(dd33) -
     &    2*Ginv(4,4)*(D2345(dd00) - D1345(dd00))
	IN(2) = f2*E(ee44) - D2345(dd33) + D1245(dd33) -
     &    2*Ginv(4,4)*(D2345(dd00) - D1245(dd00))
	IN(3) = f3*E(ee44) - D2345(dd33) + D1235(dd33) -
     &    2*Ginv(4,4)*(D2345(dd00) - D1235(dd00))
	IN(4) = f4*E(ee44) - D2345(dd33) -
     &    2*Ginv(4,4)*(D2345(dd00) - D1234(dd00))
	call SOLVER(in)
	E(ee144) = OUT(1)
	E(ee244) = OUT(2)
	E(ee344) = OUT(3)
	E(ee444) = OUT(4)

	IN(1) = f1*E(ee12) + d1sum -
     &    2*Ginv(1,2)*(D2345(dd00) - D1345(dd00))
	IN(2) = f2*E(ee12) + d1sum -
     &    2*Ginv(1,2)*(D2345(dd00) - D1245(dd00))
	IN(3) = f3*E(ee12) + d1sum + D1235(dd12) -
     &    2*Ginv(1,2)*(D2345(dd00) - D1235(dd00))
	IN(4) = f4*E(ee12) + d1sum + D1234(dd12) -
     &    2*Ginv(1,2)*(D2345(dd00) - D1234(dd00))
	call SOLVER(in)
	E(ee112) = .5D0*(E(ee112) + OUT(1))
	E(ee122) = .5D0*(E(ee122) + OUT(2))
	E(ee123) = OUT(3)
	E(ee124) = OUT(4)

	IN(1) = f1*E(ee34) - D2345(dd23) + D1345(dd23) -
     &    2*Ginv(3,4)*(D2345(dd00) - D1345(dd00))
	IN(2) = f2*E(ee34) - D2345(dd23) + D1245(dd23) -
     &    2*Ginv(3,4)*(D2345(dd00) - D1245(dd00))
	IN(3) = f3*E(ee34) - D2345(dd23) -
     &    2*Ginv(3,4)*(D2345(dd00) - D1235(dd00))
	IN(4) = f4*E(ee34) - D2345(dd23) -
     &    2*Ginv(3,4)*(D2345(dd00) - D1234(dd00))
	call SOLVER(in)
	E(ee134) = OUT(1)
	E(ee234) = OUT(2)
	E(ee334) = .5D0*(E(ee334) + OUT(3))
	E(ee344) = .5D0*(E(ee344) + OUT(4))

	E(ee0000) = 0
	E(ee0011) = 0
	E(ee0012) = 0
	E(ee0013) = 0
	E(ee0014) = 0
	E(ee0022) = 0
	E(ee0023) = 0
	E(ee0024) = 0
	E(ee0033) = 0
	E(ee0034) = 0
	E(ee0044) = 0

	d0sum = d0sum + di + 2*dii + diii
	IN(1) = f1*E(ee111) + d0sum +
     &    6*Ginv(1,1)*d00sum
	IN(2) = f2*E(ee111) + d0sum + D1245(dd111) +
     &    6*Ginv(1,1)*(d00sum + D1245(dd001))
	IN(3) = f3*E(ee111) + d0sum + D1235(dd111) +
     &    6*Ginv(1,1)*(d00sum + D1235(dd001))
	IN(4) = f4*E(ee111) + d0sum + D1234(dd111) +
     &    6*Ginv(1,1)*(d00sum + D1234(dd001))
	call SOLVER(in)
	E(ee1111) = OUT(1)
	E(ee1112) = OUT(2)
	E(ee1113) = OUT(3)
	E(ee1114) = OUT(4)

	IN(1) = f1*E(ee222) - D2345(dd111) + D1345(dd111) -
     &    6*Ginv(2,2)*(D2345(dd001) - D1345(dd001))
	IN(2) = f2*E(ee222) - D2345(dd111) -
     &    6*Ginv(2,2)*D2345(dd001)
	IN(3) = f3*E(ee222) - D2345(dd111) + D1235(dd222) -
     &    6*Ginv(2,2)*(D2345(dd001) - D1235(dd002))
	IN(4) = f4*E(ee222) - D2345(dd111) + D1234(dd222) -
     &    6*Ginv(2,2)*(D2345(dd001) - D1234(dd002))
	call SOLVER(in)
	E(ee1222) = OUT(1)
	E(ee2222) = OUT(2)
	E(ee2223) = OUT(3)
	E(ee2224) = OUT(4)

	IN(1) = f1*E(ee333) - D2345(dd222) + D1345(dd222) -
     &    6*Ginv(3,3)*(D2345(dd002) - D1345(dd002))
	IN(2) = f2*E(ee333) - D2345(dd222) + D1245(dd222) -
     &    6*Ginv(3,3)*(D2345(dd002) - D1245(dd002))
	IN(3) = f3*E(ee333) - D2345(dd222) -
     &    6*Ginv(3,3)*D2345(dd002)
	IN(4) = f4*E(ee333) - D2345(dd222) + D1234(dd333) -
     &    6*Ginv(3,3)*(D2345(dd002) - D1234(dd003))
	call SOLVER(in)
	E(ee1333) = OUT(1)
	E(ee2333) = OUT(2)
	E(ee3333) = OUT(3)
	E(ee3334) = OUT(4)

	IN(1) = f1*E(ee444) - D2345(dd333) + D1345(dd333) -
     &    6*Ginv(4,4)*(D2345(dd003) - D1345(dd003))
	IN(2) = f2*E(ee444) - D2345(dd333) + D1245(dd333) -
     &    6*Ginv(4,4)*(D2345(dd003) - D1245(dd003))
	IN(3) = f3*E(ee444) - D2345(dd333) + D1235(dd333) -
     &    6*Ginv(4,4)*(D2345(dd003) - D1235(dd003))
	IN(4) = f4*E(ee444) - D2345(dd333) -
     &    6*Ginv(4,4)*D2345(dd003)
	call SOLVER(in)
	E(ee1444) = OUT(1)
	E(ee2444) = OUT(2)
	E(ee3444) = OUT(3)
	E(ee4444) = OUT(4)

	d1sum = d1sum + d1i + d1ii
	IN(1) = f1*E(ee112) - d1sum -
     &    2*Ginv(1,1)*(D2345(dd001) - D1345(dd001)) +
     &    4*Ginv(1,2)*d00sum
	IN(2) = f2*E(ee112) - d1sum -
     &    2*Ginv(1,1)*D2345(dd001) +
     &    4*Ginv(1,2)*(d00sum + D1245(dd001))
	IN(3) = f3*E(ee112) - d1sum + D1235(dd112) -
     &    2*Ginv(1,1)*(D2345(dd001) - D1235(dd002)) +
     &    4*Ginv(1,2)*(d00sum + D1235(dd001))
	IN(4) = f4*E(ee112) - d1sum + D1234(dd112) -
     &    2*Ginv(1,1)*(D2345(dd001) - D1234(dd002)) +
     &    4*Ginv(1,2)*(d00sum + D1234(dd001))
	call SOLVER(in)
	E(ee1112) = .5D0*(E(ee1112) + OUT(1))
	E(ee1122) = OUT(2)
	E(ee1123) = OUT(3)
	E(ee1124) = OUT(4)

	IN(1) = f1*E(ee223) - D2345(dd112) + D1345(dd112) -
     &    2*Ginv(2,2)*(D2345(dd002) - D1345(dd002)) -
     &    4*Ginv(2,3)*(D2345(dd001) - D1345(dd001))
	IN(2) = f2*E(ee223) - D2345(dd112) -
     &    2*Ginv(2,2)*(D2345(dd002) - D1245(dd002)) -
     &    4*Ginv(2,3)*D2345(dd001)
	IN(3) = f3*E(ee223) - D2345(dd112) -
     &    2*Ginv(2,2)*D2345(dd002) -
     &    4*Ginv(2,3)*(D2345(dd001) - D1235(dd002))
	IN(4) = f4*E(ee223) - D2345(dd112) + D1234(dd223) -
     &    2*Ginv(2,2)*(D2345(dd002) - D1234(dd003)) -
     &    4*Ginv(2,3)*(D2345(dd001) - D1234(dd002))
	call SOLVER(in)
	E(ee1223) = OUT(1)
	E(ee2223) = .5D0*(E(ee2223) + OUT(2))
	E(ee2233) = OUT(3)
	E(ee2234) = OUT(4)

	IN(1) = f1*E(ee334) - D2345(dd223) + D1345(dd223) -
     &    2*Ginv(3,3)*(D2345(dd003) - D1345(dd003)) -
     &    4*Ginv(3,4)*(D2345(dd002) - D1345(dd002))
	IN(2) = f2*E(ee334) - D2345(dd223) + D1245(dd223) -
     &    2*Ginv(3,3)*(D2345(dd003) - D1245(dd003)) -
     &    4*Ginv(3,4)*(D2345(dd002) - D1245(dd002))
	IN(3) = f3*E(ee334) - D2345(dd223) -
     &    2*Ginv(3,3)*(D2345(dd003) - D1235(dd003)) -
     &    4*Ginv(3,4)*D2345(dd002)
	IN(4) = f4*E(ee334) - D2345(dd223) -
     &    2*Ginv(3,3)*D2345(dd003) -
     &    4*Ginv(3,4)*(D2345(dd002) - D1234(dd003))
	call SOLVER(in)
	E(ee1334) = OUT(1)
	E(ee2334) = OUT(2)
	E(ee3334) = .5D0*(E(ee3334) + OUT(3))
	E(ee3344) = OUT(4)

	IN(1) = f1*E(ee144) + d33sum -
     &    4*Ginv(1,4)*(D2345(dd003) - D1345(dd003)) +
     &    2*Ginv(4,4)*d00sum
	IN(2) = f2*E(ee144) + d33sum + D1245(dd133) -
     &    4*Ginv(1,4)*(D2345(dd003) - D1245(dd003)) +
     &    2*Ginv(4,4)*(d00sum + D1245(dd001))
	IN(3) = f3*E(ee144) + d33sum + D1235(dd133) -
     &    4*Ginv(1,4)*(D2345(dd003) - D1235(dd003)) +
     &    2*Ginv(4,4)*(d00sum + D1235(dd001))
	IN(4) = f4*E(ee144) + d33sum -
     &    4*Ginv(1,4)*D2345(dd003) +
     &    2*Ginv(4,4)*(d00sum + D1234(dd001))
	call SOLVER(in)
	E(ee1144) = OUT(1)
	E(ee1244) = OUT(2)
	E(ee1344) = OUT(3)
	E(ee1444) = .5D0*(E(ee1444) + OUT(4))

	d2sum = d2sum + d2i + d2ii
	IN(1) = f1*E(ee113) - d2sum -
     &    2*Ginv(1,1)*(D2345(dd002) - D1345(dd002)) +
     &    4*Ginv(1,3)*d00sum
	IN(2) = f2*E(ee113) - d2sum + D1245(dd112) -
     &    2*Ginv(1,1)*(D2345(dd002) - D1245(dd002)) +
     &    4*Ginv(1,3)*(d00sum + D1245(dd001))
	IN(3) = f3*E(ee113) - d2sum -
     &    2*Ginv(1,1)*D2345(dd002) +
     &    4*Ginv(1,3)*(d00sum + D1235(dd001))
	IN(4) = f4*E(ee113) - d2sum + D1234(dd113) -
     &    2*Ginv(1,1)*(D2345(dd002) - D1234(dd003)) +
     &    4*Ginv(1,3)*(d00sum + D1234(dd001))
	call SOLVER(in)
	E(ee1113) = .5D0*(E(ee1113) + OUT(1))
	E(ee1123) = .5D0*(E(ee1123) + OUT(2))
	E(ee1133) = OUT(3)
	E(ee1134) = OUT(4)

	IN(1) = f1*E(ee224) - D2345(dd113) + D1345(dd113) -
     &    2*Ginv(2,2)*(D2345(dd003) - D1345(dd003)) -
     &    4*Ginv(2,4)*(D2345(dd001) - D1345(dd001))
	IN(2) = f2*E(ee224) - D2345(dd113) -
     &    2*Ginv(2,2)*(D2345(dd003) - D1245(dd003)) -
     &    4*Ginv(2,4)*D2345(dd001)
	IN(3) = f3*E(ee224) - D2345(dd113) + D1235(dd223) -
     &    2*Ginv(2,2)*(D2345(dd003) - D1235(dd003)) -
     &    4*Ginv(2,4)*(D2345(dd001) - D1235(dd002))
	IN(4) = f4*E(ee224) - D2345(dd113) -
     &    2*Ginv(2,2)*D2345(dd003) -
     &    4*Ginv(2,4)*(D2345(dd001) - D1234(dd002))
	call SOLVER(in)
	E(ee1224) = OUT(1)
	E(ee2224) = .5D0*(E(ee2224) + OUT(2))
	E(ee2234) = E(ee2234) + OUT(3)
	E(ee2244) = OUT(4)

	IN(1) = f1*E(ee234) - D2345(dd123) + D1345(dd123) -
     &    2*Ginv(2,3)*(D2345(dd003) - D1345(dd003)) -
     &    2*Ginv(3,4)*(D2345(dd001) - D1345(dd001)) -
     &    2*Ginv(2,4)*(D2345(dd002) - D1345(dd002))
	IN(2) = f2*E(ee234) - D2345(dd123) -
     &    2*Ginv(2,3)*(D2345(dd003) - D1245(dd003)) -
     &    2*Ginv(3,4)*D2345(dd001) -
     &    2*Ginv(2,4)*(D2345(dd002) - D1245(dd002))
	IN(3) = f3*E(ee234) - D2345(dd123) -
     &    2*Ginv(2,3)*(D2345(dd003) - D1235(dd003)) -
     &    2*Ginv(3,4)*(D2345(dd001) - D1235(dd002)) -
     &    2*Ginv(2,4)*D2345(dd002)
	IN(4) = f4*E(ee234) - D2345(dd123) -
     &    2*Ginv(2,3)*D2345(dd003) -
     &    2*Ginv(3,4)*(D2345(dd001) - D1234(dd002)) -
     &    2*Ginv(2,4)*(D2345(dd002) - D1234(dd003))
	call SOLVER(in)
	E(ee1234) = OUT(1)
	E(ee2234) = 1/3D0*(E(ee2234) + OUT(2))
	E(ee2334) = .5D0*(E(ee2334) + OUT(3))
	E(ee2344) = OUT(4)

	IN(1) = f1*E(ee133) + d22sum -
     &    4*Ginv(1,3)*(D2345(dd002) - D1345(dd002)) +
     &    2*Ginv(3,3)*d00sum
	IN(2) = f2*E(ee133) + d22sum + D1245(dd122) -
     &    4*Ginv(1,3)*(D2345(dd002) - D1245(dd002)) +
     &    2*Ginv(3,3)*(d00sum + D1245(dd001))
	IN(3) = f3*E(ee133) + d22sum -
     &    4*Ginv(1,3)*D2345(dd002) +
     &    2*Ginv(3,3)*(d00sum + D1235(dd001))
	IN(4) = f4*E(ee133) + d22sum + D1234(dd133) -
     &    4*Ginv(1,3)*(D2345(dd002) - D1234(dd003)) +
     &    2*Ginv(3,3)*(d00sum + D1234(dd001))
	call SOLVER(in)
	E(ee1133) = .5D0*(E(ee1133) + OUT(1))
	E(ee1233) = OUT(2)
	E(ee1333) = .5D0*(E(ee1333) + OUT(3))
	E(ee1334) = .5D0*(E(ee1334) + OUT(4))

	if( dump ) call XDumpCoeff(5, E)
	end

