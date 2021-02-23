* Eget.F
* retrieve the five-point tensor coefficients
* this file is part of LoopTools
* written by M. Rauch
* last modified 9 Apr 18 th

#include "externals.h"
#include "types.h"

#define npoint 5
#include "defs.h"


	subroutine XEpara(para, p1, p2, p3, p4, p5, 
     &    p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5)
	implicit none
	ArgType para(1,*)
	ArgType p1, p2, p3, p4, p5
	ArgType p1p2, p2p3, p3p4, p4p5, p5p1
	ArgType m1, m2, m3, m4, m5

#include "lt.h"

	P(1) = p1
	P(2) = p2
	P(3) = p3
	P(4) = p4
	P(5) = p5
	P(6) = p1p2
	P(7) = p2p3
	P(8) = p3p4
	P(9) = p4p5
	P(10) = p5p1

	M(1) = m1
	if( abs(M(1)) .lt. minmass ) M(1) = 0
	M(2) = m2
	if( abs(M(2)) .lt. minmass ) M(2) = 0
	M(3) = m3
	if( abs(M(3)) .lt. minmass ) M(3) = 0
	M(4) = m4
	if( abs(M(4)) .lt. minmass ) M(4) = 0
	M(5) = m5
	if( abs(M(5)) .lt. minmass ) M(5) = 0
	end

************************************************************************

	memindex function XEget(p1, p2, p3, p4, p5, 
     &    p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5)
	implicit none
	ArgType p1, p2, p3, p4, p5
	ArgType p1p2, p2p3, p3p4, p4p5, p5p1
	ArgType m1, m2, m3, m4, m5

#include "lt.h"

	memindex cacheindex
	external cacheindex, XEcoefx

#ifdef COMPLEXPARA
	memindex Eget
	external Eget
#endif

	ArgType para(1,Pee)

#ifdef COMPLEXPARA
	if( abs(Im(p1)) + abs(Im(p2)) +
     &      abs(Im(p3)) + abs(Im(p4)) +
     &      abs(Im(p5)) + abs(Im(p1p2)) +
     &      abs(Im(p2p3)) + abs(Im(p3p4)) +
     &      abs(Im(p4p5)) + abs(Im(p5p1)) .gt. 0 )
     &    print *, "Complex momenta not implemented"
	if( abs(Im(m1)) + abs(Im(m2)) +
     &      abs(Im(m3)) + abs(Im(m4)) + abs(Im(m5)) .eq. 0 ) then
	  XEget = Eget(p1, p2, p3, p4, p1p2, p2p3,
     &      m1, m2, m3, m4) - offsetC
	  return
	endif
#endif

	call XEpara(para, p1, p2, p3, p4, p5,
     &    p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5)
	XEget = cacheindex(para, Eval(1,0), XEcoefx, RC*Pee, Nee, Eno)
	end

************************************************************************

	subroutine XEput(res, p1, p2, p3, p4, p5, 
     &    p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5)
	implicit none
	ComplexType res(*)
	ArgType p1, p2, p3, p4, p5
	ArgType p1p2, p2p3, p3p4, p4p5, p5p1
	ArgType m1, m2, m3, m4, m5

#include "lt.h"

	external XEcoefx

	ArgType para(1,Pee)

#ifdef COMPLEXPARA
	if( abs(Im(p1)) + abs(Im(p2)) +
     &      abs(Im(p3)) + abs(Im(p4)) +
     &      abs(Im(p5)) + abs(Im(p1p2)) +
     &      abs(Im(p2p3)) + abs(Im(p3p4)) +
     &      abs(Im(p4p5)) + abs(Im(p5p1)) .gt. 0 )
     &    print *, "Complex momenta not implemented"
	if( abs(Im(m1)) + abs(Im(m2)) +
     &      abs(Im(m3)) + abs(Im(m4)) + abs(Im(m5)) .eq. 0 ) then
	  call Eput(res, p1, p2, p3, p4, p1p2, p2p3,
     &      m1, m2, m3, m4)
	  return
	endif
