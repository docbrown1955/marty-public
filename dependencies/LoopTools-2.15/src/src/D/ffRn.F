* ffRn.F
* calculate Rn = \int_0^1 dx (x - cz - I signz) (x - cy - I signy)
* Input: cy, cz, signz, signy
* i*sign=-i*eps is needed in the case of real masses
* this file is part of LoopTools
* last modified 8 Dec 10 th

* Written by Le Duc Ninh, MPI, Munich (Dec 15, 2008).
* Spence, log and eta functions are taken from FF.
* 14 Aug 2009: changed ieps of cdyza to "signy" (before used "signza").

#include "externals.h"
#include "types.h"


	ComplexType function ffRn(cy, signy, cz, signz, ier)
	implicit none
	ComplexType cy, cz
	RealType signy, signz
	integer ier

#include "ff.h"

	ComplexType c1, c2, c1yz, cab1, cab2, dummy
	RealType sz, syz, sab1, sab2
	integer n

	ComplexType zfflog
	integer nffet1
	external zfflog, nffet1

	if( abs(cy - cz) .lt. precx ) then
* cy == cza and check for singularities
* be careful with log(0) singularity.
	  sz = signz
	  c1 = 0
	  c2 = 0
	  if( abs(Im(cy)) .lt. precx .and. signy*sz .lt. 0 ) then
	    sz = signy
	    if( Re(cy) .ge. 0 ) then
	      c2 = sign(2D0, signz)*c2ipi
	      if( Re(cy) .le. 1 ) then
	        call ffwarn(255, ier, 1D0, 0D0)
	        c1 = c2*(zfflog(-cy, 1, ToComplex(-sz), ier) -
     &            zfflog(ToComplex(-1D-16), 1, ToComplex(-sz), ier))
	        c2 = 0
	      endif
	    endif
	  endif
	  ffRn = .5D0*(c1 +
     &      zfflog((cy - 1)/cy, 1, ToComplex(sz), ier)*(
     &        zfflog(1 - cy, 1, ToComplex(-sz), ier) +
     &        zfflog(-cy, 1, ToComplex(-sz), ier) - c2 ))
	  return
	endif

* calculate the sign of imaginary parts and eta functions
* we do not need the ieps for y0
* if im(y0) == im(y1) we may need the ieps for the logs

	sz = Im(cz)
	if( sz .eq. 0 ) sz = signz

	syz = Im(cy - cz)
	if( syz .eq. 0 ) syz = signy

	c1yz = 1/(cy - cz)

	sab1 = Im(-cz*c1yz)
	if( sab1 .eq. 0 ) then
	  sab1 = Re(cz)*signy
c	  if( sab1 .eq. 0 ) call ffwarn(256, ier, 1D0, 0D0)
* this step: not checked but same as below
* choose +signy since this ieps is relevant if cza in (0,1)
	  if( sab1 .eq. 0 ) sab1 = signy
	endif

	sab2 = Im((1 - cz)*c1yz)
	if( sab2 .eq. 0 ) then
	  sab2 = -Re(1 - cz)*signy
c	  if( sab2 .eq. 0 ) call ffwarn(257, ier, 1D0, 0D0)
* this step: checked and worked
* choose -signy since this ieps is relevant if cza in (0,1)
	  if( sab2 .eq. 0 ) sab2 = -signy
	endif

* calculate R-func from Sp-func
* def: R(y0, y1) =
*   Sp(y0/(y0-y1)) + ln(y0/(y0-y1))*eta(-y1,1/(y0-y1)) -
*   Sp((y0-1)/(y0-y1)) - ln((y0-1)/(y0-y1))*eta(1-y1,1/(y0-y1))

* calculate the two dilogs
* calls "ffzzdl(zdilog,ipi12,zlog,cx,ier)" in "ffcli2.F" or Li2C(z)

	cab1 = cy*c1yz
	if( Im(cab1) .eq. 0 .and. Re(cab1) .ge. 1 ) then
	  call ffzzdl(c1, n, dummy, 1/cab1, ier)  
	  c1 = -c1 - n*pi12 - pi6 -
     &      .5D0*zfflog(-cab1, 1, ToComplex(sab1), ier)**2
	else
	  call ffzzdl(c1, n, dummy, cab1, ier)
	  c1 = c1 + n*pi12
	endif 
	
	cab2 = (cy - 1)*c1yz
	if( Im(cab2) .eq. 0 .and. Re(cab2) .ge. 1 ) then
	  call ffzzdl(c2, n, dummy, 1/cab2, ier)
	  c2 = -c2 - n*pi12 - pi6 -
     &      .5D0*zfflog(-cab2, 1, ToComplex(sab2), ier)**2
	else
	  call ffzzdl(c2, n, dummy, cab2, ier)
	  c2 = c2 + n*pi12
	endif

* calculate the two logs
* ieps=1 to choose the cut along the real axis,

	n = nffet1(ToComplex(0D0, -sz), ToComplex(0D0, -syz),
     &    ToComplex(0D0, sab1), ier)
	if( n .ne. 0 )
     &    c1 = c1 + n*c2ipi*zfflog(cab1, 1, ToComplex(-sab1), ier)

	n = nffet1(ToComplex(0D0, -sz), ToComplex(0D0, -syz),
     &    ToComplex(0D0, sab2), ier)
	if( n .ne. 0 )
     &    c2 = c2 + n*c2ipi*zfflog(cab2, 1, ToComplex(-sab2), ier)

	ffRn = c1 - c2 +
     &    zfflog((cy - 1)/cy, 1, ToComplex(signy), ier)*
     &      zfflog(cy - cz, 1, ToComplex(signy), ier)
	end
