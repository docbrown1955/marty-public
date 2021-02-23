#include "externals.h"
#include "types.h"


*###[ ffxdb1:
	subroutine ffxdb1(cdb1, p, m1, m2, ier)
***#[*comment:***********************************************************
*									*
*	DB1 function (derivative of B1)					*
*									*
*	algorithm adapted from Ansgar Denner's bcanew.f			*
*									*
***#]*comment:*********************************************************** 
*  #[ declarations:
	implicit none
*
*	arguments
*
	ComplexType cdb1
	RealType p, m1, m2
	integer ier

	ComplexType ffpvf, ffypvf
	external ffpvf, ffypvf

	ComplexType xp, xm, yp, ym, r

#include "ff.h"

	logical initir
	save initir

	data initir /.FALSE./
*
*  #[ declarations:

	if( abs(p) .gt. diffeps*(m1 + m2) ) then

*		IR divergent case
	  if( m2 .eq. 0 .and. p .eq. m1 ) then
	    if( .not. initir ) then
	      initir = .TRUE.
	      print *, "ffxdb1: IR divergent B1', using cutoff ",
     +          lambda
	    endif
	    if( lambda .le. 0 ) then
	      cdb1 = .5D0*(3 + log(mudim/p))/p
	    else
	      cdb1 = .5D0*(3 + log(lambda/p))/p
	    endif
	    return
	  endif

	  call ffroots(p, m1, m2, xp, xm, yp, ym, r, ier)
	  if( abs(xp - xm) .gt. diffeps*abs(xp + xm) ) then
	    cdb1 = (ffypvf(2, xp, yp) - ffypvf(2, xm, ym))/r
	  else if( abs(xp) .gt. 10 ) then
	    cdb1 = Re( (2/3D0 +
     +       (2 - 3*xp)*ffpvf(3, xp, yp))/xp**2 )/p
	  else if( abs(yp) .gt. diffeps ) then
	    cdb1 = Re( (3/2D0 +
     +       (2 - 3*xp)*ffpvf(1, xp, yp)) )/p
	  else
	    call fferr(101, ier)
	    cdb1 = nan
	  endif

*		zero momentum case
	else if( abs(m1 - m2) .gt. diffeps*(m1 + m2) ) then
	  xm = (1 - cIeps)*m1/(m1 - m2)
	  ym = (1 - cIeps)*m2/(m2 - m1)
	  if( abs(xm) .lt. 10 ) then
	    cdb1 = -(1/3D0 + ffypvf(2, xm, ym))/(m1 - m2)
	  else
	    cdb1 = -(1/3D0 + ffypvf(3, xm, ym))/m1
	  endif

	else
	  cdb1 = -1/12D0/m1

	endif

	end


*###[ ffxdb11:
	subroutine ffxdb11(cdb11, p, m1, m2, ier)
***#[*comment:***********************************************************
*									*
*	DB11 function (derivative of B11)                               *
*									*
***#]*comment:*********************************************************** 
*  #[ declarations:
	implicit none
*
*	arguments
*
	ComplexType cdb11
	RealType p, m1, m2
	integer ier

	ComplexType ffpvf, ffypvf
	external ffpvf, ffypvf

	ComplexType xp, xm, yp, ym, r

#include "ff.h"
*
*  #] declarations:

	if( abs(p) .gt. diffeps*(m1 + m2) ) then

	  call ffroots(p, m1, m2, xp, xm, yp, ym, r, ier)
	  if( abs(xp - xm) .gt. diffeps*abs(xp + xm) ) then
	    cdb11 = (ffypvf(3, xm, ym) - ffypvf(3, xp, yp))/r
	  else if( abs(xp) .gt. 10 ) then
	    cdb11 = Re( (-3/4D0 +
     +       (4*xp - 3)*ffpvf(4, xp, yp))/xp**2 )/p
	  else if( abs(yp) .gt. diffeps ) then
	    cdb11 = Re( (-4/3D0 +
     +       (4*xp - 3)*ffpvf(2, xp, yp))/p )
	  else
c	    call fferr(102, ier)
	    cdb11 = nan
	  endif

