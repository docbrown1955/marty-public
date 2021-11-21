* auxCD.F
* auxillary functions used by the three- and four-point integrals
* these functions are adapted from Ansgar Denner's bcanew.f
* to the conventions of LoopTools;
* they are used for double-checking the results of FF
* last modified 1 Sep 14 th

#include "externals.h"
#include "types.h"
#include "defs.h"


	ComplexType function lndiv0(x, y)
	implicit none
	RealType x, y

#include "ff.h"

	RealType den

	den = 1 - x/y
	if( abs(den) .lt. 1D-7 ) then
	  lndiv0 = -1 - den*(.5D0 + den/3D0)
	else
	  lndiv0 = lnrat(x, y)/den
	endif
	end

************************************************************************

	ComplexType function lndiv1(x, y)
	implicit none
	RealType x, y

#include "ff.h"

	RealType den

	den = 1 - x/y
	if( abs(den) .lt. 1D-7 ) then
	  lndiv1 = -.5D0 - den/3D0*(1 + .75D0*den)
	else
	  lndiv1 = (lnrat(x, y)/den + 1)/den
	endif
	end

************************************************************************
* Li2omrat(x, y) = Li2(1 - (x - i eps)/(y - i eps)) for real x and y
* hence arguments are typically negative invariants
* original version by R.K. Ellis

	ComplexType function Li2omrat(x, y)
	implicit none
	RealType x, y

#include "ff.h"

	ComplexType spence
	external spence

	ComplexType omarg

	omarg = x/y
	if( Re(omarg) .lt. 0 ) then
	  Li2omrat = pi6 - spence(0, omarg, 0D0) -
     &      log(1 - omarg)*lnrat(x, y)
	else
	  Li2omrat = spence(1, omarg, 0D0)
	endif
	end

************************************************************************

	ComplexType function cLi2omrat(x, y)
	implicit none
	ComplexType x, y

#include "ff.h"

	ComplexType spence
	external spence

	ComplexType omarg

	omarg = x/y
	if( Im(omarg) .eq. 0 .and. Re(omarg) .lt. 0 ) then
	  cLi2omrat = pi6 - spence(0, omarg, 0D0) -
     &      log(1 - Re(omarg))*lnrat(Re(x), Re(y))
	else
	  cLi2omrat = spence(1, omarg, 0D0)
	endif
	end

************************************************************************
* Li2omx2 = Li2(1 - (z1 + i eps1) (z2 + i eps2)) for complex z1, z2
* for z1 z2 < 1: +Li2(1 - z1 z2)
* for z1 z2 > 1: -Li2(1 - 1/(z1 z2)) - 1/2 (log(z1) + log(z2))^2
* original version by R.K. Ellis

	ComplexType function Li2omx2(z1, s1, z2, s2)
	implicit none
	ComplexType z1, z2
	RealType s1, s2

#include "ff.h"

	ComplexType spence
	external spence

	ComplexType z12, l12
	RealType s12

	z12 = z1*z2
	if( abs(z12) .lt. zeroeps ) then
	  Li2omx2 = 0
	else if( abs(z12 - 1) .eq. diffeps ) then
	  Li2omx2 = pi6
	else
	  l12 = ln(z1, s1) + ln(z2, s2)
	  s12 = sign(1D0, Re(z2))*s1 + sign(1D0, Re(z1))*s2
	  if( abs(z12) .le. 1 ) then
	    Li2omx2 = pi6 - spence(0, z12, s12) -
     &        l12*ln(1 - z12, -s12)
	  else
	    z12 = 1/z12
	    Li2omx2 = -pi6 + spence(0, z12, s12) -
     &        l12*(ln(1 - z12, -s12) + .5D0*l12)
	  endif
	endif
	end

************************************************************************
* Li2omx3 = Li2(1 - (z1 + i eps1) (z2 + i eps2)) for complex z1, z2
* for z1 z2 < 1: +Li2(1 - z1 z2)
* for z1 z2 > 1: -Li2(1 - 1/(z1 z2)) - 1/2 (log(z1) + log(z2))^2
* original version by R.K. Ellis

	ComplexType function Li2omx3(z1, s1, z2, s2, z3, s3)
	implicit none
	ComplexType z1, z2, z3
	RealType s1, s2, s3

