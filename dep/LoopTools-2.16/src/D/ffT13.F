* ffT13.F
* part of the complex four-point function
* this file is part of LoopTools
* last modified 8 Dec 10 th

#include "externals.h"
#include "types.h"


* T13 = \int_0^1 dx \int_0^x dy
*   y/( (rg y^2 + rh xy + cd x + cj y + cf + I signf) *
*       (ra y^2 + rc xy + cd x + ce y + cf + I signf) )
* with signf = -eps
* variables "signX" is the sign of im(X) in case X becomes real.
* No extra term is needed.
* Nov 11 2008

	ComplexType function ffT13(ra, rc, rg, rh,
     &    cd, ce, cf, signf, cj, ier)
	implicit none
	RealType ra, rc, rg, rh, signf
	ComplexType cd, ce, cf, cj
	integer ier

#include "ff.h"

	ComplexType ck, cl, cn, cy(2), crdetq4
	ComplexType cbj(4), ccj(4)
	ComplexType ffS3nAll1, ffS3nAll2
	RealType sn, scj, sy(2), raj(4)

	ComplexType ffS2, ffS3n
	external ffS2, ffS3n

* the coefficients of the 4 log arguments
	raj(1) = ra
	raj(2) = rg
	raj(3) = rg + rh
	raj(4) = ra + rc

	cbj(1) = ce + rc
	cbj(2) = cj + rh
	cbj(3) = cd + cj
	cbj(4) = ce + cd

	ccj(1) = cf + cd
	ccj(2) = cf + cd
	ccj(3) = cf
	ccj(4) = cf

* the ieps is the same for all
	scj = signf

* the prefactor 1/(S V - T U)
* eq. (S V - T U) = K y^2 + L y + N == 0
* Leading Landau singularity can occur if y1 = y2 and eps -> 0
* the ieps is needed for the roots

	ck = rh*ra - rc*rg
	cl = (ra - rg)*cd + rh*ce - rc*cj
	cn = (rh - rc)*cf + cd*(ce - cj)
* the ieps for cn
	sn = signf*(rh - rc)
* if (rh - rc) = 0 then we are at the boundary of phase space
* and sn is irrelevant

	if( abs(ck) .lt. precx ) then
	  if( abs(cl) .lt. precx ) then
	    if( abs(cn) .lt. precx ) then
	      call fferr(99, ier)
	      ffT13 = 0
	      return
	    endif
* the case ny = 0, (SV - TU) = N = constant
* no extra term is needed
	    ffT13 = -1/cn*(
     &        ffS2(raj(1), cbj(1), ccj(1), scj, ier) -
     &        ffS2(raj(2), cbj(2), ccj(2), scj, ier) +
     &        ffS2(raj(3), cbj(3), ccj(3), scj, ier) -
     &        ffS2(raj(4), cbj(4), ccj(4), scj, ier) )
	    return
	  endif

* the case ny = 1, (S V - T U) = L y + N
	  cy(1) = -cn/cl
* ieps for this root
	  sy(1) = -sn*Re(cl)
	  if( sy(1) .eq. 0 ) sy(1) = signf

	  ffS3nAll1 =
     &      ffS3n(cy(1), sy(1), raj(1), cbj(1), ccj(1), scj, ier) -
     &      ffS3n(cy(1), sy(1), raj(2), cbj(2), ccj(2), scj, ier) +
     &      ffS3n(cy(1), sy(1), raj(3), cbj(3), ccj(3), scj, ier) -
     &      ffS3n(cy(1), sy(1), raj(4), cbj(4), ccj(4), scj, ier)
	  ffT13 = -ffS3nAll1/cl
	  return
	endif

* the case ny = 2, (SV - TU) = K y^2 + L y + N
	crdetq4 = sqrt(cl**2 - 4*ck*cn)
	cy(1) = -.5D0/ck*(cl + crdetq4)
	cy(2) = -.5D0/ck*(cl - crdetq4)
	if( abs(cy(1)) .gt. abs(cy(2)) ) then
	  cy(2) = cn/(ck*cy(1))
	else
	  cy(1) = cn/(ck*cy(2))
	endif

* calculate the signs of img(cy1) and img(cy2) which are related to ieps
	sy(1) = sn*Re(crdetq4)
	if( sy(1) .eq. 0 ) sy(1) = signf
	sy(2) = -sy(1)

	ffS3nAll1 =
     &    ffS3n(cy(1), sy(1), raj(1), cbj(1), ccj(1), scj, ier) -
     &    ffS3n(cy(1), sy(1), raj(2), cbj(2), ccj(2), scj, ier) +
     &    ffS3n(cy(1), sy(1), raj(3), cbj(3), ccj(3), scj, ier) -
     &    ffS3n(cy(1), sy(1), raj(4), cbj(4), ccj(4), scj, ier)
	ffS3nAll2 =
     &    ffS3n(cy(2), sy(2), raj(1), cbj(1), ccj(1), scj, ier) -
     &    ffS3n(cy(2), sy(2), raj(2), cbj(2), ccj(2), scj, ier) +
     &    ffS3n(cy(2), sy(2), raj(3), cbj(3), ccj(3), scj, ier) -
     &    ffS3n(cy(2), sy(2), raj(4), cbj(4), ccj(4), scj, ier)
	ffT13 = (ffS3nAll1 - ffS3nAll2)/crdetq4
	end