*		zero momentum case
	else if( abs(m1 - m2) .gt. diffeps*(m1 + m2) ) then
	  xm = (1 - cIeps)*m1/(m1 - m2)
	  ym = (1 - cIeps)*m2/(m2 - m1)
	  if( abs(xm) .lt. 10 ) then
	    cdb11 = (1/4D0 + ffypvf(3, xm, ym))/(m1 - m2)
	  else
	    cdb11 = (1/4D0 + ffypvf(4, xm, ym))/m1
	  endif

	else
	  cdb11 = 1/20D0/m1

	endif

	end


*###[ ffxdb11:
	subroutine ffxb111(cb111, p, m1, m2, ier)
***#[*comment:***********************************************************
*									*
*	B111 function (coefficient of p_mu p_nu p_rho)                  *
*									*
***#]*comment:*********************************************************** 
*  #[ declarations:
	implicit none
*
*	arguments
*
	ComplexType cb111
	RealType p, m1, m2
	integer ier

	ComplexType ffpvf, ffypvf, ffthf, ffxlogx
	external ffpvf, ffypvf, ffthf, ffxlogx

	ComplexType xp, xm, yp, ym, r

#include "ff.h"
*
*  #] declarations:

	if( abs(p) .gt. diffeps*(m1 + m2) ) then

	  call ffroots(p, m1, m2, xp, xm, yp, ym, r, ier)
	  if( abs(yp) .gt. .5D0 .and. abs(ym) .gt. .5D0 ) then
	    cb111 = 1/4D0*( log(m2/mudim) - delta +
     &        ffpvf(4, xp, yp) + ffpvf(4, xm, ym) )
	  else if( abs(xp) .lt. 10 .and. abs(xm) .lt. 10 ) then
	    cb111 = 1/4D0*( log(p/mudim*(1 - cIeps)) -
     &        delta - 1/2D0 +
     &        (1 + xp)*(1 + xp**2)*ffxlogx(yp) -
     &        xp*(1/3D0 + xp*(1/2D0 + xp*(1 + ffxlogx(-xp)))) +
     &        (1 + xm)*(1 + xm**2)*ffxlogx(ym) -
     &        xm*(1/3D0 + xm*(1/2D0 + xm*(1 + ffxlogx(-xm)))) )
          else if( abs(xp) .gt. .5D0 .and. abs(xm) .gt. .5D0 ) then
	    cb111 = 1/4D0*( log(m1/mudim) - delta +
     &        ffthf(4, xp, yp) + ffthf(4, xm, ym) )
	  else
c	    call fferr(102, ier)
	    cb111 = nan
	  endif

*		zero momentum case
	else if( abs(m1 - m2) .gt. diffeps*(m1 + m2) ) then
	  xm = (1 - cIeps)*m1/(m1 - m2)
	  ym = (1 - cIeps)*m2/(m2 - m1)
	  if( abs(ym) .gt. .5D0 ) then
	    cb111 = 1/4D0*(log(m2/mudim) - delta + ffpvf(4, xm, ym))
	  else
	    cb111 = 1/4D0*(log(m1/mudim) - delta -
     &        (1 + xm*(1 + xm*(1 + xm)))*ffypvf(0, xm, ym) -
     &        xm*(xm*(xm + 1/2D0) + 1/3D0) - 1/4D0)
	  endif

	else
	  cb111 = 1/4D0*(log(m2/mudim) - delta)

	endif

	end


*###[ ffroots
	subroutine ffroots(p, m1, m2, xp, xm, yp, ym, r, ier)
***#[*comment:***********************************************************
*									*
*	roots of quadratic equation					*
*	p*x^2 + (m2 - m1 - p)*x + m2 - I eps =				*
*	  p*(x - xp)*(x - xm) = p*(x - 1 + yp)*(x - 1 + ym)		*
*	i.e. x[pm] = 1 - y[pm]						*
*									*
***#]*comment:*********************************************************** 
*  #[ declarations:
	implicit none
*
*	arguments
*
	RealType p, m1, m2
	ComplexType xp, xm, yp, ym, r
	integer ier

	RealType qx, qy

#include "ff.h"
*
*  #] declarations:

*  #[ check input:
	if( p .eq. 0 ) then
	  call fferr(39, ier)
	  return
	endif
