* ffTn.F
* calculate T(ra, rb, rc, rg, rh; cd, ce, cf, cj) defined as: 
* T = \int_0^1 dx \int_0^x dy
*   1/((rg x + rh y + cj)
*      (ra x^2 + rb y^2 + rc x y + cd x + ce y + cf + I signf))
* with signf = -eps,
* {ra,rb,rc,rg,rh} are real, {cd,ce,cf,cj} are complex.
* important: variables "signX" is the sign of im(X) in case X becomes real.
* this file is part of LoopTools
* last modified 8 Dec 10 th

* Written by Le Duc Ninh, MPI, Munich (2008).
* Spence, log and eta functions are taken from FF.
* Oct 27 2008

#include "externals.h"
#include "types.h"
#include "defs.h"


	ComplexType function ffTn(ra, rb, rc, rgx, rhx,
     &    cd, ce, cf, signf, cjx, signj, key, ier)
	implicit none
	RealType ra, rb, rc, rgx, rhx, signf, signj
	ComplexType cd, ce, cf, cjx
	integer key, ier

#include "ff.h"

	ComplexType cj, crdetq4, crdetq42, cy(2), cy2(2)
	ComplexType crdisc, cbeta1, cbeta2, cbeta
	ComplexType ctv, ctemp, cresd, cyij
	ComplexType cbj(6), ccj(6), cbk(6), cck(6)
	RealType rg, rh, reps
	RealType sj, scj, sy(2), sy2(2), stv, syij
	RealType rminuv, rminuv2, raj(6)
	integer i, j, ny, ny2, chketa(2), chketa2(2)

	ComplexType ffT_lin, ffS2, ffS3n, zfflog
	integer nffet1
	external ffT_lin, ffS2, ffS3n, zfflog, nffet1

	ier = 0

* calculate ieps and the sign of im(J)
	reps = Im(cf)
	if( reps .eq. 0 ) reps = signf
	reps = sign(1D0, -reps)

	sj = Im(cjx)
	if( sj .eq. 0 ) sj = signj
	sj = sign(1D0, sj*reps)

* change the sign of G,H,J
* sj = 1 or -1
	rg = -sj*rgx
	rh = -sj*rhx
	cj = -sj*cjx

	if( abs(rb) .lt. precx ) then
	  ffTn = sj*ffT_lin(ra, rc, rg, rh, cd, ce, cf, cj,
     &      signf, reps, ier)
	  return
	endif 

	if( abs(ra) .lt. precx ) then
* change the integration variables to get rb = 0 as above
	  ffTn = sj*ffT_lin(rb + rc, -rc, -rg - rh, rg,
     &      -2*(rb + rc) - cd - ce,
     &      rc + cd,
     &      rb + rc + cd + ce + cf,
     &      rg + rh + cj,
     &      signf, reps, ier)
	  return
	endif 	      	
	
* calculate beta
* beta is one root of: B beta^2 + C beta + A = 0
* we do not need the ieps for beta
	crdisc = sqrt(ToComplex(rc**2 - 4*rb*ra))
	cbeta1 = -.5D0/rb*(rc + crdisc)
	cbeta2 = -.5D0/rb*(rc - crdisc)
	if( abs(cbeta1) .gt. abs(cbeta2) ) then
	  cbeta2 = ra/(rb*cbeta1)
	else
	  cbeta1 = cbeta2      
	  cbeta2 = ra/(rb*cbeta2)
	endif

* Ninh added: 14 Aug 2009
* be careful with this approximation, IMG can be wrong
	if( abs(1 - cbeta1) .lt. precx ) cbeta1 = 1
	if( abs(1 - cbeta2) .lt. precx ) cbeta2 = 1
* which one for beta?
	if( abs(cbeta1) .gt. abs(cbeta2) ) then
	  ctemp = cbeta1
	  cbeta1 = cbeta2
	  cbeta2 = ctemp
	endif

* look at the prefactor 1/(S V - T U)
* eq. (S V - T U) = K y^2 + L y + N == 0
* to decide which beta is the best.
* The two roots are calculated.
* Leading Landau Sing. can occur if y1 = y2 and eps -> 0
* the ieps is needed for the roots

	cbeta = cbeta1
	if( abs(cbeta2 - 1) .lt. precx ) then
	  cbeta = cbeta2
	  cbeta2 = cbeta1
	endif

	call ffwbeta(rb, rc, rg, rh, cd, ce, cf, cj, signf,
     &    cbeta, crdetq4, ny, cy, sy, chketa, rminuv, key, ier)