#endif

	call XEpara(para, p1, p2, p3, p4, p5,
     &    p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5)
	call cachecopy(res, para, Eval(1,0), XEcoefx, RC*Pee, Nee, Eno)
	end

************************************************************************

	subroutine XE0nocache(res, p1, p2, p3, p4, p5, 
     &    p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5)
	implicit none
	ComplexType res(*)
	ArgType p1, p2, p3, p4, p5
	ArgType p1p2, p2p3, p3p4, p4p5, p5p1
	ArgType m1, m2, m3, m4, m5

#include "lt.h"

	ArgType para(1,Pee)
	ComplexType D2345(0:2), D1345(0:2), D1245(0:2)
	ComplexType D1235(0:2), D1234(0:2)

#ifdef COMPLEXPARA
	if( abs(Im(p1)) + abs(Im(p2)) +
     &      abs(Im(p3)) + abs(Im(p4)) +
     &      abs(Im(p5)) + abs(Im(p1p2)) +
     &      abs(Im(p2p3)) + abs(Im(p3p4)) +
     &      abs(Im(p4p5)) + abs(Im(p5p1)) .gt. 0 )
     &    print *, "Complex momenta not implemented"
	if( abs(Im(m1)) + abs(Im(m2)) +
     &      abs(Im(m3)) + abs(Im(m4)) + abs(Im(m5)) .eq. 0 ) then
	  call E0nocache(res, p1, p2, p3, p4, p1p2, p2p3,
     &      m1, m2, m3, m4)
	  return
	endif
#endif

	call XEpara(para, p1, p2, p3, p4, p5,
     &    p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5)
	call XE0func(res, para, D2345, D1345, D1245, D1235, D1234, 0)
	end

************************************************************************

	ComplexType function XE0i(i, p1, p2, p3, p4, p5,
     &    p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5)
	implicit none
	integer i
	ArgType p1, p2, p3, p4, p5
	ArgType p1p2, p2p3, p3p4, p4p5, p5p1
	ArgType m1, m2, m3, m4, m5

#include "lt.h"

	memindex XEget
	external XEget

	memindex b

	b = XEget(p1, p2, p3, p4, p5,
     &    p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5)
	XE0i = Eval(i+epsi,b)
	end

************************************************************************

	ComplexType function XE0(p1, p2, p3, p4, p5,
     &    p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5)
	implicit none
	ArgType p1, p2, p3, p4, p5
	ArgType p1p2, p2p3, p3p4, p4p5, p5p1
	ArgType m1, m2, m3, m4, m5

#include "lt.h"

	ComplexType XE0i
	external XE0i

	XE0 = XE0i(ee0, p1, p2, p3, p4, p5,
     &    p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5)
	end

************************************************************************

	subroutine XEcoefx(res, para)
	implicit none
	ComplexType res(*)
	ArgType para(1,*)

#include "lt.h"

	memindex d2345, d1345, d1245, d1235, d1234

	memindex XDget
	external XDget

	d2345 = XDget(P(2), P(3), P(4), P(10), P(7), P(8),
     &    M(2), M(3), M(4), M(5))
	d1345 = XDget(P(6), P(3), P(4), P(5), P(9), P(8),
     &    M(1), M(3), M(4), M(5))
	d1245 = XDget(P(1), P(7), P(4), P(5), P(9), P(10),
     &    M(1), M(2), M(4), M(5))
	d1235 = XDget(P(1), P(2), P(8), P(5), P(6), P(10),
     &    M(1), M(2), M(3), M(5))
	d1234 = XDget(P(1), P(2), P(3), P(9), P(6), P(7),
     &    M(1), M(2), M(3), M(4))

	call XEcoeff(res, para, Dval(1,d2345), Dval(1,d1345),
     &    Dval(1,d1245), Dval(1,d1235), Dval(1,d1234))
	end

************************************************************************

	subroutine XEcoeff(res, para, D2345, D1345, D1245, D1235, D1234)
	implicit none
	ComplexType res(*)
	ComplexType D2345(*), D1345(*), D1245(*), D1235(*), D1234(*)
	ArgType para(1,*)

