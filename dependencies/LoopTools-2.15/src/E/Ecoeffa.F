* Ecoeffa.F
* the five-point tensor coefficients
* this file is part of LoopTools
* written by M. Rauch
* last modified 9 Apr 18 th

#include "externals.h"
#include "types.h"

#define npoint 5
#include "defs.h"


	subroutine XEcoeffa(E, para, D2345, D1345, D1245, D1235, D1234)
	implicit none
	ComplexType E(*)
	ComplexType D2345(*), D1345(*), D1245(*), D1235(*), D1234(*)
	ArgType para(1,*)

#include "lt.h"

	ArgType p1, p2, p3, p4, p5
	ArgType p1p2, p2p3, p3p4, p4p5, p5p1
	ArgType m1, m2, m3, m4, m5
	ArgQuad Y(5,5), Yi(5,5), Z(4,4), Zij(3,3)
	ArgQuad eta(5), zeta(4,4), detY, detZ
	RealType del, del4
	integer i, j, k, l, finite
	logical dump

	ComplexQuad help1(0:2), help2(0:2), help3(0:2), help4(0:2)
	ComplexQuad dabbr41(0:2), dabbr48(0:2), dabbr65(0:2)
	ComplexQuad dabbr60(0:2), dabbr55(0:2), dabbr50(0:2)
	ComplexQuad dabbr49(0:2), dabbr45(0:2), dabbr42(0:2)
	ComplexQuad dabbr10(0:2), dabbr84(0:2), dabbr91(0:2)
	ComplexQuad dabbr81(0:2), dabbr52(0:2), dabbr88(0:2)
	ComplexQuad dabbr77(0:2), dabbr90(0:2), dabbr46(0:2)
	ComplexQuad dabbr87(0:2), dabbr74(0:2), dabbr80(0:2)
	ComplexQuad dabbr71(0:2), dabbr83(0:2), dabbr76(0:2)
	ComplexQuad dabbr79(0:2), dabbr43(0:2), dabbr70(0:2)
	ComplexQuad dabbr73(0:2), dabbr35(0:2), dabbr37(0:2)
	ComplexQuad dabbr39(0:2), dabbr27(0:2), dabbr31(0:2)
	ComplexQuad dabbr24(0:2), dabbr20(0:2), dabbr16(0:2)
	ComplexQuad dabbr30(0:2), dabbr13(0:2), dabbr23(0:2)
	ComplexQuad dabbr26(0:2), dabbr5(0:2), dabbr7(0:2)
	ComplexQuad dabbr9(0:2), dabbr33(0:2), dabbr92(0:2)
	ComplexQuad dabbr89(0:2), dabbr86(0:2), dabbr85(0:2)
	ComplexQuad dabbr82(0:2), dabbr78(0:2), dabbr75(0:2)
	ComplexQuad dabbr72(0:2), dabbr69(0:2), dabbr68(0:2)
	ComplexQuad dabbr38(0:2), dabbr36(0:2), dabbr34(0:2)
	ComplexQuad dabbr32(0:2), dabbr29(0:2), dabbr28(0:2)
	ComplexQuad dabbr25(0:2), dabbr22(0:2), dabbr21(0:2)
	ComplexQuad dabbr3(0:2), dabbr8(0:2), dabbr6(0:2)
	ComplexQuad dabbr4(0:2), dabbr1(0:2), dabbr61(0:2)
	ComplexQuad dabbr66(0:2), dabbr57(0:2), dabbr47(0:2)
	ComplexQuad dabbr51(0:2), dabbr64(0:2), dabbr44(0:2)
	ComplexQuad dabbr56(0:2), dabbr59(0:2), dabbr14(0:2)
	ComplexQuad dabbr17(0:2), dabbr19(0:2), dabbr67(0:2)
	ComplexQuad dabbr63(0:2), dabbr62(0:2), dabbr58(0:2)
	ComplexQuad dabbr54(0:2), dabbr53(0:2), dabbr11(0:2)
	ComplexQuad dabbr18(0:2), dabbr15(0:2), dabbr12(0:2)
	ComplexQuad dabbr2(0:2), dabbr40(0:2)

	serial = serial + 1
	dump = ibits(debugkey, DebugE, 1) .ne. 0 .and.
     &    serial .ge. debugfrom .and. serial .le. debugto
	if( dump ) call XDumpPara(5, para, "Ecoeffa")

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

	finite = 0
	del = 0
	if( lambda .ge. 0 ) then
	  finite = 1
	  del = (delta + log(mudim))/24D0
	else if( lambda .eq. -1 ) then
	  del = 1/24D0
	endif
	del4 = .25D0*del

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

* calculate the Y(i), their determinants, and eta(i)
	do i = 1, 5
	  Yi = Y
	  Yi(:,i) = 1
	  call XDet(5, Yi,5, eta(i))
	enddo

* Y is no longer needed, now calculate its determinant and
* add the missing factor 1/detY to eta
	call XDet(5, Y,5, detY)
	eta = eta/detY

	Z(1,1) = 2*p1
	Z(2,2) = 2*p1p2
	Z(3,3) = 2*p4p5
	Z(4,4) = 2*p5
	Z(1,2) = p1
	  Z(1,2) = Z(1,2) + p1p2
	  Z(1,2) = Z(1,2) - p2
	Z(2,1) = Z(1,2)
	Z(1,3) = p1
	  Z(1,3) = Z(1,3) - p2p3
	  Z(1,3) = Z(1,3) + p4p5
	Z(3,1) = Z(1,3)
	Z(1,4) = p1
	  Z(1,4) = Z(1,4) - p5p1
	  Z(1,4) = Z(1,4) + p5
	Z(4,1) = Z(1,4)
	Z(2,3) = p1p2
	  Z(2,3) = Z(2,3) - p3
	  Z(2,3) = Z(2,3) + p4p5
	Z(3,2) = Z(2,3)
	Z(2,4) = p1p2
	  Z(2,4) = Z(2,4) - p3p4
	  Z(2,4) = Z(2,4) + p5
	Z(4,2) = Z(2,4)
	Z(3,4) = p5
	  Z(3,4) = Z(3,4) + p4p5
	  Z(3,4) = Z(3,4) - p4
	Z(4,3) = Z(3,4)

* calculate the zeta(i,j)
	do i = 1, 4
	  do j = i, 4