* to check whether there is numerical cancellation
* at the border of the triangle
	if( rminuv .lt. 1D-10 ) then
	  call ffwbeta(rb, rc, rg, rh, cd, ce, cf, cj, signf,
     &      cbeta2, crdetq42, ny2, cy2, sy2, chketa2, rminuv2,
     &      key, ier)
	  if( rminuv2 .lt. rminuv ) then
	    call ffwarn(254, ier, 1D0, 0D0)
	  else
* choose the beta2-parameters
	    cbeta = cbeta2
	    crdetq4 = crdetq42
	    ny = ny2
	    do i = 1, ny
	      sy(i) = sy2(i)
	      cy(i) = cy2(i)
	      chketa(i) = chketa2(i)
	    enddo
	  endif
	endif

* the coefficients of the 6 log arguments
	raj(1) = 0
	raj(2) = 0
	raj(3) = 0
	raj(4) = rb
	raj(5) = ra + rb + rc
	raj(6) = ra

	cbj(1) = rh
	cbj(2) = rg + rh
	cbj(3) = rg
	cbj(4) = rc + ce
	cbj(5) = ce + cd
	cbj(6) = cd

	ccj(1) = rg + cj
	ccj(2) = cj
	ccj(3) = cj
	ccj(4) = ra + cd + cf
	ccj(5) = cf
	ccj(6) = cf

* the ieps for the log arguments
	scj = -reps

* the cck(6)-coefficients before the logs
	cck(1) = 1
	cck(2) = -1 + cbeta
	cck(3) = -cbeta
	cck(4) = -1
	cck(5) = 1 - cbeta
	cck(6) = cbeta

	if( ny .eq. 0 ) then
* no extra term is needed
	  ffTn = -sj/crdetq4*(
     &      cck(1)*ffS2(raj(1), cbj(1), ccj(1), scj, ier) +
     &      cck(2)*ffS2(raj(2), cbj(2), ccj(2), scj, ier) +
     &      cck(3)*ffS2(raj(3), cbj(3), ccj(3), scj, ier) +
     &      cck(4)*ffS2(raj(4), cbj(4), ccj(4), scj, ier) +
     &      cck(5)*ffS2(raj(5), cbj(5), ccj(5), scj, ier) +
     &      cck(6)*ffS2(raj(6), cbj(6), ccj(6), scj, ier) )
	  return
	endif

* cbk(6)-coefficients of cj/(aj y - bj - yi)
	cbk(1) = cbeta
	cbk(2) = 0
	cbk(3) = 0
	cbk(4) = cbeta
	cbk(5) = 0
	cbk(6) = 0

	ffTn = 0
	do i = 1, ny
	  cresd = 0
	  if( chketa(i) .ne. 0 ) then
* extra term needed
* calculate the residue

* the denominator was checked above in ffS3n therefore the (V/T)_pole
* should be safe now:
	    ctv = (rh*cy(i) + cj)/(cy(i)*(rb*cy(i) + ce) + cf)
	    ctemp = (rg + cbeta*rh)/
     &        ((rc + 2*cbeta*rb)*cy(i) + cd + ce*cbeta)
	    if( abs(Im(ctemp)) .gt. abs(Im(ctv)) ) ctv = ctemp

* if im(ctv) = 0 then take the ieps from T/V
	    stv = -signf*Re(rh*cy(i) + cj)
	    if( stv .eq. 0 ) stv = -signf

	    ctv = zfflog(ctv, 1, ToComplex(stv), ier)
	    if( abs(ctv) .gt. precx ) then
	      do j = 1, 3
	        if( abs(cck(j)) .gt. precx ) then
	          cyij = -Sgn(j)*(cy(i) + cbk(j))/cck(j)
	          syij = -Sgn(j)*sy(i)*Re(cck(j))
	          if( syij .eq. 0 ) syij = sy(i)
	          cresd = cresd - Sgn(i+j)*
     &              zfflog((cyij - 1)/cyij, 1, ToComplex(syij), ier)
	        endif
	      enddo
	      cresd = cresd*ctv
	    endif
	  endif