*  #] check input:

	qx = m1 - m2 + p
	qy = m2 - m1 + p

	r = sqrt(ToComplex(p*(p - m1 - m2) - m1*qy - m2*qx))
	xp = .5D0*(qx + r)/p
	xm = .5D0*(qx - r)/p
	if( abs(xm) .gt. abs(xp) ) then
	  xp = m1/(p*xm)
	else if( abs(xp) .gt. abs(xm) ) then
	  xm = m1/(p*xp)
	endif
	xp = xp + sign(abs(xp), p)*cIeps
	xm = xm - sign(abs(xm), p)*cIeps

	ym = .5D0*(qy + r)/p
	yp = .5D0*(qy - r)/p
	if( abs(ym) .gt. abs(yp) ) then
	  yp = m2/(p*ym)
	else if( abs(yp) .gt. abs(ym) ) then
	  ym = m2/(p*yp)
	endif
	yp = yp - sign(abs(yp), p)*cIeps
	ym = ym + sign(abs(ym), p)*cIeps
	end


*###[ ffpvf
	ComplexType function ffpvf(n, x, y)
***#[*comment:***********************************************************
*									*
*	Passarino-Veltman function f(n, x)				*
*	here third arg y = 1 - x					*
*									*
***#]*comment:*********************************************************** 
*  #[ declarations:
	implicit none
*
*	arguments
*
	integer n
	ComplexType x, y

	ComplexType xm
	integer m

#include "ff.h"
*
*  #] declarations:

	if( abs(x) .lt. 5 ) then
	  if( n .eq. 0 ) then
	    ffpvf = -log(-y/x)
	  else if( abs(x) .lt. 1D-14 ) then
	    ffpvf = -1D0/n
	  else
	    xm = -log(-y/x)
	    do m = 1, n
	      xm = x*xm - 1D0/m
	    enddo
	    ffpvf = xm
	  endif
	else
	  ffpvf = 0
	  xm = 1
	  do m = 1, 30
	    xm = xm/x
	    ffpvf = ffpvf + xm/(m + n)
	    if( abs(xm) .lt. precx*abs(ffpvf) ) return
	  enddo
	endif
	end


*###[ ffypvf
	ComplexType function ffypvf(n, x, y)
***#[*comment:***********************************************************
*									*
*	y*ffpvf(n, x, y)						*
*									*
***#]*comment:*********************************************************** 
*  #[ declarations:
	implicit none
*
*	arguments
*
	integer n
	ComplexType x, y

	ComplexType ffpvf
	external ffpvf
*
*  #] declarations:

	if( abs(y) .eq. 0 ) then
	  ffypvf = 0
	else
	  ffypvf = y*ffpvf(n, x, y)
	endif
	end


*###[ ffypvf
	ComplexType function ffxlogx(x)
***#[*comment:***********************************************************
*									*
*	x*log(x)							*
*									*
***#]*comment:*********************************************************** 
*  #[ declarations:
	implicit none
*
*	arguments
*
	ComplexType x
*
*  #] declarations:

	if( abs(x) .eq. 0 ) then
	  ffxlogx = 0
	else
	  ffxlogx = x*log(x)
	endif
	end


*###[ ffthf
	ComplexType function ffthf(n, x, y)
***#[*comment:***********************************************************
*									*
*	y*ffpvf(n, x, y)						*
*									*
***#]*comment:*********************************************************** 
*  #[ declarations:
	implicit none
*
*	arguments
*
	integer n
	ComplexType x, y

	ComplexType ffpvf
	external ffpvf

	ComplexType xm
	integer m

#include "ff.h"
*
*  #] declarations:

	if( abs(x) .gt. 1D4 ) then
	  xm = n
	  ffthf = 0
	  do m = 1, 30
	    xm = xm/x
	    ffthf = ffthf - xm/(m*(m + n))
	    if( abs(xm) .lt. precx*abs(ffthf) ) return
	  enddo
	else
	  xm = ffpvf(1, y, x)
	  ffthf = xm
	  do m = 1, n - 1
	    xm = x*xm + 1D0/(m*(m + 1))
	    ffthf = ffthf + xm
	  enddo
	endif

	end