* generate the submatrix Z_ij
	    forall(l = 1:3, k = 1:3)
     &        Zij(k,l) = Z(k+merge(1,0,k.ge.i),l+merge(1,0,l.ge.j))
	    call XDet(3, Zij,3, detZ)
	    zeta(i,j) = Sgn(i + j)*detZ/detY
	    zeta(j,i) = zeta(i,j)
	  enddo
	enddo
	call XDet(4, Z,4, detZ)

	help1 = D2345(DD003) + 2*D2345(DD0033) + D2345(DD00333)
	help2 = D2345(DD002) + 2*D2345(DD0023) + D2345(DD00233)
	help3 = D2345(DD001) + 2*D2345(DD0013) + D2345(DD00133)
	help4 = D2345(DD00) + 2*D2345(DD003) + D2345(DD0033)
	dabbr41 = D2345(DD00223) + D2345(DD00233)
	dabbr48 = D2345(DD00113) + D2345(DD00133)
	dabbr65 = D2345(DD0023) + D2345(DD00123)
	dabbr60 = D2345(DD0013) + D2345(DD00123)
	dabbr55 = D2345(DD0012) + D2345(DD00123)
	dabbr50 = D2345(DD00112) + D2345(DD00122)
	dabbr49 = D2345(DD2233) + D2345(DD2333)
	dabbr45 = D2345(DD2223) + D2345(DD2233)
	dabbr42 = D2345(DD1223) + D2345(DD1233)
	dabbr10 = D2345(DD223) + D2345(DD233)
	dabbr84 = D2345(DD0033) + D2345(DD00233) + D2345(DD00333)
	dabbr91 = D2345(DD0033) + D2345(DD00133) + D2345(DD00333)
	dabbr81 = dabbr41 + D2345(DD0023)
	dabbr52 = D2345(DD0033) + D2345(DD00133) + D2345(DD00233)
	dabbr88 = dabbr65 + D2345(DD00233)
	dabbr77 = D2345(DD0022) + D2345(DD00222) + D2345(DD00223)
	dabbr90 = dabbr65 + D2345(DD00223)
	dabbr46 = D2345(DD0022) + D2345(DD00122) + D2345(DD00223)
	dabbr87 = D2345(DD0022) + D2345(DD00122) + D2345(DD00222)
	dabbr74 = dabbr60 + D2345(DD00133)
	dabbr80 = dabbr48 + D2345(DD0013)
	dabbr71 = dabbr55 + D2345(DD00122)
	dabbr83 = dabbr60 + D2345(DD00113)
	dabbr76 = dabbr55 + D2345(DD00112)
	dabbr79 = dabbr50 + D2345(DD0012)
	dabbr43 = D2345(DD0011) + D2345(DD00112) + D2345(DD00113)
	dabbr70 = D2345(DD0011) + D2345(DD00111) + D2345(DD00113)
	dabbr73 = D2345(DD0011) + D2345(DD00111) + D2345(DD00112)
	dabbr35 = (D2345(DD0000) - del) + 
     &   (D2345(DD00002) + del4) + (D2345(DD00003) + del4)
	dabbr37 = (D2345(DD0000) - del) + 
     &   (D2345(DD00001) + del4) + (D2345(DD00003) + del4)
	dabbr39 = (D2345(DD0000) - del) + 
     &   (D2345(DD00001) + del4) + (D2345(DD00002) + del4)
	dabbr27 = D2345(DD003) + D2345(DD0023) + D2345(DD0033)
	dabbr31 = D2345(DD003) + D2345(DD0013) + D2345(DD0033)
	dabbr24 = D2345(DD002) + D2345(DD0022) + D2345(DD0023)
	dabbr20 = D2345(DD003) + D2345(DD0013) + D2345(DD0023)
	dabbr16 = D2345(DD002) + D2345(DD0012) + D2345(DD0023)
	dabbr30 = D2345(DD002) + D2345(DD0012) + D2345(DD0022)
	dabbr13 = D2345(DD001) + D2345(DD0012) + D2345(DD0013)
	dabbr23 = D2345(DD001) + D2345(DD0011) + D2345(DD0013)
	dabbr26 = D2345(DD001) + D2345(DD0011) + D2345(DD0012)
	dabbr5 = D2345(DD00) + D2345(DD002) + D2345(DD003)
	dabbr7 = D2345(DD00) + D2345(DD001) + D2345(DD003)
	dabbr9 = D2345(DD00) + D2345(DD001) + D2345(DD002)
	dabbr33 = dabbr35 + (D2345(DD00001) + del4)
	dabbr92 = D2345(DD333) + D2345(DD1333) + 
     &   D2345(DD2333) + D2345(DD3333)
	dabbr89 = dabbr49 + D2345(DD233) + D2345(DD1233)
	dabbr86 = dabbr45 + D2345(DD223) + D2345(DD1223)
	dabbr85 = D2345(DD222) + D2345(DD1222) + 
     &   D2345(DD2222) + D2345(DD2223)
	dabbr82 = D2345(DD133) + D2345(DD1133) + 
     &   D2345(DD1233) + D2345(DD1333)
	dabbr78 = dabbr42 + D2345(DD123) + D2345(DD1123)
	dabbr75 = D2345(DD122) + D2345(DD1122) + 
     &   D2345(DD1222) + D2345(DD1223)
	dabbr72 = D2345(DD113) + D2345(DD1113) + 
     &   D2345(DD1123) + D2345(DD1133)
	dabbr69 = D2345(DD112) + D2345(DD1112) + 
     &   D2345(DD1122) + D2345(DD1123)
	dabbr68 = D2345(DD111) + D2345(DD1111) + 
     &   D2345(DD1112) + D2345(DD1113)
	dabbr38 = dabbr27 + D2345(DD0013)
	dabbr36 = dabbr24 + D2345(DD0012)
	dabbr34 = dabbr13 + D2345(DD0011)
	dabbr32 = D2345(DD33) + D2345(DD133) + 
     &   D2345(DD233) + D2345(DD333)
	dabbr29 = dabbr10 + D2345(DD23) + D2345(DD123)
	dabbr28 = D2345(DD22) + D2345(DD122) + 
     &   D2345(DD222) + D2345(DD223)
	dabbr25 = D2345(DD13) + D2345(DD113) + 
     &   D2345(DD123) + D2345(DD133)
	dabbr22 = D2345(DD12) + D2345(DD112) + 
     &   D2345(DD122) + D2345(DD123)
	dabbr21 = D2345(DD11) + D2345(DD111) + 
     &   D2345(DD112) + D2345(DD113)
	dabbr3 = dabbr5 + D2345(DD001)
	dabbr8 = D2345(DD3) + D2345(DD13) + D2345(DD23) + D2345(DD33)
	dabbr6 = D2345(DD2) + D2345(DD12) + D2345(DD22) + D2345(DD23)
	dabbr4 = D2345(DD1) + D2345(DD11) + D2345(DD12) + D2345(DD13)
	dabbr1 = D2345(DD0) + D2345(DD1) + D2345(DD2) + D2345(DD3)
	dabbr61 = help1 + 2*D2345(DD0023) + 
     &   D2345(DD00223) + 2*D2345(DD00233)
	dabbr66 = help1 + 2*D2345(DD0013) + 
     &   D2345(DD00113) + 2*D2345(DD00133)
	dabbr57 = help2 + 2*D2345(DD0022) + 
     &   D2345(DD00222) + 2*D2345(DD00223)
	dabbr47 = help2 + 2*D2345(DD0012) + 
     &   D2345(DD00112) + 2*D2345(DD00123)
	dabbr51 = D2345(DD003) + 2*D2345(DD0013) + 
     &   2*D2345(DD0023) + D2345(DD00113) + 
     &   2*D2345(DD00123) + D2345(DD00223)
	dabbr64 = D2345(DD002) + 2*D2345(DD0012) + 
     &   2*D2345(DD0022) + D2345(DD00112) + 
     &   2*D2345(DD00122) + D2345(DD00222)
	dabbr44 = help3 + 2*D2345(DD0012) + 
     &   D2345(DD00122) + 2*D2345(DD00123)
	dabbr56 = help3 + 2*D2345(DD0011) + 
     &   D2345(DD00111) + 2*D2345(DD00113)
	dabbr59 = D2345(DD001) + 2*D2345(DD0011) + 
     &   2*D2345(DD0012) + D2345(DD00111) + 
     &   2*D2345(DD00112) + D2345(DD00122)
	dabbr14 = help4 + 2*D2345(DD002) + 
     &   D2345(DD0022) + 2*D2345(DD0023)
	dabbr17 = help4 + 2*D2345(DD001) + 
     &   D2345(DD0011) + 2*D2345(DD0013)
	dabbr19 = D2345(DD00) + 2*D2345(DD001) + 
     &   2*D2345(DD002) + D2345(DD0011) + 
     &   2*D2345(DD0012) + D2345(DD0022)
	dabbr67 = D2345(DD33) + 2*D2345(DD133) + 
     &   2*D2345(DD233) + 2*D2345(DD333) + 
     &   D2345(DD1133) + 2*D2345(DD1233) + 
     &   2*D2345(DD1333) + D2345(DD2233) + 
     &   2*D2345(DD2333) + D2345(DD3333)
	dabbr63 = D2345(DD23) + 2*D2345(DD123) + 
     &   2*D2345(DD223) + 2*D2345(DD233) + 
     &   D2345(DD1123) + 2*D2345(DD1223) + 
     &   2*D2345(DD1233) + D2345(DD2223) + 
     &   2*D2345(DD2233) + D2345(DD2333)
	dabbr62 = D2345(DD22) + 2*D2345(DD122) + 
     &   2*D2345(DD222) + 2*D2345(DD223) + 
     &   D2345(DD1122) + 2*D2345(DD1222) + 
     &   2*D2345(DD1223) + D2345(DD2222) + 
     &   2*D2345(DD2223) + D2345(DD2233)
	dabbr58 = D2345(DD13) + 2*D2345(DD113) + 
     &   2*D2345(DD123) + 2*D2345(DD133) + 
     &   D2345(DD1113) + 2*D2345(DD1123) + 
     &   2*D2345(DD1133) + D2345(DD1223) + 
     &   2*D2345(DD1233) + D2345(DD1333)
	dabbr54 = D2345(DD12) + 2*D2345(DD112) + 
     &   2*D2345(DD122) + 2*D2345(DD123) + 
     &   D2345(DD1112) + 2*D2345(DD1122) + 
     &   2*D2345(DD1123) + D2345(DD1222) + 
     &   2*D2345(DD1223) + D2345(DD1233)
	dabbr53 = D2345(DD11) + 2*D2345(DD111) + 
     &   2*D2345(DD112) + 2*D2345(DD113) + 
     &   D2345(DD1111) + 2*D2345(DD1112) + 
     &   2*D2345(DD1113) + D2345(DD1122) + 
     &   2*D2345(DD1123) + D2345(DD1133)
	dabbr11 = dabbr14 + 2*D2345(DD001) + 
     &   D2345(DD0011) + 2*D2345(DD0012) + 
     &   2*D2345(DD0013)
	dabbr18 = D2345(DD3) + 2*D2345(DD13) + 
     &   2*D2345(DD23) + 2*D2345(DD33) + 
     &   D2345(DD113) + 2*D2345(DD123) + 
     &   2*D2345(DD133) + D2345(DD223) + 
     &   2*D2345(DD233) + D2345(DD333)
	dabbr15 = D2345(DD2) + 2*D2345(DD12) + 
     &   2*D2345(DD22) + 2*D2345(DD23) + 
     &   D2345(DD112) + 2*D2345(DD122) + 
     &   2*D2345(DD123) + D2345(DD222) + 
     &   2*D2345(DD223) + D2345(DD233)
	dabbr12 = D2345(DD1) + 2*D2345(DD11) + 
     &   2*D2345(DD12) + 2*D2345(DD13) + 
     &   D2345(DD111) + 2*D2345(DD112) + 
     &   2*D2345(DD113) + D2345(DD122) + 
     &   2*D2345(DD123) + D2345(DD133)
	dabbr2 = D2345(DD0) + 2*D2345(DD1) + 
     &   2*D2345(DD2) + 2*D2345(DD3) + 
     &   D2345(DD11) + 2*D2345(DD12) + 
     &   2*D2345(DD13) + D2345(DD22) + 
     &   2*D2345(DD23) + D2345(DD33)
	dabbr40 = D2345(DD0) + 4*D2345(DD1) + 
     &   4*D2345(DD2) + 4*D2345(DD3) + 
     &   6*D2345(DD11) + 12*D2345(DD12) + 
     &   12*D2345(DD13) + 6*D2345(DD22) + 
     &   12*D2345(DD23) + 6*D2345(DD33) + 
     &   4*D2345(DD111) + 12*D2345(DD112) + 
     &   12*D2345(DD113) + 12*D2345(DD122) + 
     &   24*D2345(DD123) + 12*D2345(DD133) + 
     &   4*D2345(DD222) + 12*D2345(DD223) + 
     &   12*D2345(DD233) + 4*D2345(DD333) + 
     &   D2345(DD1111) + 4*D2345(DD1112) + 
     &   4*D2345(DD1113) + 6*D2345(DD1122) + 
     &   12*D2345(DD1123) + 6*D2345(DD1133) + 
     &   4*D2345(DD1222) + 12*D2345(DD1223) + 
     &   12*D2345(DD1233) + 4*D2345(DD1333) + 
     &   D2345(DD2222) + 4*D2345(DD2223) + 
     &   6*D2345(DD2233) + 4*D2345(DD2333) + 
     &   D2345(DD3333)

	call XE0func(E(EE0), para,
     &    D2345, D1345, D1245, D1235, D1234, 1)

	E(EE1) = dabbr1*eta(1) - eta(3)*D1245(DD1) - 
     &   eta(4)*D1235(DD1) - eta(5)*D1234(DD1) + 
     &   2*(zeta(1,1)*D1345(DD00) + 
     &      zeta(1,2)*D1245(DD00) + 
     &      zeta(1,3)*D1235(DD00) + 
     &      zeta(1,4)*D1234(DD00) - 
     &      (zeta(1,1) + zeta(1,2) + zeta(1,3) + zeta(1,4))*
     &       D2345(DD00))
	E(EE2) = -(eta(2)*D1345(DD1)) - 
     &   eta(4)*D1235(DD2) - eta(5)*D1234(DD2) - 
     &   eta(1)*D2345(DD1) + 
     &   2*zeta(1,2)*D1345(DD00) + 
     &   2*zeta(2,2)*D1245(DD00) + 
     &   2*zeta(2,3)*D1235(DD00) + 
     &   2*zeta(2,4)*D1234(DD00) - 
     &   2*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    D2345(DD00)
	E(EE3) = -(eta(2)*D1345(DD2)) - 
     &   eta(3)*D1245(DD2) - eta(5)*D1234(DD3) - 
     &   eta(1)*D2345(DD2) + 
     &   2*zeta(1,3)*D1345(DD00) + 
     &   2*zeta(2,3)*D1245(DD00) + 
     &   2*zeta(3,3)*D1235(DD00) + 
     &   2*zeta(3,4)*D1234(DD00) - 
     &   2*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    D2345(DD00)
	E(EE4) = -(eta(2)*D1345(DD3)) - 
     &   eta(3)*D1245(DD3) - eta(4)*D1235(DD3) - 
     &   eta(1)*D2345(DD3) + 
     &   2*zeta(1,4)*D1345(DD00) + 
     &   2*zeta(2,4)*D1245(DD00) + 
     &   2*zeta(3,4)*D1235(DD00) + 
     &   2*zeta(4,4)*D1234(DD00) - 
     &   2*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    D2345(DD00)

	E(EE00) = -(eta(2)*D1345(DD00)) - 
     &   eta(3)*D1245(DD00) - eta(4)*D1235(DD00) - 
     &   eta(5)*D1234(DD00) - eta(1)*D2345(DD00)
	E(EE11) = -(dabbr2*eta(1)) - eta(3)*D1245(DD11) - 
     &   eta(4)*D1235(DD11) - eta(5)*D1234(DD11) + 
     &   4*(dabbr3*(zeta(1,1) + zeta(1,2) + zeta(1,3) + 
     &         zeta(1,4)) + zeta(1,2)*D1245(DD001) + 
     &      zeta(1,3)*D1235(DD001) + 
     &      zeta(1,4)*D1234(DD001))
	E(EE12) = dabbr4*eta(1) - eta(4)*D1235(DD12) - 
     &   eta(5)*D1234(DD12) + 
     &   2*(dabbr5*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &         zeta(2,4)) + zeta(1,1)*D1345(DD001) + 
     &      zeta(2,2)*D1245(DD001) + 
     &      zeta(2,3)*D1235(DD001) + 
     &      zeta(2,4)*D1234(DD001) + 
     &      zeta(1,3)*D1235(DD002) + 
     &      zeta(1,4)*D1234(DD002) - 
     &      (zeta(1,1) + zeta(1,3) + zeta(1,4) - zeta(2,2) - 
     &         zeta(2,3) - zeta(2,4))*D2345(DD001))
	E(EE13) = dabbr6*eta(1) - eta(3)*D1245(DD12) - 
     &   eta(5)*D1234(DD13) + 
     &   2*(dabbr7*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &         zeta(3,4)) + zeta(2,3)*D1245(DD001) + 
     &      zeta(3,3)*D1235(DD001) + 
     &      zeta(3,4)*D1234(DD001) + 
     &      zeta(1,1)*D1345(DD002) + 
     &      zeta(1,2)*D1245(DD002) + 
     &      zeta(1,4)*D1234(DD003) - 
     &      (zeta(1,1) + zeta(1,2) + zeta(1,4) - zeta(2,3) - 
     &         zeta(3,3) - zeta(3,4))*D2345(DD002))
	E(EE14) = dabbr8*eta(1) - eta(3)*D1245(DD13) - 
     &   eta(4)*D1235(DD13) + 
     &   2*(dabbr9*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &         zeta(4,4)) + zeta(2,4)*D1245(DD001) + 
     &      zeta(3,4)*D1235(DD001) + 
     &      zeta(4,4)*D1234(DD001) + 
     &      zeta(1,1)*D1345(DD003) + 
     &      zeta(1,2)*D1245(DD003) + 
     &      zeta(1,3)*D1235(DD003) - 
     &      (zeta(1,1) + zeta(1,2) + zeta(1,3) - zeta(2,4) - 
     &         zeta(3,4) - zeta(4,4))*D2345(DD003))
	E(EE22) = -(eta(2)*D1345(DD11)) - 
     &   eta(4)*D1235(DD22) - eta(5)*D1234(DD22) - 
     &   eta(1)*D2345(DD11) + 
     &   4*zeta(1,2)*D1345(DD001) + 
     &   4*zeta(2,3)*D1235(DD002) + 
     &   4*zeta(2,4)*D1234(DD002) - 
     &   4*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    D2345(DD001)
	E(EE23) = -(eta(2)*D1345(DD12)) - 
     &   eta(5)*D1234(DD23) - eta(1)*D2345(DD12) + 
     &   2*(zeta(1,3)*D1345(DD001) + 
     &      zeta(1,2)*D1345(DD002) + 
     &      zeta(2,2)*D1245(DD002) + 
     &      zeta(3,3)*D1235(DD002) + 
     &      zeta(3,4)*D1234(DD002) + 
     &      zeta(2,4)*D1234(DD003) - 
     &      (zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &       D2345(DD001) - 
     &      (zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &       D2345(DD002))
	E(EE24) = -(eta(2)*D1345(DD13)) - 
     &   eta(4)*D1235(DD23) - eta(1)*D2345(DD13) + 
     &   2*(zeta(1,4)*D1345(DD001) + 
     &      zeta(3,4)*D1235(DD002) + 
     &      zeta(4,4)*D1234(DD002) + 
     &      zeta(1,2)*D1345(DD003) + 
     &      zeta(2,2)*D1245(DD003) + 
     &      zeta(2,3)*D1235(DD003) - 
     &      (zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       D2345(DD001) - 
     &      (zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &       D2345(DD003))
	E(EE33) = -(eta(2)*D1345(DD22)) - 
     &   eta(3)*D1245(DD22) - eta(5)*D1234(DD33) - 
     &   eta(1)*D2345(DD22) + 
     &   4*zeta(1,3)*D1345(DD002) + 
     &   4*zeta(2,3)*D1245(DD002) + 
     &   4*zeta(3,4)*D1234(DD003) - 
     &   4*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    D2345(DD002)
	E(EE34) = -(eta(2)*D1345(DD23)) - 
     &   eta(3)*D1245(DD23) - eta(1)*D2345(DD23) + 
     &   2*(zeta(1,4)*D1345(DD002) + 
     &      zeta(2,4)*D1245(DD002) + 
     &      zeta(1,3)*D1345(DD003) + 
     &      zeta(2,3)*D1245(DD003) + 
     &      zeta(3,3)*D1235(DD003) + 
     &      zeta(4,4)*D1234(DD003) - 
     &      (zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       D2345(DD002) - 
     &      (zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &       D2345(DD003))
	E(EE44) = -(eta(2)*D1345(DD33)) - 
     &   eta(3)*D1245(DD33) - eta(4)*D1235(DD33) - 
     &   eta(1)*D2345(DD33) + 
     &   4*zeta(1,4)*D1345(DD003) + 
     &   4*zeta(2,4)*D1245(DD003) + 
     &   4*zeta(3,4)*D1235(DD003) - 
     &   4*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    D2345(DD003)

	E(EE001) = dabbr3*eta(1) - eta(3)*D1245(DD001) - 
     &   eta(4)*D1235(DD001) - eta(5)*D1234(DD001) + 
     &   2*(zeta(1,1)*(D1345(DD0000) - del) + 
     &      zeta(1,2)*(D1245(DD0000) - del) + 
     &      zeta(1,3)*(D1235(DD0000) - del) + 
     &      zeta(1,4)*(D1234(DD0000) - del) - 
     &      (zeta(1,1) + zeta(1,2) + zeta(1,3) + zeta(1,4))*
     &       (D2345(DD0000) - del))
	E(EE002) = -(eta(2)*D1345(DD001)) - 
     &   eta(4)*D1235(DD002) - eta(5)*D1234(DD002) - 
     &   eta(1)*D2345(DD001) + 
     &   2*zeta(1,2)*(D1345(DD0000) - del) + 
     &   2*zeta(2,2)*(D1245(DD0000) - del) + 
     &   2*zeta(2,3)*(D1235(DD0000) - del) + 
     &   2*zeta(2,4)*(D1234(DD0000) - del) - 
     &   2*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    (D2345(DD0000) - del)
	E(EE003) = -(eta(2)*D1345(DD002)) - 
     &   eta(3)*D1245(DD002) - eta(5)*D1234(DD003) - 
     &   eta(1)*D2345(DD002) + 
     &   2*zeta(1,3)*(D1345(DD0000) - del) + 
     &   2*zeta(2,3)*(D1245(DD0000) - del) + 
     &   2*zeta(3,3)*(D1235(DD0000) - del) + 
     &   2*zeta(3,4)*(D1234(DD0000) - del) - 
     &   2*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    (D2345(DD0000) - del)
	E(EE004) = -(eta(2)*D1345(DD003)) - 
     &   eta(3)*D1245(DD003) - eta(4)*D1235(DD003) - 
     &   eta(1)*D2345(DD003) + 
     &   2*zeta(1,4)*(D1345(DD0000) - del) + 
     &   2*zeta(2,4)*(D1245(DD0000) - del) + 
     &   2*zeta(3,4)*(D1235(DD0000) - del) + 
     &   2*zeta(4,4)*(D1234(DD0000) - del) - 
     &   2*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    (D2345(DD0000) - del)
	E(EE111) = -6*dabbr11*
     &    (zeta(1,1) + zeta(1,2) + zeta(1,3) + zeta(1,4)) - 
     &   eta(3)*D1245(DD111) - eta(4)*D1235(DD111) - 
     &   eta(5)*D1234(DD111) + 
     &   eta(1)*(3*dabbr10 + D2345(DD0) + 
     &      3*D2345(DD1) + 3*D2345(DD2) + 
     &      3*D2345(DD3) + 3*D2345(DD11) + 
     &      6*D2345(DD12) + 6*D2345(DD13) + 
     &      3*D2345(DD22) + 6*D2345(DD23) + 
     &      3*D2345(DD33) + D2345(DD111) + 
     &      3*D2345(DD112) + 3*D2345(DD113) + 
     &      3*D2345(DD122) + 6*D2345(DD123) + 
     &      3*D2345(DD133) + D2345(DD222) + 
     &      D2345(DD333)) + 
     &   6*zeta(1,2)*D1245(DD0011) + 
     &   6*zeta(1,3)*D1235(DD0011) + 
     &   6*zeta(1,4)*D1234(DD0011)
	E(EE112) = -(dabbr12*eta(1)) - 
     &   2*dabbr14*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &      zeta(2,4)) - eta(4)*D1235(DD112) - 
     &   eta(5)*D1234(DD112) + 
     &   2*(2*dabbr13*(zeta(1,1) + zeta(1,3) + zeta(1,4) - 
     &         zeta(2,2) - zeta(2,3) - zeta(2,4)) + 
     &      zeta(2,2)*D1245(DD0011) + 
     &      zeta(2,3)*D1235(DD0011) + 
     &      zeta(2,4)*D1234(DD0011) + 
     &      2*zeta(1,3)*D1235(DD0012) + 
     &      2*zeta(1,4)*D1234(DD0012) + 
     &      (2*zeta(1,1) + zeta(1,2) + 2*zeta(1,3) + 2*zeta(1,4) - 
     &         zeta(2,2) - zeta(2,3) - zeta(2,4))*
     &       D2345(DD0011))
	E(EE113) = -(dabbr15*eta(1)) - 
     &   2*dabbr17*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &      zeta(3,4)) - eta(3)*D1245(DD112) - 
     &   eta(5)*D1234(DD113) + 
     &   2*(2*dabbr16*(zeta(1,1) + zeta(1,2) + zeta(1,4) - 
     &         zeta(2,3) - zeta(3,3) - zeta(3,4)) + 
     &      zeta(2,3)*D1245(DD0011) + 
     &      zeta(3,3)*D1235(DD0011) + 
     &      zeta(3,4)*D1234(DD0011) + 
     &      2*zeta(1,2)*D1245(DD0012) + 
     &      2*zeta(1,4)*D1234(DD0013) + 
     &      (2*zeta(1,1) + 2*zeta(1,2) + zeta(1,3) + 2*zeta(1,4) - 
     &         zeta(2,3) - zeta(3,3) - zeta(3,4))*
     &       D2345(DD0022))
	E(EE114) = -(dabbr18*eta(1)) - 
     &   2*dabbr19*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &      zeta(4,4)) - eta(3)*D1245(DD113) - 
     &   eta(4)*D1235(DD113) + 
     &   2*(2*dabbr20*(zeta(1,1) + zeta(1,2) + zeta(1,3) - 
     &         zeta(2,4) - zeta(3,4) - zeta(4,4)) + 
     &      zeta(2,4)*D1245(DD0011) + 
     &      zeta(3,4)*D1235(DD0011) + 
     &      zeta(4,4)*D1234(DD0011) + 
     &      2*zeta(1,2)*D1245(DD0013) + 
     &      2*zeta(1,3)*D1235(DD0013) + 
     &      (2*zeta(1,1) + 2*zeta(1,2) + 2*zeta(1,3) + zeta(1,4) - 
     &         zeta(2,4) - zeta(3,4) - zeta(4,4))*
     &       D2345(DD0033))
	E(EE122) = dabbr21*eta(1) - eta(4)*D1235(DD122) - 
     &   eta(5)*D1234(DD122) + 
     &   2*(2*dabbr13*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &         zeta(2,4)) + zeta(1,1)*D1345(DD0011) + 
     &      2*zeta(2,3)*D1235(DD0012) + 
     &      2*zeta(2,4)*D1234(DD0012) + 
     &      zeta(1,3)*D1235(DD0022) + 
     &      zeta(1,4)*D1234(DD0022) - 
     &      (zeta(1,1) - zeta(1,2) + zeta(1,3) + zeta(1,4) - 
     &         2*(zeta(2,2) + zeta(2,3) + zeta(2,4)))*
     &       D2345(DD0011))
	E(EE123) = dabbr22*eta(1) - eta(5)*D1234(DD123) + 
     &   2*(dabbr24*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &         zeta(2,4)) + 
     &      dabbr23*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &         zeta(3,4)) + zeta(1,1)*D1345(DD0012) + 
     &      zeta(2,2)*D1245(DD0012) + 
     &      zeta(3,3)*D1235(DD0012) + 
     &      zeta(3,4)*D1234(DD0012) + 
     &      zeta(2,4)*D1234(DD0013) + 
     &      zeta(1,4)*D1234(DD0023) + 
     &      (-zeta(1,1) - zeta(1,4) + zeta(2,2) + 2*zeta(2,3) + 
     &         zeta(2,4) + zeta(3,3) + zeta(3,4))*
     &       D2345(DD0012))
	E(EE124) = dabbr25*eta(1) - eta(4)*D1235(DD123) + 
     &   2*(dabbr27*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &         zeta(2,4)) + 
     &      dabbr26*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &         zeta(4,4)) + zeta(3,4)*D1235(DD0012) + 
     &      zeta(4,4)*D1234(DD0012) + 
     &      zeta(1,1)*D1345(DD0013) + 
     &      zeta(2,2)*D1245(DD0013) + 
     &      zeta(2,3)*D1235(DD0013) + 
     &      zeta(1,3)*D1235(DD0023) + 
     &      (-zeta(1,1) - zeta(1,3) + zeta(2,2) + zeta(2,3) + 
     &         2*zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       D2345(DD0013))
	E(EE133) = dabbr28*eta(1) - eta(3)*D1245(DD122) - 
     &   eta(5)*D1234(DD133) + 
     &   2*(2*dabbr16*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &         zeta(3,4)) + 2*zeta(2,3)*D1245(DD0012) + 
     &      2*zeta(3,4)*D1234(DD0013) + 
     &      zeta(1,1)*D1345(DD0022) + 
     &      zeta(1,2)*D1245(DD0022) + 
     &      zeta(1,4)*D1234(DD0033) - 
     &      (zeta(1,1) + zeta(1,2) - zeta(1,3) + zeta(1,4) - 
     &         2*(zeta(2,3) + zeta(3,3) + zeta(3,4)))*
     &       D2345(DD0022))
	E(EE134) = dabbr29*eta(1) - eta(3)*D1245(DD123) + 
     &   2*(dabbr31*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &         zeta(3,4)) + 
     &      dabbr30*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &         zeta(4,4)) + zeta(2,4)*D1245(DD0012) + 
     &      zeta(2,3)*D1245(DD0013) + 
     &      zeta(3,3)*D1235(DD0013) + 
     &      zeta(4,4)*D1234(DD0013) + 
     &      zeta(1,1)*D1345(DD0023) + 
     &      zeta(1,2)*D1245(DD0023) + 
     &      (-zeta(1,1) - zeta(1,2) + zeta(2,3) + zeta(2,4) + 
     &         zeta(3,3) + 2*zeta(3,4) + zeta(4,4))*
     &       D2345(DD0023))
	E(EE144) = dabbr32*eta(1) - eta(3)*D1245(DD133) - 
     &   eta(4)*D1235(DD133) + 
     &   2*(2*dabbr20*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &         zeta(4,4)) + 2*zeta(2,4)*D1245(DD0013) + 
     &      2*zeta(3,4)*D1235(DD0013) + 
     &      zeta(1,1)*D1345(DD0033) + 
     &      zeta(1,2)*D1245(DD0033) + 
     &      zeta(1,3)*D1235(DD0033) - 
     &      (zeta(1,1) + zeta(1,2) + zeta(1,3) - zeta(1,4) - 
     &         2*(zeta(2,4) + zeta(3,4) + zeta(4,4)))*
     &       D2345(DD0033))
	E(EE222) = -(eta(2)*D1345(DD111)) - 
     &   eta(4)*D1235(DD222) - eta(5)*D1234(DD222) - 
     &   eta(1)*D2345(DD111) + 
     &   6*zeta(1,2)*D1345(DD0011) + 
     &   6*zeta(2,3)*D1235(DD0022) + 
     &   6*zeta(2,4)*D1234(DD0022) - 
     &   6*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    D2345(DD0011)
	E(EE223) = -(eta(2)*D1345(DD112)) - 
     &   eta(5)*D1234(DD223) - eta(1)*D2345(DD112) + 
     &   2*zeta(1,3)*D1345(DD0011) + 
     &   4*zeta(1,2)*D1345(DD0012) + 
     &   2*zeta(3,3)*D1235(DD0022) + 
     &   2*zeta(3,4)*D1234(DD0022) + 
     &   4*zeta(2,4)*D1234(DD0023) - 
     &   2*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    D2345(DD0011) - 
     &   4*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    D2345(DD0012)
	E(EE224) = -(eta(2)*D1345(DD113)) - 
     &   eta(4)*D1235(DD223) - eta(1)*D2345(DD113) + 
     &   2*zeta(1,4)*D1345(DD0011) + 
     &   4*zeta(1,2)*D1345(DD0013) + 
     &   2*zeta(3,4)*D1235(DD0022) + 
     &   2*zeta(4,4)*D1234(DD0022) + 
     &   4*zeta(2,3)*D1235(DD0023) - 
     &   2*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    D2345(DD0011) - 
     &   4*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    D2345(DD0013)
	E(EE233) = -(eta(2)*D1345(DD122)) - 
     &   eta(5)*D1234(DD233) - eta(1)*D2345(DD122) + 
     &   4*zeta(1,3)*D1345(DD0012) + 
     &   2*zeta(1,2)*D1345(DD0022) + 
     &   2*zeta(2,2)*D1245(DD0022) + 
     &   4*zeta(3,4)*D1234(DD0023) + 
     &   2*zeta(2,4)*D1234(DD0033) - 
     &   4*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    D2345(DD0012) - 
     &   2*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    D2345(DD0022)
	E(EE234) = -(eta(2)*D1345(DD123)) - 
     &   eta(1)*D2345(DD123) + 
     &   2*(zeta(1,4)*D1345(DD0012) + 
     &      zeta(1,3)*D1345(DD0013) + 
     &      zeta(1,2)*D1345(DD0023) + 
     &      zeta(2,2)*D1245(DD0023) + 
     &      zeta(3,3)*D1235(DD0023) + 
     &      zeta(4,4)*D1234(DD0023) - 
     &      (zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       D2345(DD0012) - 
     &      (zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &       D2345(DD0013) - 
     &      (zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &       D2345(DD0023))
	E(EE244) = -(eta(2)*D1345(DD133)) - 
     &   eta(4)*D1235(DD233) - eta(1)*D2345(DD133) + 
     &   4*zeta(1,4)*D1345(DD0013) + 
     &   4*zeta(3,4)*D1235(DD0023) + 
     &   2*zeta(1,2)*D1345(DD0033) + 
     &   2*zeta(2,2)*D1245(DD0033) + 
     &   2*zeta(2,3)*D1235(DD0033) - 
     &   4*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    D2345(DD0013) - 
     &   2*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    D2345(DD0033)
	E(EE333) = -(eta(2)*D1345(DD222)) - 
     &   eta(3)*D1245(DD222) - eta(5)*D1234(DD333) - 
     &   eta(1)*D2345(DD222) + 
     &   6*zeta(1,3)*D1345(DD0022) + 
     &   6*zeta(2,3)*D1245(DD0022) + 
     &   6*zeta(3,4)*D1234(DD0033) - 
     &   6*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    D2345(DD0022)
	E(EE334) = -(eta(2)*D1345(DD223)) - 
     &   eta(3)*D1245(DD223) - eta(1)*D2345(DD223) + 
     &   2*zeta(1,4)*D1345(DD0022) + 
     &   2*zeta(2,4)*D1245(DD0022) + 
     &   4*zeta(1,3)*D1345(DD0023) + 
     &   4*zeta(2,3)*D1245(DD0023) + 
     &   2*zeta(4,4)*D1234(DD0033) - 
     &   2*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    D2345(DD0022) - 
     &   4*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    D2345(DD0023)
	E(EE344) = -(eta(2)*D1345(DD233)) - 
     &   eta(3)*D1245(DD233) - eta(1)*D2345(DD233) + 
     &   4*zeta(1,4)*D1345(DD0023) + 
     &   4*zeta(2,4)*D1245(DD0023) + 
     &   2*zeta(1,3)*D1345(DD0033) + 
     &   2*zeta(2,3)*D1245(DD0033) + 
     &   2*zeta(3,3)*D1235(DD0033) - 
     &   4*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    D2345(DD0023) - 
     &   2*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    D2345(DD0033)
	E(EE444) = -(eta(2)*D1345(DD333)) - 
     &   eta(3)*D1245(DD333) - eta(4)*D1235(DD333) - 
     &   eta(1)*D2345(DD333) + 
     &   6*zeta(1,4)*D1345(DD0033) + 
     &   6*zeta(2,4)*D1245(DD0033) + 
     &   6*zeta(3,4)*D1235(DD0033) - 
     &   6*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    D2345(DD0033)

	E(EE0000) = (-finite*(detZ/detY) - 
     &     48*(eta(2)*(D1345(DD0000) - del) + 
     &        eta(3)*(D1245(DD0000) - del) + 
     &        eta(4)*(D1235(DD0000) - del) + 
     &        eta(5)*(D1234(DD0000) - del) + 
     &        eta(1)*(D2345(DD0000) - del)))/48D0
	E(EE0011) = -(dabbr11*eta(1)) - 
     &   eta(3)*D1245(DD0011) - 
     &   eta(4)*D1235(DD0011) - 
     &   eta(5)*D1234(DD0011) + 
     &   4*(dabbr33*(zeta(1,1) + zeta(1,2) + zeta(1,3) + 
     &         zeta(1,4)) + 
     &      zeta(1,2)*(D1245(DD00001) + del4) + 
     &      zeta(1,3)*(D1235(DD00001) + del4) + 
     &      zeta(1,4)*(D1234(DD00001) + del4))
	E(EE0012) = dabbr34*eta(1) - 
     &   eta(4)*D1235(DD0012) - 
     &   eta(5)*D1234(DD0012) + 
     &   2*(dabbr35*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &         zeta(2,4)) + 
     &      zeta(1,1)*(D1345(DD00001) + del4) + 
     &      zeta(2,2)*(D1245(DD00001) + del4) + 
     &      zeta(2,3)*(D1235(DD00001) + del4) + 
     &      zeta(2,4)*(D1234(DD00001) + del4) + 
     &      zeta(1,3)*(D1235(DD00002) + del4) + 
     &      zeta(1,4)*(D1234(DD00002) + del4) - 
     &      (zeta(1,1) + zeta(1,3) + zeta(1,4) - zeta(2,2) - 
     &         zeta(2,3) - zeta(2,4))*
     &       (D2345(DD00001) + del4))
	E(EE0013) = dabbr36*eta(1) - 
     &   eta(3)*D1245(DD0012) - 
     &   eta(5)*D1234(DD0013) + 
     &   2*(dabbr37*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &         zeta(3,4)) + 
     &      zeta(2,3)*(D1245(DD00001) + del4) + 
     &      zeta(3,3)*(D1235(DD00001) + del4) + 
     &      zeta(3,4)*(D1234(DD00001) + del4) + 
     &      zeta(1,1)*(D1345(DD00002) + del4) + 
     &      zeta(1,2)*(D1245(DD00002) + del4) + 
     &      zeta(1,4)*(D1234(DD00003) + del4) - 
     &      (zeta(1,1) + zeta(1,2) + zeta(1,4) - zeta(2,3) - 
     &         zeta(3,3) - zeta(3,4))*
     &       (D2345(DD00002) + del4))
	E(EE0014) = dabbr38*eta(1) - 
     &   eta(3)*D1245(DD0013) - 
     &   eta(4)*D1235(DD0013) + 
     &   2*(dabbr39*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &         zeta(4,4)) + 
     &      zeta(2,4)*(D1245(DD00001) + del4) + 
     &      zeta(3,4)*(D1235(DD00001) + del4) + 
     &      zeta(4,4)*(D1234(DD00001) + del4) + 
     &      zeta(1,1)*(D1345(DD00003) + del4) + 
     &      zeta(1,2)*(D1245(DD00003) + del4) + 
     &      zeta(1,3)*(D1235(DD00003) + del4) - 
     &      (zeta(1,1) + zeta(1,2) + zeta(1,3) - zeta(2,4) - 
     &         zeta(3,4) - zeta(4,4))*
     &       (D2345(DD00003) + del4))
	E(EE0022) = -(eta(2)*D1345(DD0011)) - 
     &   eta(4)*D1235(DD0022) - 
     &   eta(5)*D1234(DD0022) - 
     &   eta(1)*D2345(DD0011) + 
     &   4*zeta(1,2)*(D1345(DD00001) + del4) + 
     &   4*zeta(2,3)*(D1235(DD00002) + del4) + 
     &   4*zeta(2,4)*(D1234(DD00002) + del4) - 
     &   4*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    (D2345(DD00001) + del4)
	E(EE0023) = -(eta(2)*D1345(DD0012)) - 
     &   eta(5)*D1234(DD0023) - 
     &   eta(1)*D2345(DD0012) + 
     &   2*(zeta(1,3)*(D1345(DD00001) + del4) + 
     &      zeta(1,2)*(D1345(DD00002) + del4) + 
     &      zeta(2,2)*(D1245(DD00002) + del4) + 
     &      zeta(3,3)*(D1235(DD00002) + del4) + 
     &      zeta(3,4)*(D1234(DD00002) + del4) + 
     &      zeta(2,4)*(D1234(DD00003) + del4) - 
     &      (zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &       (D2345(DD00001) + del4) - 
     &      (zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &       (D2345(DD00002) + del4))
	E(EE0024) = -(eta(2)*D1345(DD0013)) - 
     &   eta(4)*D1235(DD0023) - 
     &   eta(1)*D2345(DD0013) + 
     &   2*(zeta(1,4)*(D1345(DD00001) + del4) + 
     &      zeta(3,4)*(D1235(DD00002) + del4) + 
     &      zeta(4,4)*(D1234(DD00002) + del4) + 
     &      zeta(1,2)*(D1345(DD00003) + del4) + 
     &      zeta(2,2)*(D1245(DD00003) + del4) + 
     &      zeta(2,3)*(D1235(DD00003) + del4) - 
     &      (zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       (D2345(DD00001) + del4) - 
     &      (zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &       (D2345(DD00003) + del4))
	E(EE0033) = -(eta(2)*D1345(DD0022)) - 
     &   eta(3)*D1245(DD0022) - 
     &   eta(5)*D1234(DD0033) - 
     &   eta(1)*D2345(DD0022) + 
     &   4*zeta(1,3)*(D1345(DD00002) + del4) + 
     &   4*zeta(2,3)*(D1245(DD00002) + del4) + 
     &   4*zeta(3,4)*(D1234(DD00003) + del4) - 
     &   4*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    (D2345(DD00002) + del4)
	E(EE0034) = -(eta(2)*D1345(DD0023)) - 
     &   eta(3)*D1245(DD0023) - 
     &   eta(1)*D2345(DD0023) + 
     &   2*(zeta(1,4)*(D1345(DD00002) + del4) + 
     &      zeta(2,4)*(D1245(DD00002) + del4) + 
     &      zeta(1,3)*(D1345(DD00003) + del4) + 
     &      zeta(2,3)*(D1245(DD00003) + del4) + 
     &      zeta(3,3)*(D1235(DD00003) + del4) + 
     &      zeta(4,4)*(D1234(DD00003) + del4) - 
     &      (zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       (D2345(DD00002) + del4) - 
     &      (zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &       (D2345(DD00003) + del4))
	E(EE0044) = -(eta(2)*D1345(DD0033)) - 
     &   eta(3)*D1245(DD0033) - 
     &   eta(4)*D1235(DD0033) - 
     &   eta(1)*D2345(DD0033) + 
     &   4*zeta(1,4)*(D1345(DD00003) + del4) + 
     &   4*zeta(2,4)*(D1245(DD00003) + del4) + 
     &   4*zeta(3,4)*(D1235(DD00003) + del4) - 
     &   4*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    (D2345(DD00003) + del4)
	E(EE1111) = -(dabbr40*eta(1)) - 
     &   eta(3)*D1245(DD1111) - 
     &   eta(4)*D1235(DD1111) - 
     &   eta(5)*D1234(DD1111) + 
     &   8*zeta(1,2)*D1245(DD00111) + 
     &   8*zeta(1,3)*D1235(DD00111) + 
     &   8*zeta(1,4)*D1234(DD00111) + 
     &   8*(zeta(1,1) + zeta(1,2) + zeta(1,3) + zeta(1,4))*
     &    (3*dabbr41 + D2345(DD00) + 
     &      3*D2345(DD001) + 3*D2345(DD002) + 
     &      3*D2345(DD003) + 3*D2345(DD0011) + 
     &      6*D2345(DD0012) + 6*D2345(DD0013) + 
     &      3*D2345(DD0022) + 6*D2345(DD0023) + 
     &      3*D2345(DD0033) + D2345(DD00111) + 
     &      3*D2345(DD00112) + 3*D2345(DD00113) + 
     &      3*D2345(DD00122) + 6*D2345(DD00123) + 
     &      3*D2345(DD00133) + D2345(DD00222) + 
     &      D2345(DD00333))
	E(EE1112) = -6*dabbr44*
     &    (zeta(1,1) + zeta(1,3) + zeta(1,4) - zeta(2,2) - 
     &      zeta(2,3) - zeta(2,4)) - 
     &   6*dabbr43*(2*zeta(1,1) + zeta(1,2) + 2*zeta(1,3) + 
     &      2*zeta(1,4) - zeta(2,2) - zeta(2,3) - zeta(2,4)) - 
     &   eta(4)*D1235(DD1112) - 
     &   eta(5)*D1234(DD1112) + 
     &   eta(1)*(3*dabbr42 + D2345(DD1) + 
     &      3*D2345(DD11) + 3*D2345(DD12) + 
     &      3*D2345(DD13) + 3*D2345(DD111) + 
     &      6*D2345(DD112) + 6*D2345(DD113) + 
     &      3*D2345(DD122) + 6*D2345(DD123) + 
     &      3*D2345(DD133) + D2345(DD1111) + 
     &      3*D2345(DD1112) + 3*D2345(DD1113) + 
     &      3*D2345(DD1122) + 6*D2345(DD1123) + 
     &      3*D2345(DD1133) + D2345(DD1222) + 
     &      D2345(DD1333)) + 
     &   2*(zeta(2,2)*D1245(DD00111) + 
     &      zeta(2,3)*D1235(DD00111) + 
     &      zeta(2,4)*D1234(DD00111) + 
     &      3*zeta(1,3)*D1235(DD00112) + 
     &      3*zeta(1,4)*D1234(DD00112) + 
     &      (-3*zeta(1,1) - 2*zeta(1,2) - 3*zeta(1,3) - 
     &         3*zeta(1,4) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &       D2345(DD00111)) + 
     &   2*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    (3*dabbr41 + D2345(DD00) + 
     &      3*D2345(DD002) + 3*D2345(DD003) + 
     &      3*D2345(DD0022) + 6*D2345(DD0023) + 
     &      3*D2345(DD0033) + D2345(DD00222) + 
     &      D2345(DD00333))
	E(EE1113) = -6*dabbr47*
     &    (zeta(1,1) + zeta(1,2) + zeta(1,4) - zeta(2,3) - 
     &      zeta(3,3) - zeta(3,4)) - 
     &   6*dabbr46*(2*zeta(1,1) + 2*zeta(1,2) + zeta(1,3) + 
     &      2*zeta(1,4) - zeta(2,3) - zeta(3,3) - zeta(3,4)) - 
     &   eta(3)*D1245(DD1112) - 
     &   eta(5)*D1234(DD1113) + 
     &   eta(1)*(3*dabbr45 + D2345(DD2) + 
     &      3*D2345(DD12) + 3*D2345(DD22) + 
     &      3*D2345(DD23) + 3*D2345(DD112) + 
     &      6*D2345(DD122) + 6*D2345(DD123) + 
     &      3*D2345(DD222) + 6*D2345(DD223) + 
     &      3*D2345(DD233) + D2345(DD1112) + 
     &      3*D2345(DD1122) + 3*D2345(DD1123) + 
     &      3*D2345(DD1222) + 6*D2345(DD1223) + 
     &      3*D2345(DD1233) + D2345(DD2222) + 
     &      D2345(DD2333)) + 
     &   2*(zeta(2,3)*D1245(DD00111) + 
     &      zeta(3,3)*D1235(DD00111) + 
     &      zeta(3,4)*D1234(DD00111) + 
     &      3*zeta(1,2)*D1245(DD00112) + 
     &      3*zeta(1,4)*D1234(DD00113) + 
     &      (-3*zeta(1,1) - 3*zeta(1,2) - 2*zeta(1,3) - 
     &         3*zeta(1,4) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &       D2345(DD00222)) + 
     &   2*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    (3*dabbr48 + D2345(DD00) + 
     &      3*D2345(DD001) + 3*D2345(DD003) + 
     &      3*D2345(DD0011) + 6*D2345(DD0013) + 
     &      3*D2345(DD0033) + D2345(DD00111) + 
     &      D2345(DD00333))
	E(EE1114) = -6*dabbr51*
     &    (zeta(1,1) + zeta(1,2) + zeta(1,3) - zeta(2,4) - 
     &      zeta(3,4) - zeta(4,4)) - 
     &   6*dabbr52*(2*zeta(1,1) + 2*zeta(1,2) + 2*zeta(1,3) + 
     &      zeta(1,4) - zeta(2,4) - zeta(3,4) - zeta(4,4)) - 
     &   eta(3)*D1245(DD1113) - 
     &   eta(4)*D1235(DD1113) + 
     &   eta(1)*(3*dabbr49 + D2345(DD3) + 
     &      3*D2345(DD13) + 3*D2345(DD23) + 
     &      3*D2345(DD33) + 3*D2345(DD113) + 
     &      6*D2345(DD123) + 6*D2345(DD133) + 
     &      3*D2345(DD223) + 6*D2345(DD233) + 
     &      3*D2345(DD333) + D2345(DD1113) + 
     &      3*D2345(DD1123) + 3*D2345(DD1133) + 
     &      3*D2345(DD1223) + 6*D2345(DD1233) + 
     &      3*D2345(DD1333) + D2345(DD2223) + 
     &      D2345(DD3333)) + 
     &   2*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    (3*dabbr50 + D2345(DD00) + 
     &      3*D2345(DD001) + 3*D2345(DD002) + 
     &      3*D2345(DD0011) + 6*D2345(DD0012) + 
     &      3*D2345(DD0022) + D2345(DD00111) + 
     &      D2345(DD00222)) + 
     &   2*(zeta(2,4)*D1245(DD00111) + 
     &      zeta(3,4)*D1235(DD00111) + 
     &      zeta(4,4)*D1234(DD00111) + 
     &      3*zeta(1,2)*D1245(DD00113) + 
     &      3*zeta(1,3)*D1235(DD00113) + 
     &      (-3*zeta(1,1) - 3*zeta(1,2) - 3*zeta(1,3) - 
     &         2*zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       D2345(DD00333))
	E(EE1122) = -(dabbr53*eta(1)) - 
     &   4*dabbr44*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &      zeta(2,4)) - eta(4)*D1235(DD1122) - 
     &   eta(5)*D1234(DD1122) + 
     &   4*(-2*dabbr43*(zeta(2,2) + zeta(2,3) + zeta(2,4)) + 
     &      zeta(1,1)*D2345(DD0011) - 
     &      zeta(1,2)*D2345(DD0011) + 
     &      zeta(1,3)*D2345(DD0011) + 
     &      zeta(1,4)*D2345(DD0011) + 
     &      zeta(2,3)*D1235(DD00112) + 
     &      zeta(2,4)*D1234(DD00112) + 
     &      zeta(1,3)*D1235(DD00122) + 
     &      zeta(1,4)*D1234(DD00122) + 
     &      (zeta(1,1) + zeta(1,3) + zeta(1,4) - zeta(2,2) - 
     &         zeta(2,3) - zeta(2,4))*D2345(DD00111) + 
     &      zeta(1,1)*D2345(DD00112) - 
     &      zeta(1,2)*D2345(DD00112) + 
     &      zeta(1,3)*D2345(DD00112) + 
     &      zeta(1,4)*D2345(DD00112) + 
     &      (zeta(1,1) - zeta(1,2) + zeta(1,3) + zeta(1,4))*
     &       D2345(DD00113))
	E(EE1123) = -(dabbr54*eta(1)) - 
     &   2*dabbr57*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &      zeta(2,4)) - 2*dabbr56*
     &    (zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4)) - 
     &   eta(5)*D1234(DD1123) + 
     &   2*(2*dabbr55*(zeta(1,1) + zeta(1,4) - zeta(2,2) - 
     &         2*zeta(2,3) - zeta(2,4) - zeta(3,3) - zeta(3,4)) + 
     &      zeta(2,2)*D1245(DD00112) + 
     &      zeta(3,3)*D1235(DD00112) + 
     &      zeta(3,4)*D1234(DD00112) + 
     &      zeta(2,4)*D1234(DD00113) + 
     &      2*zeta(1,4)*D1234(DD00123) + 
     &      (2*zeta(1,1) + zeta(1,2) + 2*zeta(1,4) - zeta(2,2) - 
     &         3*zeta(2,3) - zeta(2,4) - 2*zeta(3,3) - 2*zeta(3,4))
     &        *D2345(DD00112) + 
     &      (2*zeta(1,1) + zeta(1,3) + 2*zeta(1,4) - 2*zeta(2,2) - 
     &         3*zeta(2,3) - 2*zeta(2,4) - zeta(3,3) - zeta(3,4))*
     &       D2345(DD00122))
	E(EE1124) = -(dabbr58*eta(1)) - 
     &   2*dabbr61*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &      zeta(2,4)) - 2*dabbr59*
     &    (zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4)) - 
     &   eta(4)*D1235(DD1123) + 
     &   2*(2*dabbr60*(zeta(1,1) + zeta(1,3) - zeta(2,2) - 
     &         zeta(2,3) - 2*zeta(2,4) - zeta(3,4) - zeta(4,4)) + 
     &      zeta(3,4)*D1235(DD00112) + 
     &      zeta(4,4)*D1234(DD00112) + 
     &      zeta(2,2)*D1245(DD00113) + 
     &      zeta(2,3)*D1235(DD00113) + 
     &      2*zeta(1,3)*D1235(DD00123) + 
     &      (2*zeta(1,1) + zeta(1,2) + 2*zeta(1,3) - zeta(2,2) - 
     &         zeta(2,3) - 3*zeta(2,4) - 2*zeta(3,4) - 2*zeta(4,4))
     &        *D2345(DD00113) + 
     &      (2*zeta(1,1) + 2*zeta(1,3) + zeta(1,4) - 2*zeta(2,2) - 
     &         2*zeta(2,3) - 3*zeta(2,4) - zeta(3,4) - zeta(4,4))*
     &       D2345(DD00133))
	E(EE1133) = -(dabbr62*eta(1)) - 
     &   4*dabbr47*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &      zeta(3,4)) - eta(3)*D1245(DD1122) - 
     &   eta(5)*D1234(DD1133) + 
     &   4*(-2*dabbr46*(zeta(2,3) + zeta(3,3) + zeta(3,4)) + 
     &      zeta(1,1)*D2345(DD0022) + 
     &      zeta(1,2)*D2345(DD0022) - 
     &      zeta(1,3)*D2345(DD0022) + 
     &      zeta(1,4)*D2345(DD0022) + 
     &      zeta(2,3)*D1245(DD00112) + 
     &      zeta(3,4)*D1234(DD00113) + 
     &      zeta(1,2)*D1245(DD00122) + 
     &      zeta(1,4)*D1234(DD00133) + 
     &      zeta(1,1)*D2345(DD00122) + 
     &      zeta(1,2)*D2345(DD00122) - 
     &      zeta(1,3)*D2345(DD00122) + 
     &      zeta(1,4)*D2345(DD00122) + 
     &      (zeta(1,1) + zeta(1,2) + zeta(1,4) - zeta(2,3) - 
     &         zeta(3,3) - zeta(3,4))*D2345(DD00222) + 
     &      (zeta(1,1) + zeta(1,2) - zeta(1,3) + zeta(1,4))*
     &       D2345(DD00223))
	E(EE1134) = -(dabbr63*eta(1)) - 
     &   2*dabbr66*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &      zeta(3,4)) - 2*dabbr64*
     &    (zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4)) - 
     &   eta(3)*D1245(DD1123) + 
     &   2*(2*dabbr65*(zeta(1,1) + zeta(1,2) - zeta(2,3) - 
     &         zeta(2,4) - zeta(3,3) - 2*zeta(3,4) - zeta(4,4)) + 
     &      zeta(2,4)*D1245(DD00112) + 
     &      zeta(2,3)*D1245(DD00113) + 
     &      zeta(3,3)*D1235(DD00113) + 
     &      zeta(4,4)*D1234(DD00113) + 
     &      2*zeta(1,2)*D1245(DD00123) + 
     &      (2*zeta(1,1) + 2*zeta(1,2) + zeta(1,3) - zeta(2,3) - 
     &         2*zeta(2,4) - zeta(3,3) - 3*zeta(3,4) - 2*zeta(4,4))
     &        *D2345(DD00223) + 
     &      (2*zeta(1,1) + 2*zeta(1,2) + zeta(1,4) - 2*zeta(2,3) - 
     &         zeta(2,4) - 2*zeta(3,3) - 3*zeta(3,4) - zeta(4,4))*
     &       D2345(DD00233))
	E(EE1144) = -(dabbr67*eta(1)) - 
     &   4*dabbr51*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &      zeta(4,4)) - eta(3)*D1245(DD1133) - 
     &   eta(4)*D1235(DD1133) + 
     &   4*(-2*dabbr52*(zeta(2,4) + zeta(3,4) + zeta(4,4)) + 
     &      zeta(1,1)*D2345(DD0033) + 
     &      zeta(1,2)*D2345(DD0033) + 
     &      zeta(1,3)*D2345(DD0033) - 
     &      zeta(1,4)*D2345(DD0033) + 
     &      zeta(2,4)*D1245(DD00113) + 
     &      zeta(3,4)*D1235(DD00113) + 
     &      zeta(1,2)*D1245(DD00133) + 
     &      zeta(1,3)*D1235(DD00133) + 
     &      zeta(1,1)*D2345(DD00133) + 
     &      zeta(1,2)*D2345(DD00133) + 
     &      zeta(1,3)*D2345(DD00133) - 
     &      zeta(1,4)*D2345(DD00133) + 
     &      (zeta(1,1) + zeta(1,2) + zeta(1,3) - zeta(1,4))*
     &       D2345(DD00233) + 
     &      (zeta(1,1) + zeta(1,2) + zeta(1,3) - zeta(2,4) - 
     &         zeta(3,4) - zeta(4,4))*D2345(DD00333))
	E(EE1222) = dabbr68*eta(1) - 
     &   eta(4)*D1235(DD1222) - 
     &   eta(5)*D1234(DD1222) + 
     &   2*(3*dabbr43*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &         zeta(2,4)) + zeta(1,1)*D1345(DD00111) + 
     &      3*zeta(2,3)*D1235(DD00122) + 
     &      3*zeta(2,4)*D1234(DD00122) + 
     &      zeta(1,3)*D1235(DD00222) + 
     &      zeta(1,4)*D1234(DD00222) - 
     &      (zeta(1,1) - 2*zeta(1,2) + zeta(1,3) + zeta(1,4) - 
     &         3*(zeta(2,2) + zeta(2,3) + zeta(2,4)))*
     &       D2345(DD00111))
	E(EE1223) = dabbr69*eta(1) - 
     &   eta(5)*D1234(DD1223) + 
     &   2*(2*dabbr71*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &         zeta(2,4)) + 
     &      dabbr70*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &         zeta(3,4)) + zeta(1,1)*D1345(DD00112) + 
     &      zeta(3,3)*D1235(DD00122) + 
     &      zeta(3,4)*D1234(DD00122) + 
     &      2*zeta(2,4)*D1234(DD00123) + 
     &      zeta(1,4)*D1234(DD00223) + 
     &      (-zeta(1,1) + zeta(1,2) - zeta(1,4) + 2*zeta(2,2) + 
     &         3*zeta(2,3) + 2*zeta(2,4) + zeta(3,3) + zeta(3,4))*
     &       D2345(DD00112))
	E(EE1224) = dabbr72*eta(1) - 
     &   eta(4)*D1235(DD1223) + 
     &   2*(2*dabbr74*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &         zeta(2,4)) + 
     &      dabbr73*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &         zeta(4,4)) + zeta(1,1)*D1345(DD00113) + 
     &      zeta(3,4)*D1235(DD00122) + 
     &      zeta(4,4)*D1234(DD00122) + 
     &      2*zeta(2,3)*D1235(DD00123) + 
     &      zeta(1,3)*D1235(DD00223) + 
     &      (-zeta(1,1) + zeta(1,2) - zeta(1,3) + 2*zeta(2,2) + 
     &         2*zeta(2,3) + 3*zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       D2345(DD00113))
	E(EE1233) = dabbr75*eta(1) - 
     &   eta(5)*D1234(DD1233) + 
     &   2*(dabbr77*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &         zeta(2,4)) + 
     &      2*dabbr76*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &         zeta(3,4)) + zeta(1,1)*D1345(DD00122) + 
     &      zeta(2,2)*D1245(DD00122) + 
     &      2*zeta(3,4)*D1234(DD00123) + 
     &      zeta(2,4)*D1234(DD00133) + 
     &      zeta(1,4)*D1234(DD00233) + 
     &      (-zeta(1,1) + zeta(1,3) - zeta(1,4) + zeta(2,2) + 
     &         3*zeta(2,3) + zeta(2,4) + 2*(zeta(3,3) + zeta(3,4)))
     &        *D2345(DD00122))
	E(EE1234) = dabbr78*eta(1) + 
     &   2*(dabbr81*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &         zeta(2,4)) + 
     &      dabbr80*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &         zeta(3,4)) + 
     &      dabbr79*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &         zeta(4,4)) + zeta(1,1)*D1345(DD00123) + 
     &      zeta(2,2)*D1245(DD00123) + 
     &      zeta(3,3)*D1235(DD00123) + 
     &      zeta(4,4)*D1234(DD00123) + 
     &      (-zeta(1,1) + zeta(2,2) + 2*zeta(2,3) + 2*zeta(2,4) + 
     &         zeta(3,3) + 2*zeta(3,4) + zeta(4,4))*
     &       D2345(DD00123))
	E(EE1244) = dabbr82*eta(1) - 
     &   eta(4)*D1235(DD1233) + 
     &   2*(dabbr84*(zeta(1,2) + zeta(2,2) + zeta(2,3) + 
     &         zeta(2,4)) + 
     &      2*dabbr83*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &         zeta(4,4)) + 2*zeta(3,4)*D1235(DD00123) + 
     &      zeta(1,1)*D1345(DD00133) + 
     &      zeta(2,2)*D1245(DD00133) + 
     &      zeta(2,3)*D1235(DD00133) + 
     &      zeta(1,3)*D1235(DD00233) + 
     &      (-zeta(1,1) - zeta(1,3) + zeta(1,4) + zeta(2,2) + 
     &         zeta(2,3) + 3*zeta(2,4) + 2*(zeta(3,4) + zeta(4,4)))
     &        *D2345(DD00133))
	E(EE1333) = dabbr85*eta(1) - 
     &   eta(3)*D1245(DD1222) - 
     &   eta(5)*D1234(DD1333) + 
     &   2*(3*dabbr46*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &         zeta(3,4)) + 3*zeta(2,3)*D1245(DD00122) + 
     &      3*zeta(3,4)*D1234(DD00133) + 
     &      zeta(1,1)*D1345(DD00222) + 
     &      zeta(1,2)*D1245(DD00222) + 
     &      zeta(1,4)*D1234(DD00333) - 
     &      (zeta(1,1) + zeta(1,2) - 2*zeta(1,3) + zeta(1,4) - 
     &         3*(zeta(2,3) + zeta(3,3) + zeta(3,4)))*
     &       D2345(DD00222))
	E(EE1334) = dabbr86*eta(1) - 
     &   eta(3)*D1245(DD1223) + 
     &   2*(2*dabbr88*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &         zeta(3,4)) + 
     &      dabbr87*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &         zeta(4,4)) + zeta(2,4)*D1245(DD00122) + 
     &      2*zeta(2,3)*D1245(DD00123) + 
     &      zeta(4,4)*D1234(DD00133) + 
     &      zeta(1,1)*D1345(DD00223) + 
     &      zeta(1,2)*D1245(DD00223) + 
     &      (-zeta(1,1) - zeta(1,2) + zeta(1,3) + 2*zeta(2,3) + 
     &         zeta(2,4) + 2*zeta(3,3) + 3*zeta(3,4) + zeta(4,4))*
     &       D2345(DD00223))
	E(EE1344) = dabbr89*eta(1) - 
     &   eta(3)*D1245(DD1233) + 
     &   2*(dabbr91*(zeta(1,3) + zeta(2,3) + zeta(3,3) + 
     &         zeta(3,4)) + 
     &      2*dabbr90*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &         zeta(4,4)) + 2*zeta(2,4)*D1245(DD00123) + 
     &      zeta(2,3)*D1245(DD00133) + 
     &      zeta(3,3)*D1235(DD00133) + 
     &      zeta(1,1)*D1345(DD00233) + 
     &      zeta(1,2)*D1245(DD00233) + 
     &      (-zeta(1,1) - zeta(1,2) + zeta(1,4) + zeta(2,3) + 
     &         2*zeta(2,4) + zeta(3,3) + 3*zeta(3,4) + 2*zeta(4,4))
     &        *D2345(DD00233))
	E(EE1444) = dabbr92*eta(1) - 
     &   eta(3)*D1245(DD1333) - 
     &   eta(4)*D1235(DD1333) + 
     &   2*(3*dabbr52*(zeta(1,4) + zeta(2,4) + zeta(3,4) + 
     &         zeta(4,4)) + 3*zeta(2,4)*D1245(DD00133) + 
     &      3*zeta(3,4)*D1235(DD00133) + 
     &      zeta(1,1)*D1345(DD00333) + 
     &      zeta(1,2)*D1245(DD00333) + 
     &      zeta(1,3)*D1235(DD00333) - 
     &      (zeta(1,1) + zeta(1,2) + zeta(1,3) - 2*zeta(1,4) - 
     &         3*(zeta(2,4) + zeta(3,4) + zeta(4,4)))*
     &       D2345(DD00333))
	E(EE2222) = -(eta(2)*D1345(DD1111)) - 
     &   eta(4)*D1235(DD2222) - 
     &   eta(5)*D1234(DD2222) - 
     &   eta(1)*D2345(DD1111) + 
     &   8*zeta(1,2)*D1345(DD00111) + 
     &   8*zeta(2,3)*D1235(DD00222) + 
     &   8*zeta(2,4)*D1234(DD00222) - 
     &   8*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    D2345(DD00111)
	E(EE2223) = -(eta(2)*D1345(DD1112)) - 
     &   eta(5)*D1234(DD2223) - 
     &   eta(1)*D2345(DD1112) + 
     &   2*zeta(1,3)*D1345(DD00111) + 
     &   6*zeta(1,2)*D1345(DD00112) + 
     &   2*zeta(3,3)*D1235(DD00222) + 
     &   2*zeta(3,4)*D1234(DD00222) + 
     &   6*zeta(2,4)*D1234(DD00223) - 
     &   2*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    D2345(DD00111) - 
     &   6*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    D2345(DD00112)
	E(EE2224) = -(eta(2)*D1345(DD1113)) - 
     &   eta(4)*D1235(DD2223) - 
     &   eta(1)*D2345(DD1113) + 
     &   2*zeta(1,4)*D1345(DD00111) + 
     &   6*zeta(1,2)*D1345(DD00113) + 
     &   2*zeta(3,4)*D1235(DD00222) + 
     &   2*zeta(4,4)*D1234(DD00222) + 
     &   6*zeta(2,3)*D1235(DD00223) - 
     &   2*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    D2345(DD00111) - 
     &   6*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    D2345(DD00113)
	E(EE2233) = -(eta(2)*D1345(DD1122)) - 
     &   eta(5)*D1234(DD2233) - 
     &   eta(1)*D2345(DD1122) + 
     &   4*(zeta(1,3)*D1345(DD00112) + 
     &      zeta(1,2)*D1345(DD00122) + 
     &      zeta(3,4)*D1234(DD00223) + 
     &      zeta(2,4)*D1234(DD00233) - 
     &      (zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &       D2345(DD00112) - 
     &      (zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &       D2345(DD00122))
	E(EE2234) = -(eta(2)*D1345(DD1123)) - 
     &   eta(1)*D2345(DD1123) + 
     &   2*(zeta(1,4)*D1345(DD00112) + 
     &      zeta(1,3)*D1345(DD00113) + 
     &      2*zeta(1,2)*D1345(DD00123) + 
     &      zeta(3,3)*D1235(DD00223) + 
     &      zeta(4,4)*D1234(DD00223) - 
     &      (zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       D2345(DD00112) - 
     &      (zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &       D2345(DD00113) - 
     &      2*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &       D2345(DD00123))
	E(EE2244) = -(eta(2)*D1345(DD1133)) - 
     &   eta(4)*D1235(DD2233) - 
     &   eta(1)*D2345(DD1133) + 
     &   4*(zeta(1,4)*D1345(DD00113) + 
     &      zeta(1,2)*D1345(DD00133) + 
     &      zeta(3,4)*D1235(DD00223) + 
     &      zeta(2,3)*D1235(DD00233) - 
     &      (zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       D2345(DD00113) - 
     &      (zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &       D2345(DD00133))
	E(EE2333) = -(eta(2)*D1345(DD1222)) - 
     &   eta(5)*D1234(DD2333) - 
     &   eta(1)*D2345(DD1222) + 
     &   6*zeta(1,3)*D1345(DD00122) + 
     &   2*zeta(1,2)*D1345(DD00222) + 
     &   2*zeta(2,2)*D1245(DD00222) + 
     &   6*zeta(3,4)*D1234(DD00233) + 
     &   2*zeta(2,4)*D1234(DD00333) - 
     &   6*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    D2345(DD00122) - 
     &   2*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    D2345(DD00222)
	E(EE2334) = -(eta(2)*D1345(DD1223)) - 
     &   eta(1)*D2345(DD1223) + 
     &   2*(zeta(1,4)*D1345(DD00122) + 
     &      2*zeta(1,3)*D1345(DD00123) + 
     &      zeta(1,2)*D1345(DD00223) + 
     &      zeta(2,2)*D1245(DD00223) + 
     &      zeta(4,4)*D1234(DD00233) - 
     &      (zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       D2345(DD00122) - 
     &      2*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &       D2345(DD00123) - 
     &      (zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &       D2345(DD00223))
	E(EE2344) = -(eta(2)*D1345(DD1233)) - 
     &   eta(1)*D2345(DD1233) + 
     &   2*(2*zeta(1,4)*D1345(DD00123) + 
     &      zeta(1,3)*D1345(DD00133) + 
     &      zeta(1,2)*D1345(DD00233) + 
     &      zeta(2,2)*D1245(DD00233) + 
     &      zeta(3,3)*D1235(DD00233) - 
     &      2*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       D2345(DD00123) - 
     &      (zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &       D2345(DD00133) - 
     &      (zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &       D2345(DD00233))
	E(EE2444) = -(eta(2)*D1345(DD1333)) - 
     &   eta(4)*D1235(DD2333) - 
     &   eta(1)*D2345(DD1333) + 
     &   6*zeta(1,4)*D1345(DD00133) + 
     &   6*zeta(3,4)*D1235(DD00233) + 
     &   2*zeta(1,2)*D1345(DD00333) + 
     &   2*zeta(2,2)*D1245(DD00333) + 
     &   2*zeta(2,3)*D1235(DD00333) - 
     &   6*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    D2345(DD00133) - 
     &   2*(zeta(1,2) + zeta(2,2) + zeta(2,3) + zeta(2,4))*
     &    D2345(DD00333)
	E(EE3333) = -(eta(2)*D1345(DD2222)) - 
     &   eta(3)*D1245(DD2222) - 
     &   eta(5)*D1234(DD3333) - 
     &   eta(1)*D2345(DD2222) + 
     &   8*zeta(1,3)*D1345(DD00222) + 
     &   8*zeta(2,3)*D1245(DD00222) + 
     &   8*zeta(3,4)*D1234(DD00333) - 
     &   8*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    D2345(DD00222)
	E(EE3334) = -(eta(2)*D1345(DD2223)) - 
     &   eta(3)*D1245(DD2223) - 
     &   eta(1)*D2345(DD2223) + 
     &   2*zeta(1,4)*D1345(DD00222) + 
     &   2*zeta(2,4)*D1245(DD00222) + 
     &   6*zeta(1,3)*D1345(DD00223) + 
     &   6*zeta(2,3)*D1245(DD00223) + 
     &   2*zeta(4,4)*D1234(DD00333) - 
     &   2*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    D2345(DD00222) - 
     &   6*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    D2345(DD00223)
	E(EE3344) = -(eta(2)*D1345(DD2233)) - 
     &   eta(3)*D1245(DD2233) - 
     &   eta(1)*D2345(DD2233) + 
     &   4*(zeta(1,4)*D1345(DD00223) + 
     &      zeta(2,4)*D1245(DD00223) + 
     &      zeta(1,3)*D1345(DD00233) + 
     &      zeta(2,3)*D1245(DD00233) - 
     &      (zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &       D2345(DD00223) - 
     &      (zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &       D2345(DD00233))
	E(EE3444) = -(eta(2)*D1345(DD2333)) - 
     &   eta(3)*D1245(DD2333) - 
     &   eta(1)*D2345(DD2333) + 
     &   6*zeta(1,4)*D1345(DD00233) + 
     &   6*zeta(2,4)*D1245(DD00233) + 
     &   2*zeta(1,3)*D1345(DD00333) + 
     &   2*zeta(2,3)*D1245(DD00333) + 
     &   2*zeta(3,3)*D1235(DD00333) - 
     &   6*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    D2345(DD00233) - 
     &   2*(zeta(1,3) + zeta(2,3) + zeta(3,3) + zeta(3,4))*
     &    D2345(DD00333)
	E(EE4444) = -(eta(2)*D1345(DD3333)) - 
     &   eta(3)*D1245(DD3333) - 
     &   eta(4)*D1235(DD3333) - 
     &   eta(1)*D2345(DD3333) + 
     &   8*zeta(1,4)*D1345(DD00333) + 
     &   8*zeta(2,4)*D1245(DD00333) + 
     &   8*zeta(3,4)*D1235(DD00333) - 
     &   8*(zeta(1,4) + zeta(2,4) + zeta(3,4) + zeta(4,4))*
     &    D2345(DD00333)

	if( dump ) call XDumpCoeff(5, E)
	end