* calculate the main part
	  do j = 1, 6
	    if( abs(cck(j)) .gt. precx ) then
	      cyij = -Sgn(j)*(cy(i) + cbk(j))/cck(j)
	      syij = -Sgn(j)*sy(i)*Re(cck(j))
	      if( syij .eq. 0 ) syij = sy(i)
	      cresd = cresd + Sgn(i+j)*
     &          ffS3n(cyij, syij, raj(j), cbj(j), ccj(j), scj, ier)
	    endif
	  enddo
	  ffTn = ffTn + cresd
	enddo

* the prefactor of Landau det.
	ffTn = sj/crdetq4*ffTn
	end

************************************************************************
* calculate the roots of the eq. ck x^2 + cl x + cn = 0
* and check if the roots are inside the triangle [0, -cbeta, 1 - cbeta]
* the ieps part for the roots is needed.
* Nov 17 2008

* input: rb, rc, rg, rh, cd, ce, cf, cj, signf, cbeta     
* output: ru, rv, ny, cy, signy, ck, cl, cn

	subroutine ffwbeta(rb, rc, rg, rh, cd, ce, cf, cj, signf,
     &    cbeta, crdetq4, ny, cy, signy, chketa, rminuv, key, ier)
	implicit none
	RealType rb, rc, rg, rh, signf, signy(2), rminuv
	ComplexType cd, ce, cf, cj, cbeta, cy(2), crdetq4
	integer ny, chketa(2), key, ier

#include "lt.h"

	ComplexType ck, cl, cn
	ComplexType cab, cac, cay
	RealType dotyc, dotyb, dotbc, dotbb, dotcc
	RealType sn, ru, rv, abc2
	integer i

	chketa(1) = 0
	chketa(2) = 0
	rminuv = 1D300

	ck = rb*rg - rh*(rc + cbeta*rb)
	cl = rg*ce - rh*cd - cj*(rc + 2*rb*cbeta)
	cn = (rg + rh*cbeta)*cf - cj*(cd + ce*cbeta)

* the ieps for cn
	sn = signf*Re(rg + rh*cbeta)
	if( sn .eq. 0 ) sn = signf

	if( abs(ck) .lt. precx ) then
	  if( abs(cl) .lt. precx ) then
* the case ny = 0, (S V - T U) = N = constant
	    if( abs(cn) .lt. precx ) then
	      call fferr(104, ier)
	      cbeta = 0
	      return
	    endif
	    ny = 0
	    crdetq4 = cn
	  else
* the case ny = 1, (S V - T U) = L y + N
	    ny = 1

	    cy(1) = -cn/cl
* ieps for this pole
	    signy(1) = -sn*Re(cl)
	    if( signy(1) .eq. 0 ) signy(1) = signf

	    crdetq4 = cl
	  endif
	else
* the case ny = 2, (S V - T U) = K y^2 + L y + N
	  ny = 2

	  crdetq4 = sqrt(cl**2 - 4*ck*cn)
	  cy(1) = -.5D0/ck*(cl + crdetq4)
	  cy(2) = -.5D0/ck*(cl - crdetq4)
	  if( abs(cy(1)) .gt. abs(cy(2)) ) then
	    cy(2) = cn/(ck*cy(1))
	  else
	    cy(1) = cn/(ck*cy(2))
	  endif

* calculate the sign of img(cy1) and img(cy2) which are related to ieps
	  signy(1) = sn*Re(crdetq4)
	  if( signy(1) .eq. 0 ) signy(1) = signf
	  signy(2) = -signy(1)  
	endif

	if( ny .eq. 0 .or. abs(Im(cbeta)) .lt. precx ) return

	if( key .eq. 1 ) then
	  chketa(1) = 1
	  chketa(2) = 1
	else
* check if the poles are inside the triangle [0, -cbeta, 1 - cbeta]
* using the barycentric technique

	  abc2 = 1/Im(cbeta)**2

	  do i = 1, ny
	    cay = cy(i) + cbeta
	    cac = cbeta
	    cab = 1

	    dotyc = Re(cay)*Re(cac) + Im(cay)*Im(cac)
	    dotyb = Re(cay)*Re(cab) + Im(cay)*Im(cab)
	    dotbc = Re(cab)*Re(cac) + Im(cab)*Im(cac)
	    dotcc = Re(cac)*Re(cac) + Im(cac)*Im(cac)
	    dotbb = 1

	    ru = (dotyc*dotbb - dotbc*dotyb)*abc2
	    rv = (dotcc*dotyb - dotyc*dotbc)*abc2
	    if( ru .ge. 0 .and. rv .ge. 0 .and. ru + rv .le. 1 )
     &        chketa(i) = 1
	    rminuv = min(rminuv, abs(ru), abs(rv))
	  enddo
	endif
	end