#include "ff.h"

	ComplexType spence
	external spence

	ComplexType z123, l123
	RealType s123

	z123 = z1*z2*z3
	if( abs(Im(z123)) .lt. zeroeps )
     &    s123 = sign(1D0,
     &      Re(z2*z3)*s1 + Re(z1*z3)*s2 + Re(z1*z2)*s3)

	if( abs(z123) .le. 1 ) then
	  Li2omx3 = pi6 - spence(0, z123, s123)
	  if( abs(z123) .gt. zeroeps .and. abs(z123 - 1) .gt. diffeps )
     &      Li2omx3 = Li2omx3 - ln(1 - z123, 0D0)*
     &        (ln(z1, s1) + ln(z2, s2) + ln(z3, s3))
	else
	  z123 = 1/z123
	  l123 = ln(z1, s1) + ln(z2, s2) + ln(z3, s3)
	  Li2omx3 = -pi6 + spence(0, z123, s123) -
     &      l123*(.5D0*l123 + ln(1 - z123, 0D0))
	endif
	end

************************************************************************
* Li2omrat2 = Li2(1 - (n1 - i eps) (n2 - i eps)/(d1 - i eps)/(d2 - i eps))
* for real n1, n2, d1, d2
* original version by R.K. Ellis

	ComplexType function Li2omrat2(n1, d1, n2, d2)
	implicit none
	RealType n1, d1, n2, d2

#include "ff.h"

	ComplexType spence
	external spence

	RealType r12
	ComplexType l12

	r12 = n1*n2/(d1*d2)
	if( r12 .lt. 1 ) then
	  Li2omrat2 = pi6 - spence(0, ToComplex(r12), 0D0)
	  if( abs(r12*(1 - r12)) .gt. diffeps )
     &      Li2omrat2 = Li2omrat2 -
     &        (lnrat(n1, d1) + lnrat(n2, d2))*log(1 - r12)
	else
	  r12 = 1/r12
	  l12 = lnrat(n1, d1) + lnrat(n2, d2)
	  Li2omrat2 = -pi6 + spence(0, ToComplex(r12), 0D0) -
     &      l12*(.5D0*l12 + log(1 - r12))
	endif
	end

************************************************************************

	ComplexType function cLi2omrat2(n1, d1, n2, d2)
	implicit none
	ComplexType n1, d1, n2, d2

#include "ff.h"

	ComplexType spence
	external spence

	ComplexType r12, l12

	r12 = n1*n2/(d1*d2)
	if( abs(r12) .lt. 1 ) then
	  cLi2omrat2 = pi6 - spence(0, r12, 0D0)
	  if( abs(r12*(1 - r12)) .gt. diffeps )
     &      cLi2omrat2 = cLi2omrat2 -
     &        (lnrat(n1, d1) + lnrat(n2, d2))*log(1 - r12)
	else
	  r12 = 1/r12
	  l12 = lnrat(n1, d1) + lnrat(n2, d2)
	  cLi2omrat2 = -pi6 + spence(0, r12, 0D0) -
     &      l12*(.5D0*l12 + log(1 - r12))
	endif
	end

************************************************************************
* original version by R.K. Ellis

	ComplexType function Li2rat(r1, s1, r2, s2)
	implicit none
	ComplexType r1, r2
	RealType s1, s2

#include "ff.h"

	ComplexType Li2omx2, spence
	external Li2omx2, spence

	ComplexType r12, l12

	if( abs(Im(r1)) + abs(Im(r2)) .lt. zeroeps ) then
	  Li2rat = Li2omx2(r1, s1, r2, s2)
	  return
	endif

	r12 = r1*r2
	if( abs(r12) .lt. 1 ) then
	  Li2rat = pi6 - spence(0, r12, 0D0)
	  if( abs(r12*(1 - r12)) .gt. diffeps ) Li2rat = Li2rat -
     &      (ln(r1, s1) + ln(r2, s2))*log(1 - r12)
	else
	  r12 = 1/r12
	  l12 = ln(r1, s1) + ln(r2, s2)
	  Li2rat = -pi6 + spence(0, r12, 0D0) -
     &      l12*(.5D0*l12 + log(1 - r12))
	endif
	end

************************************************************************

	ComplexType function spence(i_in, z_in, s)
	implicit none
	integer i_in
	ComplexType z_in
	RealType s

#include "ff.h"

	ComplexType Li2series
	external Li2series

	ComplexType z(0:1)
	RealType az1

	z(i_in) = z_in
	z(1-i_in) = 1 - z_in

#ifdef WARNINGS
	if( s .eq. 0 .and. 
     &    Im(z) .eq. 0 .and. abs(Re(z1)) .lt. diffeps )
     &    print *, "spence: argument on cut"