#include "lt.h"

	ComplexType cmp(Nee)

#ifdef COMPLEXPARA
	goto (1, 2, 3) ibits(versionkey, KeyEgetC, 2)
#else
	goto (1, 2, 3) ibits(versionkey, KeyEget, 2)
#endif

	call XEcoeffa(res, para, D2345, D1345, D1245, D1235, D1234)
	return

1	call XEcoeffb(res, para, D2345, D1345, D1245, D1235, D1234)
	return

2	call XEcoeffa(res, para, D2345, D1345, D1245, D1235, D1234)
	call XEcoeffb(cmp, para, D2345, D1345, D1245, D1235, D1234)
	call XEcheck(res, cmp, para)
	return

3	call XEcoeffa(cmp, para, D2345, D1345, D1245, D1235, D1234)
	call XEcoeffb(res, para, D2345, D1345, D1245, D1235, D1234)
	call XEcheck(cmp, res, para)
	end

************************************************************************

	subroutine XEcheck(Ea, Eb, para)
	implicit none
	ComplexType Ea(*), Eb(*)
	ArgType para(1,*)

#include "lt.h"
#include "ltnames.h"

	ComplexType Ecmp(Nee)
	integer i
	logical ini

	ArgQuad Ginv(4,4)
	common /XInvGramE/ Ginv

	Ecmp(EE0) = Ea(EE0)
	Ecmp(EE1) = Ea(EE1)
	Ecmp(EE2) = Ea(EE2)
	Ecmp(EE3) = Ea(EE3)
	Ecmp(EE4) = Ea(EE4)
	Ecmp(EE11) = Ea(EE11) + 2*Ea(EE00)*Ginv(1,1)
	Ecmp(EE12) = Ea(EE12) + 2*Ea(EE00)*Ginv(1,2)
	Ecmp(EE13) = Ea(EE13) + 2*Ea(EE00)*Ginv(1,3)
	Ecmp(EE14) = Ea(EE14) + 2*Ea(EE00)*Ginv(1,4)
	Ecmp(EE22) = Ea(EE22) + 2*Ea(EE00)*Ginv(2,2)
	Ecmp(EE23) = Ea(EE23) + 2*Ea(EE00)*Ginv(2,3)
	Ecmp(EE24) = Ea(EE24) + 2*Ea(EE00)*Ginv(2,4)
	Ecmp(EE33) = Ea(EE33) + 2*Ea(EE00)*Ginv(3,3)
	Ecmp(EE34) = Ea(EE34) + 2*Ea(EE00)*Ginv(3,4)
	Ecmp(EE44) = Ea(EE44) + 2*Ea(EE00)*Ginv(4,4)
	Ecmp(EE00) = 0
	Ecmp(EE111) = Ea(EE111) + 6*Ea(EE001)*Ginv(1,1)
	Ecmp(EE112) = Ea(EE112) + 
     &    2*(Ea(EE002)*Ginv(1,1) + 
     &       Ea(EE001)*(Ginv(1,2) + Ginv(2,1)))
	Ecmp(EE113) = Ea(EE113) + 
     &    2*(Ea(EE003)*Ginv(1,1) + 
     &       Ea(EE001)*(Ginv(1,3) + Ginv(3,1)))
	Ecmp(EE114) = Ea(EE114) + 
     &    2*(Ea(EE004)*Ginv(1,1) + 
     &       Ea(EE001)*(Ginv(1,4) + Ginv(4,1)))
	Ecmp(EE122) = Ea(EE122) + 
     &    2*(Ea(EE002)*(Ginv(1,2) + Ginv(2,1)) + 
     &       Ea(EE001)*Ginv(2,2))
	Ecmp(EE123) = Ea(EE123) + 
     &    2*(Ea(EE003)*Ginv(1,2) + Ea(EE001)*Ginv(2,3) + 
     &       Ea(EE002)*Ginv(3,1))
	Ecmp(EE124) = Ea(EE124) + 
     &    2*(Ea(EE004)*Ginv(1,2) + Ea(EE001)*Ginv(2,4) + 
     &       Ea(EE002)*Ginv(4,1))
	Ecmp(EE133) = Ea(EE133) + 
     &    2*(Ea(EE003)*(Ginv(1,3) + Ginv(3,1)) + 
     &       Ea(EE001)*Ginv(3,3))
	Ecmp(EE134) = Ea(EE134) + 
     &    2*(Ea(EE004)*Ginv(1,3) + Ea(EE001)*Ginv(3,4) + 
     &       Ea(EE003)*Ginv(4,1))
	Ecmp(EE144) = Ea(EE144) + 
     &    2*(Ea(EE004)*(Ginv(1,4) + Ginv(4,1)) + 
     &       Ea(EE001)*Ginv(4,4))
	Ecmp(EE222) = Ea(EE222) + 6*Ea(EE002)*Ginv(2,2)
	Ecmp(EE223) = Ea(EE223) + 
     &    2*(Ea(EE003)*Ginv(2,2) + 
     &       Ea(EE002)*(Ginv(2,3) + Ginv(3,2)))
	Ecmp(EE224) = Ea(EE224) + 
     &    2*(Ea(EE004)*Ginv(2,2) + 
     &       Ea(EE002)*(Ginv(2,4) + Ginv(4,2)))
	Ecmp(EE233) = Ea(EE233) + 
     &    2*(Ea(EE003)*(Ginv(2,3) + Ginv(3,2)) + 
     &       Ea(EE002)*Ginv(3,3))
	Ecmp(EE234) = Ea(EE234) + 
     &    2*(Ea(EE004)*Ginv(2,3) + Ea(EE002)*Ginv(3,4) + 
     &       Ea(EE003)*Ginv(4,2))
	Ecmp(EE244) = Ea(EE244) + 
     &    2*(Ea(EE004)*(Ginv(2,4) + Ginv(4,2)) + 
     &       Ea(EE002)*Ginv(4,4))
	Ecmp(EE333) = Ea(EE333) + 6*Ea(EE003)*Ginv(3,3)
	Ecmp(EE334) = Ea(EE334) + 
     &    2*(Ea(EE004)*Ginv(3,3) + 
     &       Ea(EE003)*(Ginv(3,4) + Ginv(4,3)))
	Ecmp(EE344) = Ea(EE344) + 
     &    2*(Ea(EE004)*(Ginv(3,4) + Ginv(4,3)) + 
     &       Ea(EE003)*Ginv(4,4))
	Ecmp(EE444) = Ea(EE444) + 6*Ea(EE004)*Ginv(4,4)
	Ecmp(EE001) = 0
	Ecmp(EE002) = 0
	Ecmp(EE003) = 0
	Ecmp(EE004) = 0
	Ecmp(EE1111) = 
     &   Ea(EE1111) + 12*Ginv(1,1)*
     &     (Ea(EE0011) + Ea(EE0000)*Ginv(1,1))
	Ecmp(EE1112) = 
     &   Ea(EE1112) + 6*(Ea(EE0012)*Ginv(1,1) + 
     &       (Ea(EE0011) + 2*Ea(EE0000)*Ginv(1,1))*Ginv(1,2))
	Ecmp(EE1113) = 
     &   Ea(EE1113) + 6*(Ea(EE0013)*Ginv(1,1) + 
     &       (Ea(EE0011) + 2*Ea(EE0000)*Ginv(1,1))*Ginv(1,3))
	Ecmp(EE1114) = 
     &   Ea(EE1114) + 6*(Ea(EE0014)*Ginv(1,1) + 
     &       (Ea(EE0011) + 2*Ea(EE0000)*Ginv(1,1))*Ginv(1,4))
	Ecmp(EE1122) = 
     &   Ea(EE1122) + 2*(Ea(EE0022)*Ginv(1,1) + 
     &       4*Ginv(1,2)*(Ea(EE0012) + Ea(EE0000)*Ginv(1,2)) + 
     &       (Ea(EE0011) + 2*Ea(EE0000)*Ginv(1,1))*Ginv(2,2))
	Ecmp(EE1123) = 
     &   Ea(EE1123) + 2*(Ea(EE0023)*Ginv(1,1) + 
     &       2*(Ea(EE0013)*Ginv(1,2) + 
     &          (Ea(EE0012) + 2*Ea(EE0000)*Ginv(1,2))*Ginv(1,3)) + 
     &       (Ea(EE0011) + 2*Ea(EE0000)*Ginv(1,1))*Ginv(2,3))
	Ecmp(EE1124) = 
     &   Ea(EE1124) + 2*(Ea(EE0024)*Ginv(1,1) + 
     &       2*(Ea(EE0014)*Ginv(1,2) + 
     &          (Ea(EE0012) + 2*Ea(EE0000)*Ginv(1,2))*Ginv(1,4)) + 
     &       (Ea(EE0011) + 2*Ea(EE0000)*Ginv(1,1))*Ginv(2,4))
	Ecmp(EE1133) = 
     &   Ea(EE1133) + 2*(Ea(EE0033)*Ginv(1,1) + 
     &       4*Ginv(1,3)*(Ea(EE0013) + Ea(EE0000)*Ginv(1,3)) + 
     &       (Ea(EE0011) + 2*Ea(EE0000)*Ginv(1,1))*Ginv(3,3))
	Ecmp(EE1134) = 
     &   Ea(EE1134) + 2*(Ea(EE0034)*Ginv(1,1) + 
     &       2*(Ea(EE0014)*Ginv(1,3) + 
     &          (Ea(EE0013) + 2*Ea(EE0000)*Ginv(1,3))*Ginv(1,4)) + 
     &       (Ea(EE0011) + 2*Ea(EE0000)*Ginv(1,1))*Ginv(3,4))
	Ecmp(EE1144) = 
     &   Ea(EE1144) + 2*(Ea(EE0044)*Ginv(1,1) + 
     &       4*Ginv(1,4)*(Ea(EE0014) + Ea(EE0000)*Ginv(1,4)) + 
     &       (Ea(EE0011) + 2*Ea(EE0000)*Ginv(1,1))*Ginv(4,4))
	Ecmp(EE1222) = 
     &   Ea(EE1222) + 6*(Ea(EE0022)*Ginv(1,2) + 
     &       (Ea(EE0012) + 2*Ea(EE0000)*Ginv(1,2))*Ginv(2,2))
	Ecmp(EE1223) = 
     &   Ea(EE1223) + 2*(Ea(EE0022)*Ginv(1,3) + 
     &       (Ea(EE0013) + 2*Ea(EE0000)*Ginv(1,3))*Ginv(2,2) + 
     &       2*(Ea(EE0023)*Ginv(1,2) + 
     &          (Ea(EE0012) + 2*Ea(EE0000)*Ginv(1,2))*Ginv(2,3)))
	Ecmp(EE1224) = 
     &   Ea(EE1224) + 2*(Ea(EE0022)*Ginv(1,4) + 
     &       (Ea(EE0014) + 2*Ea(EE0000)*Ginv(1,4))*Ginv(2,2) + 
     &       2*(Ea(EE0024)*Ginv(1,2) + 
     &          (Ea(EE0012) + 2*Ea(EE0000)*Ginv(1,2))*Ginv(2,4)))
	Ecmp(EE1233) = 
     &   Ea(EE1233) + 2*(Ea(EE0033)*Ginv(1,2) + 
     &       2*(Ea(EE0023)*Ginv(1,3) + 
     &          (Ea(EE0013) + 2*Ea(EE0000)*Ginv(1,3))*Ginv(2,3)) + 
     &       (Ea(EE0012) + 2*Ea(EE0000)*Ginv(1,2))*Ginv(3,3))
	Ecmp(EE1234) = 
     &   Ea(EE1234) + 2*(Ea(EE0023)*Ginv(1,4) + 
     &       (Ea(EE0014) + 2*Ea(EE0000)*Ginv(1,4))*Ginv(2,3) + 
     &       Ea(EE0013)*Ginv(2,4) + 
     &       Ginv(1,3)*(Ea(EE0024) + 2*Ea(EE0000)*Ginv(2,4)) + 
     &       Ea(EE0012)*Ginv(3,4) + 
     &       Ginv(1,2)*(Ea(EE0034) + 2*Ea(EE0000)*Ginv(3,4)))
	Ecmp(EE1244) = 
     &   Ea(EE1244) + 2*(Ea(EE0044)*Ginv(1,2) + 
     &       2*(Ea(EE0024)*Ginv(1,4) + 
     &          (Ea(EE0014) + 2*Ea(EE0000)*Ginv(1,4))*Ginv(2,4)) + 
     &       (Ea(EE0012) + 2*Ea(EE0000)*Ginv(1,2))*Ginv(4,4))
	Ecmp(EE1333) = 
     &   Ea(EE1333) + 6*(Ea(EE0033)*Ginv(1,3) + 
     &       (Ea(EE0013) + 2*Ea(EE0000)*Ginv(1,3))*Ginv(3,3))
	Ecmp(EE1334) = 
     &   Ea(EE1334) + 2*(Ea(EE0033)*Ginv(1,4) + 
     &       (Ea(EE0014) + 2*Ea(EE0000)*Ginv(1,4))*Ginv(3,3) + 
     &       2*(Ea(EE0034)*Ginv(1,3) + 
     &          (Ea(EE0013) + 2*Ea(EE0000)*Ginv(1,3))*Ginv(3,4)))
	Ecmp(EE1344) = 
     &   Ea(EE1344) + 2*(Ea(EE0044)*Ginv(1,3) + 
     &       2*(Ea(EE0034)*Ginv(1,4) + 
     &          (Ea(EE0014) + 2*Ea(EE0000)*Ginv(1,4))*Ginv(3,4)) + 
     &       (Ea(EE0013) + 2*Ea(EE0000)*Ginv(1,3))*Ginv(4,4))
	Ecmp(EE1444) = 
     &   Ea(EE1444) + 6*(Ea(EE0044)*Ginv(1,4) + 
     &       (Ea(EE0014) + 2*Ea(EE0000)*Ginv(1,4))*Ginv(4,4))
	Ecmp(EE2222) = 
     &   Ea(EE2222) + 12*Ginv(2,2)*
     &     (Ea(EE0022) + Ea(EE0000)*Ginv(2,2))
	Ecmp(EE2223) = 
     &   Ea(EE2223) + 6*(Ea(EE0023)*Ginv(2,2) + 
     &       (Ea(EE0022) + 2*Ea(EE0000)*Ginv(2,2))*Ginv(2,3))
	Ecmp(EE2224) = 
     &   Ea(EE2224) + 6*(Ea(EE0024)*Ginv(2,2) + 
     &       (Ea(EE0022) + 2*Ea(EE0000)*Ginv(2,2))*Ginv(2,4))
	Ecmp(EE2233) = 
     &   Ea(EE2233) + 2*(Ea(EE0033)*Ginv(2,2) + 
     &       4*Ginv(2,3)*(Ea(EE0023) + Ea(EE0000)*Ginv(2,3)) + 
     &       (Ea(EE0022) + 2*Ea(EE0000)*Ginv(2,2))*Ginv(3,3))
	Ecmp(EE2234) = 
     &   Ea(EE2234) + 2*(Ea(EE0034)*Ginv(2,2) + 
     &       2*(Ea(EE0024)*Ginv(2,3) + 
     &          (Ea(EE0023) + 2*Ea(EE0000)*Ginv(2,3))*Ginv(2,4)) + 
     &       (Ea(EE0022) + 2*Ea(EE0000)*Ginv(2,2))*Ginv(3,4))
	Ecmp(EE2244) = 
     &   Ea(EE2244) + 2*(Ea(EE0044)*Ginv(2,2) + 
     &       4*Ginv(2,4)*(Ea(EE0024) + Ea(EE0000)*Ginv(2,4)) + 
     &       (Ea(EE0022) + 2*Ea(EE0000)*Ginv(2,2))*Ginv(4,4))
	Ecmp(EE2333) = 
     &   Ea(EE2333) + 6*(Ea(EE0033)*Ginv(2,3) + 
     &       (Ea(EE0023) + 2*Ea(EE0000)*Ginv(2,3))*Ginv(3,3))
	Ecmp(EE2334) = 
     &   Ea(EE2334) + 2*(Ea(EE0033)*Ginv(2,4) + 
     &       (Ea(EE0024) + 2*Ea(EE0000)*Ginv(2,4))*Ginv(3,3) + 
     &       2*(Ea(EE0034)*Ginv(2,3) + 
     &          (Ea(EE0023) + 2*Ea(EE0000)*Ginv(2,3))*Ginv(3,4)))
	Ecmp(EE2344) = 
     &   Ea(EE2344) + 2*(Ea(EE0044)*Ginv(2,3) + 
     &       2*(Ea(EE0034)*Ginv(2,4) + 
     &          (Ea(EE0024) + 2*Ea(EE0000)*Ginv(2,4))*Ginv(3,4)) + 
     &       (Ea(EE0023) + 2*Ea(EE0000)*Ginv(2,3))*Ginv(4,4))
	Ecmp(EE2444) = 
     &   Ea(EE2444) + 6*(Ea(EE0044)*Ginv(2,4) + 
     &       (Ea(EE0024) + 2*Ea(EE0000)*Ginv(2,4))*Ginv(4,4))
	Ecmp(EE3333) = 
     &   Ea(EE3333) + 12*Ginv(3,3)*
     &     (Ea(EE0033) + Ea(EE0000)*Ginv(3,3))
	Ecmp(EE3334) = 
     &   Ea(EE3334) + 6*(Ea(EE0034)*Ginv(3,3) + 
     &       (Ea(EE0033) + 2*Ea(EE0000)*Ginv(3,3))*Ginv(3,4))
	Ecmp(EE3344) = 
     &   Ea(EE3344) + 2*(Ea(EE0044)*Ginv(3,3) + 
     &       4*Ginv(3,4)*(Ea(EE0034) + Ea(EE0000)*Ginv(3,4)) + 
     &       (Ea(EE0033) + 2*Ea(EE0000)*Ginv(3,3))*Ginv(4,4))
	Ecmp(EE3444) = 
     &   Ea(EE3444) + 6*(Ea(EE0044)*Ginv(3,4) + 
     &       (Ea(EE0034) + 2*Ea(EE0000)*Ginv(3,4))*Ginv(4,4))
	Ecmp(EE4444) = 
     &   Ea(EE4444) + 12*Ginv(4,4)*
     &     (Ea(EE0044) + Ea(EE0000)*Ginv(4,4))
	Ecmp(EE0000) = 0
	Ecmp(EE0011) = 0
	Ecmp(EE0012) = 0
	Ecmp(EE0013) = 0
	Ecmp(EE0014) = 0
	Ecmp(EE0022) = 0
	Ecmp(EE0023) = 0
	Ecmp(EE0024) = 0
	Ecmp(EE0033) = 0
	Ecmp(EE0034) = 0
	Ecmp(EE0044) = 0

	ini = .TRUE.
	do i = 1, Nee
	  if( abs(Ecmp(i) - Eb(i)) .gt.
     &        .5D0*maxdev*abs(Ecmp(i) + Eb(i)) ) then
	    if( ini ) then
#ifdef COMPLEXPARA
	      print *, "Discrepancy in EgetC:"
#else
	      print *, "Discrepancy in Eget:"
#endif
	      call XDumpPara(5, para, " ")
	      ini = .FALSE.
	    endif
	    print *, coeffname(i,5), " a =", Ecmp(i)
	    print *, coeffname(i,5), " b =", Eb(i)
	  endif
	enddo
	end