************************************************************************
* calculate T(ra, rc, rg, rh; cd, ce, cf, cj) defined as:
* T = \int_0^1 dx \int_0^x dy
*       1/( (rg x + rh y + cj)
*           (ra x^2 + rc x y + cd x + ce y + cf + I signf) )
* with signf = -eps,
* {ra, rc, rg, rh} are real, {cd, ce, cf, cj} are complex.
* important: variables "signX" is the sign of img(X) in case X becomes real.
* No extra term is needed.

* Written by Le Duc Ninh, MPI, Munich (2008).
* Spence, log and eta functions are taken from FF.
* Nov 10 2008

	ComplexType function ffT_lin(ra, rc, rg, rh,
     &    cd, ce, cf, cj, signf, reps, ier)
	implicit none
	RealType ra, rc, rg, rh, signf, reps
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
	raj(1) = rc + ra
	raj(2) = 0
	raj(3) = 0
	raj(4) = ra

	cbj(1) = ce + cd
	cbj(2) = rh + rg
	cbj(3) = rg
	cbj(4) = cd

	ccj(1) = cf
	ccj(2) = cj
	ccj(3) = cj
	ccj(4) = cf

* the ieps is the same for all
	scj = -reps

* the prefactor 1/(S V - T U)
* eq. (S V - T U) = K y^2 + L y + N = 0
* Leading Landau Sing. can occur if y1 == y2 and eps -> 0
* the ieps is needed for the roots

	ck = rh*ra - rc*rg
	cl = rh*cd - rc*cj - ce*rg
	cn = rh*cf - ce*cj

* the ieps for cn
	sn = -reps*rh
	if( sn .eq. 0 ) sn = -reps

	if( abs(ck) .lt. precx ) then
	  if( abs(cl) .lt. precx ) then
	    if( abs(cn) .lt. precx ) then
	      call fferr(105, ier)
	      ffT_lin = 0
	      return
	    endif
* the case ny = 0, (S V - T U) = N = constant
	    ffT_lin = 1/cn*(
     &        ffS2(raj(1), cbj(1), ccj(1), scj, ier) -
     &        ffS2(raj(2), cbj(2), ccj(2), scj, ier) +
     &        ffS2(raj(3), cbj(3), ccj(3), scj, ier) -
     &        ffS2(raj(4), cbj(4), ccj(4), scj, ier) )
	    return
	  endif

* the case ny = 1, (S V - T U) = L y + N
	  cy(1) = -cn/cl
* ieps for this pole
	  sy(1) = -sn*Re(cl)
	  if( sy(1) .eq. 0 ) sy(1) = signf

	  ffS3nAll1 =
     &      ffS3n(cy(1), sy(1), raj(1), cbj(1), ccj(1), scj, ier) -
     &      ffS3n(cy(1), sy(1), raj(2), cbj(2), ccj(2), scj, ier) +
     &      ffS3n(cy(1), sy(1), raj(3), cbj(3), ccj(3), scj, ier) -
     &      ffS3n(cy(1), sy(1), raj(4), cbj(4), ccj(4), scj, ier)
	  ffT_lin = -ffS3nAll1/cl
	  return
	endif

* the case ny = 2, (S V - T U) = K y^2 + L y + N
	crdetq4 = sqrt(cl**2 - 4*ck*cn)
	cy(1) = -.5D0/ck*(cl + crdetq4)
	cy(2) = -.5D0/ck*(cl - crdetq4)
	if( abs(cy(1)) .gt. abs(cy(2)) ) then
	  cy(2) = cn/(ck*cy(1))
	else
	  cy(1) = cn/(ck*cy(2))
	endif

* calculate the sign of im(cy1) and im(cy2) which are related to ieps
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
	ffT_lin = (ffS3nAll2 - ffS3nAll1)/crdetq4
	end