#endif

	if( Re(z(0)) .lt. .5D0 ) then
	  if( abs(z(0)) .lt. 1 ) then
	    spence = Li2series(z(1), s)
	  else
	    spence = -pi6 -
     &        .5D0*ln(-z(0), -s)**2 - Li2series(-z(1)/z(0), -s)
	  endif
	else
	  az1 = abs(z(1))
	  if( az1 .lt. 1D-15 ) then
	    spence = pi6
	  else if( az1 .lt. 1 ) then
	    spence = pi6 -
     &        ln(z(0), s)*ln(z(1), -s) - Li2series(z(0), -s)
	  else
	    spence = 2*pi6 +
     &        .5D0*ln(-z(1), -s)**2 - ln(z(0), s)*ln(z(1), -s) +
     &        Li2series(-z(0)/z(1), s)
	  endif
	endif
	end

************************************************************************

	ComplexType function Li2series(z1, s)
	implicit none
	ComplexType z1
	RealType s

#include "ff.h"

	ComplexType xm, x2, new
	integer j

* these are the even-n Bernoulli numbers, already divided by (n + 1)!
* as in Table[BernoulliB[n]/(n + 1)!, {n, 2, 50, 2}]
	RealType b(25)
	data b /
     &    0.02777777777777777777777777777777777777777778774D0, 
     &    -0.000277777777777777777777777777777777777777777778D0, 
     &    4.72411186696900982615268329554043839758125472D-6, 
     &    -9.18577307466196355085243974132863021751910641D-8, 
     &    1.89788699889709990720091730192740293750394761D-9, 
     &    -4.06476164514422552680590938629196667454705711D-11, 
     &    8.92169102045645255521798731675274885151428361D-13, 
     &    -1.993929586072107568723644347793789705630694749D-14, 
     &    4.51898002961991819165047655285559322839681901D-16, 
     &    -1.035651761218124701448341154221865666596091238D-17, 
     &    2.39521862102618674574028374300098038167894899D-19, 
     &    -5.58178587432500933628307450562541990556705462D-21, 
     &    1.309150755418321285812307399186592301749849833D-22, 
     &    -3.087419802426740293242279764866462431595565203D-24, 
     &    7.31597565270220342035790560925214859103339899D-26, 
     &    -1.740845657234000740989055147759702545340841422D-27, 
     &    4.15763564461389971961789962077522667348825413D-29, 
     &    -9.96214848828462210319400670245583884985485196D-31, 
     &    2.394034424896165300521167987893749562934279156D-32, 
     &    -5.76834735536739008429179316187765424407233225D-34, 
     &    1.393179479647007977827886603911548331732410612D-35, 
     &    -3.372121965485089470468473635254930958979742891D-37, 
     &    8.17820877756210262176477721487283426787618937D-39, 
     &    -1.987010831152385925564820669234786567541858996D-40, 
     &    4.83577851804055089628705937311537820769430091D-42 /

	xm = -ln(z1, -s)
	x2 = xm**2
	Li2series = xm - x2/4D0
	do j = 1, 25
	  xm = xm*x2
	  new = Li2series + xm*b(j)
	  if( new .eq. Li2series ) return
	  Li2series = new
	enddo
#ifdef WARNINGS
	print *, "Li2series: bad convergence"
#endif
	end

************************************************************************

	integer function eta(z1, s1, z2, s2, s12)
	implicit none
	ComplexType z1, z2
	RealType s1, s2, s12

	RealType im1, im2, im12

	im1 = Im(z1)
	if( im1 .eq. 0 ) im1 = s1
	im2 = Im(z2)
	if( im2 .eq. 0 ) im2 = s2
	im12 = Im(z1*z2)
	if( im12 .eq. 0 ) im12 = s12

	if( im1 .lt. 0 .and. im2 .lt. 0 .and. im12 .gt. 0 ) then
	  eta = 1
	else
     &  if( im1 .gt. 0 .and. im2 .gt. 0 .and. im12 .lt. 0 ) then
	  eta = -1
	else
	  eta = 0
#ifdef WARNINGS
	  if( .not. (im2 .eq. 0 .and. Re(z2) .gt. 0 .or.
     &               im1 .eq. 0 .and. Re(z1) .gt. 0) .and.
     &        (im1 .eq. 0 .and. Re(z1) .lt. 0 .or.
     &         im2 .eq. 0 .and. Re(z2) .lt. 0 .or.
     &         im12 .eq. 0 .and. Re(z1*z2) .lt. 0) )
     &      print *, "eta not defined"
#endif
	endif
	end

