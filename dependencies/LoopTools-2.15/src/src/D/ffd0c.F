* ffd0c.F
* the scalar four-point function with complex masses
* this file is part of LoopTools
* last modified 26 Jan 14 th

* Written by Le Duc Ninh, MPI, Munich (2008).     
* Spence, log and eta functions are taken from FF.
* Please cite arXiv:0902.0325 [hep-ph] if you use this function.

#include "externals.h"
#include "types.h"


	subroutine ffd0c(cd0c, cpi, key, ier)
	implicit none
	ComplexType cd0c, cpi(10)
	integer key, ier

#include "ff.h"
#include "perm.h"

	integer o
	RealType ra, rb, rg, rc, rh, rj
	RealType d, a
	ComplexType cd, ce, ck, cfx

	RealType signf
	parameter (signf = -1)

	ComplexType ffT13, ffTn
	external ffT13, ffTn

#define PP(i) Re(cpi(i+4))
#define LightLike(i) abs(PP(i)) .lt. precx

* 2 lightlike momenta

	if( LightLike(1) .and.
     &      LightLike(3) ) then
	  o = p1234
	else if( LightLike(2) .and.
     &           LightLike(4) ) then
	  o = p4123
	else if( LightLike(5) .and.
     &           LightLike(6) ) then
	  o = p1342
	else if( LightLike(1) .and.
     &           LightLike(2) ) then
	  o = p1234
	else if( LightLike(2) .and.
     &           LightLike(3) ) then
	  o = p2341
	else if( LightLike(3) .and.
     &           LightLike(4) ) then
	  o = p3412
	else if( LightLike(4) .and.
     &           LightLike(1) ) then
	  o = p4123

* 1 lightlike momentum

	else if( LightLike(1) ) then
	  o = p1234
	else if( LightLike(2) ) then
	  o = p2341
	else if( LightLike(3) ) then
	  o = p3412
	else if( LightLike(4) ) then
	  o = p4123
	else if( LightLike(5) ) then
	  o = p1342
	else if( LightLike(6) ) then
	  o = p2413

* kallen(pi, pj, pk) >= 0

#define SIDE(i,j) PP(i)*(PP(i) - 2*PP(j))

	else if( SIDE(5,1) +
     &           SIDE(1,2) +
     &           SIDE(2,5) .ge. 0 ) then
	  o = p1234
	else if( SIDE(6,2) +
     &           SIDE(2,3) +
     &           SIDE(3,6) .ge. 0 ) then
	  o = p2341
	else if( SIDE(5,3) +
     &           SIDE(3,4) +
     &           SIDE(4,5) .ge. 0 ) then
	  o = p3412
	else if( SIDE(6,4) +
     &           SIDE(4,1) +
     &           SIDE(1,6) .ge. 0 ) then
	  o = p4123

	else
	  call fferr(103, ier)
	  return
	endif

#define RP(i) PP(ibits(o,3*(10-i),3))
#define CM(i) cpi(ibits(o,3*(4-i),3))

	rg = RP(1)
	rb = RP(2)
	rj = RP(5) - rb
	ra = RP(3)
	rc = RP(6) - ra
	rh = RP(4) - RP(6)
	cfx = CM(4)
	cd = CM(3) - cfx - ra
	ce = CM(2) - CM(3) - rc
	ck = CM(1) - CM(2) - rh
	rc = rc - rb
	rh = rh - rj
	rj = rj - rg

* D0C = \int_0^1 dx \int_0^x dy \int_0^y dz
*   1/(ra x^2 + rb y^2 + rg z^2 + rc xy + rh xz + rj yz +
*      cd x + ce y + ck z + cfx + I signf)
* with signf = -eps.
* important: variables "signX" is the sign of img(X) in case X becomes real.

* 2 opposite lightlike momenta

	if( ra .eq. 0 .and. rg .eq. 0 ) then
	  cd0c = ffT13(rb + rj, rc + rh, rb, rc,
     &      cd, ce + ck, cfx, signf, ce, ier)
	  return
	endif

* 2 adjacent lightlike momenta

	if( rb .eq. 0 .and. rg .eq. 0 ) then
	  cd0c = ffTn(ra, rb, rc, rh, rj,
     &             cd, ce, cfx, signf, ck, signf, key, ier) -
     &           ffTn(ra, rj, rc + rh, rh, rj,
     &             cd, ce + ck, cfx, signf, ck, signf, key, ier)
	  return
	endif

* 1 lightlike momentum

	if( rg .eq. 0 ) then
	  cd0c = ffTn(ra, rb, rc, rh, rj,
     &             cd, ce, cfx, signf, ck, signf, key, ier) -
     &           ffTn(ra, rj + rb, rc + rh, rh, rj,
     &             cd, ce + ck, cfx, signf, ck, signf, key, ier)
	  return
	endif

* alpha is one root of: rg*alpha^2 + rj*alpha + rb == 0
* we do not need the ieps for alpha
	d = rj**2 - 4*rg*rb

	d = sqrt(d)
	a = -.5D0/rg*(rj + d)
	d = -.5D0/rg*(rj - d)
* choose the bigger root or unity
	if( abs(a) .gt. abs(d) ) then
	  d = rb/(rg*a)
	else
	  a = rb/(rg*d)
	endif
* which one for alpha?
	if( abs(d) .lt. abs(a) ) a = d

	cd0c = ffTn(ra + rb + rc, rg, rj + rh,
     &    -rc - 2*rb - (rj + rh)*a, -rj - 2*a*rg,
     &    cd + ce, ck, cfx, signf, -ce - ck*a, -signf, key, ier)

	if( a .ne. 1 ) then
	  d = 1/(1 - a)
	  cd0c = cd0c + ffTn(ra, rg + rj + rb, rc + rh,
     &      d*(rc + rh*a), rj + 2*a*rg,
     &      cd, ce + ck, cfx, signf, d*(ce + ck*a), d*signf, key, ier)
	endif

	if( abs(a) .lt. precx ) then
	  call ffwarn(253, ier, 1D0, 0D0)
	else
	  d = 1/a
	  cd0c = cd0c + ffTn(ra, rb, rc,
     &      d*rc + rh, -rj - 2*a*rg,
     &      cd, ce, cfx, signf, d*ce + ck, d*signf, key, ier)
	endif
	end

