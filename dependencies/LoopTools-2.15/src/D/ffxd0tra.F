* ffd0tra.F
* a special case of the D0 function
* original code by Francesco Tramontano
* this file is part of LoopTools
* last modified 13 Sep 12 th

#include "externals.h"
#include "types.h"


	subroutine ffd0tra(res, S, T, ML2, ME2, ier)
	implicit none
	ComplexType res
	RealType S, T, ML2, ME2
	integer ier

c===============================c
c                               c
c    p1      S       p2         c
c      \            /           c
c       \          /            c  S  = (p1+p2)^2
c        \========/             c  T  = (p2+p3)^2
c        ||      ||             c  ML2= mass-square of the particle in the loop
c        ||      ||     T       c  ME2= mass-square of the external particle p4
c        ||      ||             c
c        E========\             c
c       E     ML2  \            c
c      E            \           c
c                    p3         c
c   p4^2=ME2                    c
c                               c
c===============================c

	ComplexType xp, xm, ypS, ymS, ypT, ymT, ypE, ymE
	ComplexType xr, yr
	integer iepsS, iepsT, iepsE

	ComplexType ffint3
	external ffint3

	call fftraroot(xm, xp, xr, -ML2*(ME2 - S - T)/(S*T))
	call fftraroot(ymS, ypS, yr, ML2/S)
	call fftraroot(ymT, ypT, yr, ML2/T)
	call fftraroot(ymE, ypE, yr, ML2/ME2)

	iepsS = 0
	if( S .gt. 0 ) iepsS = 1

	iepsT = 0
	if( T .gt. 0 ) iepsT = 1

	iepsE = 0
	if( ME2 .gt. 0 ) iepsE = 1

	res = (
     &    ffint3(ypS, xm,  iepsS, ier) - ffint3(ypS, xp,  iepsS, ier) +
     &    ffint3(ymS, xm, -iepsS, ier) - ffint3(ymS, xp, -iepsS, ier) +
     &    ffint3(ypT, xm,  iepsT, ier) - ffint3(ypT, xp,  iepsT, ier) +
     &    ffint3(ymT, xm, -iepsT, ier) - ffint3(ymT, xp, -iepsT, ier) -
     &    ffint3(ypE, xm,  iepsE, ier) + ffint3(ypE, xp,  iepsE, ier) -
     &    ffint3(ymE, xm, -iepsE, ier) + ffint3(ymE, xp, -iepsE, ier)
     &  )/(xr*S*T)
	end

************************************************************************

	subroutine fftraroot(xm, xp, r, c)
***#[*comment:***********************************************************
*                                                                       *
*       roots of quadratic equation x^2 + x + c == 0                    *
*                                                                       *
***#]*comment:***********************************************************
*  #[ declarations:
	implicit none
	ComplexType xm, xp, r
	RealType c
*  #] declarations:

	r = sqrt(ToComplex(1 - 4*c))
	xp = .5D0*(1 + r)
	xm = .5D0*(1 - r)
	if( abs(xp) .gt. abs(xm) ) then
	  xm = c/xp
	else
	  xp = c/xm
	endif
	end

************************************************************************

	ComplexType function ffint3(y, x, ieps, ier)
	implicit none
	ComplexType y, x
	integer ieps, ier

* compute \int_0^1 dz log(z - y)/(z - x)

#include "ff.h"

	ComplexType arg1, arg2, dd1, dd2, zlog
	integer ipi121, ipi122

c	RealType rarg1, rarg2
c	equivalence (arg1, rarg1), (arg2, rarg2)

c	RealType ddilog
c	ComplexType li2
c	external ddilog, li2

	if( Im(x) .ne. 0 ) call ffwarn(258, ier, 1D0, 0D0)

	arg1 = x/(x - y)
	if( abs(Im(arg1)) .lt. 1D-15 ) then
	  call ffzxdl(dd1, ipi121, zlog, arg1, ieps, ier)
c	  dd1 = ddilog(rarg1)
c	  if( rarg1 .gt. 1 )
c     &      dd1 = dd1 + eps*sign(pi, x)*log(rarg1)*cI
	else
	  call ffzzdl(dd1, ipi121, zlog, arg1, ier)
c	  dd1 = li2(arg1)
	endif

	arg2 = (x - 1)/(x - y)
	if( abs(Im(arg2)) .lt. 1D-15 ) then
	  call ffzxdl(dd2, ipi122, zlog, arg2, ieps, ier)
c	  dd2 = ddilog(rarg2)
c	  if( rarg2 .gt. 1 )
c     &      dd2 = dd2 - eps*sign(pi, 1 - x)*log(rarg2)*cI
	else
	  call ffzzdl(dd2, ipi122, zlog, arg2, ier)
c	  dd2 = li2(arg2)
	endif

	ffint3 = dd1 - dd2 + (ipi121 - ipi122)*pi12
	end

