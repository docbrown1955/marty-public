* ffS2.F
* calculate S2 = \int_0^1 dy ln(a y^2 + b y + c), 
* where a is real and can be zero; b and c complex
* input: ra=a (real), cb=b, cc=c
* signc=sign(img(c)) in case c is real.
* cza and czb are the 2 roots of: a y^2 + b y + c == 0
* remarks: ieps is needed for cza, czb.
* this file is part of LoopTools
* last modified 8 Dec 10 th

* Written by Le Duc Ninh, MPI, Munich (2008).
* Spence, log and eta functions are taken from FF.
* Oct 28 2008

#include "externals.h"
#include "types.h"


	ComplexType function ffS2(ra, cb, cc, signc, ier)
	implicit none
	RealType ra, signc
	ComplexType cb, cc
	integer ier

#include "ff.h"

	ComplexType crdisc, cza, czb
	RealType sza, szb, sy1, sy2, sc

	ComplexType ffS2_linr, zfflog
	integer nffet1
	external ffS2_linr, zfflog, nffet1

	sc = Im(cc)
	if( sc .eq. 0 ) sc = signc

	if( abs(ra) .lt. precx ) then
	  if( abs(cb) .lt. precx ) then
* 0 roots:
	    if( abs(cc) .lt. precx ) then
	      call fferr(89, ier)
	      ffS2 = 0
	      return
	    endif
	    ffS2 = zfflog(cc, 1, ToComplex(signc), ier)
	    return
	  endif

* 1 root:
	  cza = -cc/cb
	  sza = -signc*Re(cb)
	  if( sza .eq. 0 ) sza = -signc

	  ffS2 = zfflog(cb, 1, ToComplex(sc), ier) +
     &      ffS2_linr(cza, sza, ier)

	  if( abs(Im(cb)) .lt. precx ) return

* complex b
	  szb = Im(cza)
	  if( szb .eq. 0 ) szb = sza

	  ffS2 = ffS2 +
     &      c2ipi*nffet1(cb, ToComplex(0D0, -szb), ToComplex(0D0, sc), ier)
	  return
	endif

* 2 roots: cza = y1, czb = y2
* eq.: y**2 + (b/a) y + (c/a) = 0
* the ieps is irrelevant here since we take into account
* the contributions of both roots

*** Ninh changed: 14 Aug 2009
	crdisc = sqrt(cb**2/ra**2 - 4*cc/ra)
	cza = -.5D0*(cb/ra + crdisc)
	czb = -.5D0*(cb/ra - crdisc)
	if( abs(cza) .gt. abs(czb) ) then
	  czb = cc/(ra*cza)
	else
	  cza = cc/(ra*czb)
	endif

* calculate the sign of im(cza) and im(czb) which are related to ieps
	sza = signc/ra
	if( abs(Re(crdisc)) .gt. precx ) sza = sza/Re(crdisc)
	szb = -sza

	sy1 = Im(cza)
	if( sy1 .eq. 0 ) sy1 = sza
	sy2 = Im(czb)
	if( sy2 .eq. 0 ) sy2 = szb

* calculate the log and etas
* ieps=1 to choose the cut along the real axis

	ffS2 =
     &    zfflog(ToComplex(ra), 1, ToComplex(sc), ier) +
     &    c2ipi*nffet1(ToComplex(0D0, -sy1), ToComplex(0D0, -sy2),
     &      ToComplex(0D0, sc/ra), ier) +
     &    ffS2_linr(cza, sza, ier) +
     &    ffS2_linr(czb, szb, ier)
	end

************************************************************************
* calculate S2 = \int_0^1 dy ln(y - z),
* where z is complex
* input: cz, signz = sign(im(z)) in case z is real.
* remarks: ieps is needed.

	ComplexType function ffS2_linr(cz, signz, ier)
	implicit none
	ComplexType cz
	RealType signz
	integer ier

#include "ff.h"

	ComplexType zfflog
	external zfflog

	if( abs(cz) .lt. precx ) then
	  ffS2_linr = -1
	else if( abs(cz - 1) .lt. precx ) then
	  ffS2_linr = zfflog(-cz, 1, ToComplex(-signz), ier) - 1
	else
	  ffS2_linr =
     &      zfflog(1 - cz, 1, ToComplex(-signz), ier)*(1 - cz) +
     &      zfflog(-cz, 1, ToComplex(-signz), ier)*cz - 1
	endif
	end

