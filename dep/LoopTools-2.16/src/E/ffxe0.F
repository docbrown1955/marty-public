#include "externals.h"
#include "types.h"


*	$Id: ffxe0.f,v 1.4 1996/01/10 15:36:51 gj Exp $
*###[ ffxe0:
	subroutine ffxe0(ce0,cd0i,xpi,ier)
***#[*comment:***********************************************************
*									*
*	calculate							*
*									*
*	      1  /   /						     \-1*
*	e0= -----\dq |(q^2-m_1^2)((q+p_1)^2-m_2^2)...((q-p_5)^2-m_5^2|	*
*	    ipi^2/   \						     /	*
*									*
*	following the five four-point-function method in ....		*
*	As an extra the five fourpoint function Di are also returned	*
*	if ( ldot ) the dotproducts are left behind in fpij5(15,15) in	*
*	/ffdot/ and the external determinants fdel4 and fdl3i(5) in	*
*	/ffdel/.							*
*									*
*	Input:	xpi = m_i^2	   (real)  i=1,5			*
*		xpi = p_i.p_i	   (real)  i=6,10 (note: B&D metric)	*
*		xpi = (p_i+p_{i+1})^2 (r)  i=11,15			*
*		xpi = (p_i+p_{i+2})^2 (r)  i=16,20 OR 0			*
*									*
*	Output:	ce0		  (complex)				*
*		cd0i(5)		  (complex) D0 with s_i missing		*
*		ier		  (integr) 0=ok 1=inaccurate 2=error	*
*									*
***#]*comment:***********************************************************
*  #[ declarations:
	implicit none
*
*	arguments
*
	RealType xpi(20)
	ComplexType ce0,cd0i(5)
	integer ier
*
*	local variables
*
	integer i,j,NMIN,NMAX,ier0,i6,i7,i8,i9
	parameter(NMIN=15,NMAX=20)
	RealType dpipj(NMIN,NMAX),xmax
	logical lp5(NMAX-NMIN)
*
*	common blocks:
*
#include "ff.h"
*  #] declarations:
*  #[ get differences:
*
*	simulate the differences in the masses etc..
*
*	first p16-p20
*
	do 5 i=1,5
	    if ( xpi(i+15) .eq. 0 ) then
		i6 = i+5
		i7 = i6+1
		if ( i7 .ge. 11 ) i7 = 6
		i8 = i7+1
		if ( i8 .ge. 11 ) i8 = 6
		i9 = i8+1
		if ( i9 .ge. 11 ) i9 = 6
		xpi(i+15) = xpi(i6)+xpi(i7)+xpi(i8)-xpi(i6+5)-xpi(i7+5)+
     +			xpi(i9+5)
		xmax = max(abs(xpi(i6)),abs(xpi(i7)),abs(xpi(i8)),abs(
     +			xpi(i6+5)),abs(xpi(i7+5)),abs(xpi(i9+5)))
		if ( abs(xpi(i+15)) .lt. xloss*xmax )
     +			call ffwarn(168,ier,xpi(i+15),xmax)
		lp5(i) = .TRUE.
	    else
		lp5(i) = .FALSE.
	    endif
    5	continue
*
*	next the differences
*
	ier0 = 0
	do 40 i=1,NMAX
	  do 30 j=1,NMIN
	    dpipj(j,i) = xpi(j) - xpi(i)
   30	  continue
   40	continue
*  #] get differences:
*  #[ call ffxe0a:
	call ffxe0a(ce0,cd0i,xpi,dpipj,ier)
*  #] call ffxe0a:
*  #[ clean up:
	do 90 i=1,5
	    if ( lp5(i) ) then
		 xpi(i+NMIN) = 0
	    endif
   90	continue
*  #] clean up:
*###] ffxe0:
	end
*###[ ffxe0a:
	subroutine ffxe0a(ce0,cd0i,xpi,dpipj,ier)
***#[*comment:***********************************************************
*									*
*	calculate							*
*									*
*	      1  /   /						     \-1*
*	e0= -----\dq |(q^2-m_1^2)((q+p_1)^2-m_2^2)...((q-p_5)^2-m_5^2|	*
*	    ipi^2/   \						     /	*
*									*
*	following the five four-point-function method in ....		*
*	As an extra the five fourpoint function Di are also returned	*
*	if ( ldot ) the dotproducts are left behind in fpij5(15,15) in	*
*	/ffdot/ and the external determinants fdel4 and fdl3i(5) in	*
*	/ffdel/.							*
*									*
*	Input:	xpi = m_i^2	   (real)  i=1,5			*
*		xpi = p_i.p_i	   (real)  i=6,10 (note: B&D metric)	*
*		xpi = (p_i+p_{i+1})^2 (r)  i=11,15			*
*		xpi = (p_i+p_{i+2})^2 (r)  i=16,20			*
*		dpipj(15,20)	   (real)  = pi(i) - pi(j)		*
*									*
*	Output:	ce0		  (complex)				*
*		cd0i(5)		  (complex) D0 with s_i missing		*
*		ier		  (integer) <50:lost # digits 100=error	*
*									*
***#]*comment:***********************************************************
*  #[ declarations:
	implicit none
*
*	arguments
*
	integer ier
	ComplexType ce0,cd0i(5)
	RealType xpi(20),dpipj(15,20)
*
*	local variables
*
	integer i,j,ii(10),ii4(6),ieri(5),ier0,imin,itype,ndiv,idone,
     +		ier1
	logical ldel2s
	ComplexType c,cfac,cs,csum
	RealType dl5s,dl4p,xpi4(13),dpipj4(10,13),piDpj4(10,10),
     +		absc,xmax,piDpj(15,15),xqi4(13),dqiqj4(10,13),
     +		qiDqj4(10,10),del2s,xmx5(5),dl4ri(5)
	save ii4
*
*	common blocks:
*
#include "ff.h"
*
*	statement function
*
	absc(c) = abs(Re(c)) + abs(Im(c))
*
*	data
*
	data ii4 /5,6,7,8,9,10/
*
*  #] declarations:
*  #[ initialisations:
	ndiv = 0
	idsub = 0
	ce0 = 0
	do 1 i=1,5
	    cd0i(i) = 0
    1	continue
*  #] initialisations:
*  #[ calculations:
*
	idsub = idsub + 1
	call ffdot5(piDpj,xpi,dpipj,ier)
	if ( ldot ) then
	    do 6 i=1,15
		do 5 j=1,15
		    fpij5(j,i) = piDpj(j,i)
    5		continue
    6	    continue
	    do 10 i=1,10
		ii(i) = i+5
   10	    continue
	    idsub = idsub + 1
	    ier0 = 0
	    call ffdl4p(dl4p,piDpj,ii)
*	    if ( dl4p .lt. 0 ) then
*		call fferr(57,ier)
*	    endif
	    fdel4 = dl4p
	endif
	idsub = idsub + 1
	call ffdel5(dl5s,xpi,piDpj)
*
	do 40 i=1,5
	    ieri(i) = ier
   40	continue
*
	do 100 i=1,5
*
*	    get the coefficient determinant
*
	    idsub = idsub + 1
	    call ffdl4r(dl4ri(i),piDpj,i)
*
*	    get four-point momenta
*
	    call ffpi54(xpi4,dpipj4,piDpj4,xpi,dpipj,piDpj,i)
*
*	    first try IR divergent function to avoid error messages from ffrot4
*
	    ier1 = ieri(i)
	    call ffxdir(cs,cfac,idone,xpi4,dpipj4,6,ndiv,ier1)
	    if ( idone .gt. 0 ) then
*		done
		xmax = abs(cs)*10d0**(-mod((ier1-ieri(i)),50))
	    else
*
*		rotate to calculable posistion
*
		call ffrot4(irota4,del2s,xqi4,dqiqj4,qiDqj4,xpi4,dpipj4,
     +			piDpj4,5,itype,ieri(i))
		if ( itype .lt. 0 ) then
		    print *,'ffxe0:  error:  Cannot handle this ',
     +			' 4point masscombination yet:'
		    print *,(xpi(j),j=1,20)
		    return
		endif
		if ( itype .eq. 1 ) then
		    ldel2s = .TRUE.
		    isgnal = +1
		    print *,'ffxe0a: Cannot handle del2s = 0 yet'
		    stop
		else
		    ldel2s = .FALSE.
		endif
		if ( itype .eq. 2 ) then
		    print *,'ffxe0a: no doubly IR divergent yet'
		    stop
		endif
*
*		get fourpoint function
*
		ier0 = ieri(i)
		call ffxd0e(cs,cfac,xmax, .FALSE.,ndiv,xqi4,dqiqj4,
     +			qiDqj4,del2s,ldel2s,ieri(i))
		if ( ieri(i).gt.10 ) then
		    isgnal = -isgnal
		    ieri(i) = ier0
		    call ffxd0e(cs,cfac,xmax, .TRUE.,ndiv,xqi4,dqiqj4,
     +			qiDqj4,del2s,ldel2s,ieri(i))
		    isgnal = -isgnal
		endif
	    endif
*
*	    Finally ...
*
	    cd0i(i) = cs*cfac
	    xmx5(i) = xmax*absc(cfac)
	    if ( ldot ) then
		call ffdl3p(fdl3i(i),piDpj4,10,ii4,ii4)
*		let's hope tha tthese have been set by ffxd0e...
		fdl4si(i) = fdel4s
	    endif
  100	continue
*
*  #] calculations:
*  #[ add all up:
*
	csum = 0
	xmax = 0
	imin = 1
	do 200 i=1,5
	    imin = -imin
	    csum = csum + imin*Re(dl4ri(i))*cd0i(i)
	    if ( ieri(i) .gt. 50 ) then
		ieri(i) = mod(ieri(i),50)
	    endif
	    xmax = max(xmax,dl4ri(i)*xmx5(i)*Re(10)**mod(ieri(i),50))
  200	continue
*
*	If the imaginary part is very small it most likely is zero
*	(can be removed, just esthetically more pleasing)
*
	if ( abs(Im(csum)) .lt. precc*abs(Re(csum)) )
     +		csum = ToComplex(Re(csum))
*
*	Finally ...
*
	ce0 = csum*(1/Re(2*dl5s))
*
*  #] add all up:
*###] ffxe0a:
	end
*###[ ffxe00:
	subroutine ffxe00(ce0,cd0i,dl4ri,xpi,piDpj)
***#[*comment:***********************************************************
*									*
*	calculate							*
*									*
*	      1  /   /						     \-1*
*	e0= -----\dq |(q^2-m_1^2)((q+p_1)^2-m_2^2)...((q-p_5)^2-m_5^2|	*
*	    ipi^2/   \						     /	*
*									*
*	following the five four-point-function method in ....		*
*	The four five fourpoint function Di are input in this version.	*
*									*
*	Input:	cd0i(5)		(complex)	D0 with s_i missing	*
*		dl4ri(5)		(real)		coeff of D0		*
*		xpi = m_i^2	(real)  i=1,5				*
*		xpi = p_i.p_i	(real)  i=6,10 (note: B&D metric)	*
*		xpi = (p_i+p_{i+1})^2 (r)  i=11,15			*
*		xpi = (p_i+p_{i+2})^2 (r)  i=16,20			*
*		piDpj(15,15)	   (real)  pi.pj			*
*									*
*	Output:	ce0		  (complex)				*
*									*
***#]*comment:***********************************************************
*  #[ declarations:
	implicit none
*
*	arguments
*
	ComplexType ce0,cd0i(5)
	RealType dl4ri(5),xpi(20),piDpj(15,15)
*
*	local variables
*
	integer i,ii(10),imin
	ComplexType c,csum
	RealType dl5s,dl4p,absc,xmax
*
*	common blocks:
*
#include "ff.h"
*
*	statement function
*
	absc(c) = abs(Re(c)) + abs(Im(c))
*  #] declarations:
*  #[ initialisations:
*
	idsub = idsub + 1
	ce0 = 0
*
*  #] initialisations:
*  #[ calculations:
*
	if ( ldot ) then
	    do 10 i=1,10
		ii(i) = i+5
   10	    continue
	    idsub = idsub + 1
	    call ffdl4p(dl4p,piDpj,ii)
	    fdel4 = dl4p
	endif
	idsub = idsub + 1
	call ffdel5(dl5s,xpi,piDpj)
*
*  #] calculations:
*  #[ add all up:
*
	csum = 0
	xmax = 0
	imin = 1
	do 200 i=1,5
	    imin = -imin
	    csum = csum + imin*Re(dl4ri(i))*cd0i(i)
	    xmax = max(xmax,abs(dl4ri(i))*absc(cd0i(i)))
  200	continue
*
*	If the imaginary part is very small it most likely is zero
*	(can be removed, just esthetically more pleasing)
*
	if ( abs(Im(csum)) .lt. precc*abs(Re(csum)) )
     +		csum = ToComplex(Re(csum))
*
*	Finally ...
*
	ce0 = csum*(1/Re(2*dl5s))
*
*  #] add all up:
*###] ffxe00:
	end
*###[ ffdot5:
	subroutine ffdot5(piDpj,xpi,dpipj,ier)
***#[*comment:***********************************************************
*									*
*	calculate the dotproducts pi.pj with				*
*									*
*		xpi(i) = s_i		i=1,5				*
*		xpi(i) = p_i		i=6,10				*
*		xpi(i) = p_i+p_{i+1}	i=11,15				*
*									*
***#]*comment:***********************************************************
*  #[ declarations:
	implicit none
*
*	arguments
*
	integer ier
	RealType xpi(20),dpipj(15,20),piDpj(15,15)
*
*	local variables
*
	integer is1,is2,is3,is4,ip6,ip7,ip8,ip11,ip12,ip14,
     +		itel,i1,i2,i3,i4,i5,i6,ierin
*
*	common blocks
*
#include "ff.h"
*
*	data
*
*  #] declarations:
*  #[ indices:
	ierin = ier
	do 10 is1=1,5
	    is2 = is1 + 1
	    if ( is2 .eq. 6 ) is2 = 1
	    is3 = is2 + 1
	    if ( is3 .eq. 6 ) is3 = 1
	    ip6 = is1 + 5
	    ip7 = is2 + 5
	    ip11 = ip6 + 5
*
*	    we have now defined a 3point function
*
*	          | -p11
*	          |
*	         / \
*	      s1/   \s3
*	    ___/_____\___
*	    p6   s2    p7
*
*  #] indices:
*  #[ all in one vertex:
*
*	    pi.pi, si.si
*
	    piDpj(is1,is1) = xpi(is1)
	    piDpj(ip6,ip6) = xpi(ip6)
	    piDpj(ip11,ip11) = xpi(ip11)
*
*	    si.s(i+1)
*
	    if ( xpi(is2) .le. xpi(is1) ) then
		piDpj(is1,is2) = (dpipj(is1,ip6) + xpi(is2))/2
	    else
		piDpj(is1,is2) = (dpipj(is2,ip6) + xpi(is1))/2
	    endif
	    piDpj(is2,is1) = piDpj(is1,is2)
*
*	    si.s(i+2)
*
	    if ( xpi(is1) .le. xpi(is3) ) then
		piDpj(is3,is1) = (dpipj(is3,ip11) + xpi(is1))/2
	    else
		piDpj(is3,is1) = (dpipj(is1,ip11) + xpi(is3))/2
	    endif
	    piDpj(is1,is3) = piDpj(is3,is1)
*
*	    pi.si
*
	    if ( abs(xpi(ip6)) .le. xpi(is1) ) then
		piDpj(ip6,is1) = (dpipj(is2,is1) - xpi(ip6))/2
	    else
		piDpj(ip6,is1) = (dpipj(is2,ip6) - xpi(is1))/2
	    endif
	    piDpj(is1,ip6) = piDpj(ip6,is1)
*
*	    pi.s(i+1)
*
	    if ( abs(xpi(ip6)) .le. xpi(is2) ) then
		piDpj(ip6,is2) = (dpipj(is2,is1) + xpi(ip6))/2
	    else
		piDpj(ip6,is2) = (dpipj(ip6,is1) + xpi(is2))/2
	    endif
	    piDpj(is2,ip6) = piDpj(ip6,is2)
*
*	    p(i+2).s(i)
*
	    if ( abs(xpi(ip11)) .le. xpi(is1) ) then
		piDpj(ip11,is1) = -(dpipj(is1,is3) + xpi(ip11))/2
	    else
		piDpj(ip11,is1) = -(dpipj(ip11,is3) + xpi(is1))/2
	    endif
	    piDpj(is1,ip11) = piDpj(ip11,is1)
*
*	    p(i+2).s(i+2)
*
	    if ( abs(xpi(ip11)) .le. xpi(is3) ) then
		piDpj(ip11,is3) = -(dpipj(is1,is3) - xpi(ip11))/2
	    else
		piDpj(ip11,is3) = -(dpipj(is1,ip11) - xpi(is3))/2
	    endif
	    piDpj(is3,ip11) = piDpj(ip11,is3)
*  #] all in one vertex:
*  #[ all in one 3point:
*
*	    pi.s(i+2)
*
	    if ( min(abs(dpipj(is2,is1)),abs(dpipj(ip11,ip7))) .le.
     +		 min(abs(dpipj(ip11,is1)),abs(dpipj(is2,ip7))) ) then
		piDpj(ip6,is3) = (dpipj(ip11,ip7) + dpipj(is2,is1))/2
	    else
		piDpj(ip6,is3) = (dpipj(ip11,is1) + dpipj(is2,ip7))/2
	    endif
	    piDpj(is3,ip6) = piDpj(ip6,is3)
*
*	    p(i+1).s(i)
*
	    if ( min(abs(dpipj(is3,is2)),abs(dpipj(ip6,ip11))) .le.
     +		 min(abs(dpipj(ip6,is2)),abs(dpipj(is3,ip11))) ) then
		piDpj(ip7,is1) = (dpipj(ip6,ip11) + dpipj(is3,is2))/2
	    else
		piDpj(ip7,is1) = (dpipj(ip6,is2) + dpipj(is3,ip11))/2
	    endif
	    piDpj(is1,ip7) = piDpj(ip7,is1)
*
*	    p(i+2).s(i+1)
*
	    if ( min(abs(dpipj(is1,is3)),abs(dpipj(ip7,ip6))) .le.
     +		 min(abs(dpipj(ip7,is3)),abs(dpipj(is1,ip6))) ) then
		piDpj(ip11,is2) = -(dpipj(ip7,ip6) + dpipj(is1,is3))/2
	    else
		piDpj(ip11,is2) = -(dpipj(ip7,is3) + dpipj(is1,ip6))/2
	    endif
	    piDpj(is2,ip11) = piDpj(ip11,is2)
*  #] all in one 3point:
*  #[ all external 3point:
*
*	    pi.p(i+1)
*
	    if ( abs(xpi(ip7)) .le. abs(xpi(ip6)) ) then
		piDpj(ip6,ip7) = (dpipj(ip11,ip6) - xpi(ip7))/2
	    else
		piDpj(ip6,ip7) = (dpipj(ip11,ip7) - xpi(ip6))/2
	    endif
	    piDpj(ip7,ip6) = piDpj(ip6,ip7)
*
*	    p(i+1).p(i+2)
*
	    if ( abs(xpi(ip11)) .le. abs(xpi(ip7)) ) then
		piDpj(ip7,ip11) = -(dpipj(ip6,ip7) - xpi(ip11))/2
	    else
		piDpj(ip7,ip11) = -(dpipj(ip6,ip11) - xpi(ip7))/2
	    endif
	    piDpj(ip11,ip7) = piDpj(ip7,ip11)
*
*	    p(i+2).p(i)
*
	    if ( abs(xpi(ip6)) .le. abs(xpi(ip11)) ) then
		piDpj(ip11,ip6) = -(dpipj(ip7,ip11) - xpi(ip6))/2
	    else
		piDpj(ip11,ip6) = -(dpipj(ip7,ip6) - xpi(ip11))/2
	    endif
	    piDpj(ip6,ip11) = piDpj(ip11,ip6)
*  #] all external 3point:
*  #[ the other 3point:
	    is4 = is3 + 1
	    if ( is4 .eq. 6 ) is4 = 1
	    ip8 = is3 + 5
	    ip14 = is4 + 10
*
*	    we now work with the threepoint configuration
*
*	          | p14
*	          |
*	         / \
*	      s1/   \s4
*	    ___/_____\___
*	    p11  s3    p8
*
*	    s1.p8
*
	    do 11 itel = 1,3
		if ( itel .eq. 1 ) then
		    i1 = is1
		    i2 = is3
		    i3 = is4
		    i4 = ip11
		    i5 = ip8
		    i6 = ip14
		elseif ( itel .eq. 2 ) then
		    i1 = is3
		    i2 = is4
		    i3 = is1
		    i4 = ip8
		    i5 = ip14
		    i6 = ip11
		else
		    i1 = is4
		    i2 = is1
		    i3 = is3
		    i4 = ip14
		    i5 = ip11
		    i6 = ip8
		endif
*
*		in one go: the opposite sides
*
		if ( min(abs(dpipj(i3,i2)),abs(dpipj(i4,i6))) .le.
     +		     min(abs(dpipj(i4,i2)),abs(dpipj(i3,i6))) ) then
		    piDpj(i5,i1) = (dpipj(i3,i2) + dpipj(i4,i6))/2
		else
		    piDpj(i5,i1) = (dpipj(i4,i2) + dpipj(i3,i6))/2
		endif
		piDpj(i1,i5) = piDpj(i5,i1)
*
*		and the remaining external ones
*
		if ( abs(xpi(i5)) .le. abs(xpi(i4)) ) then
		    piDpj(i4,i5) = (dpipj(i6,i4) - xpi(i5))/2
		else
		    piDpj(i4,i5) = (dpipj(i6,i5) - xpi(i4))/2
		endif
		piDpj(i5,i4) = piDpj(i4,i5)
   11	    continue
*  #] the other 3point:
*  #[ 4point indices:
	    ip12 = ip7+5
*
*	    we now have the fourpoint configuration
*
*	    \p14   /p8
*	     \____/
*	     | s4 |
*	   s1|    |s3
*	     |____|
*	   p6/ s2 \p7
*	    /      \
*
*
*
	    do 12 itel = 1,2
		if ( itel .eq. 1 ) then
		    i1 = ip6
		    i2 = ip8
		    i3 = ip7
		    i4 = ip14
		else
		    i1 = ip7
		    i2 = ip14
		    i3 = ip6
		    i4 = ip8
		endif
		if ( min(abs(dpipj(i3,ip11)),abs(dpipj(i4,ip12))) .le.
     +		     min(abs(dpipj(i4,ip11)),abs(dpipj(i3,ip12))) ) then
		    piDpj(i1,i2) = (dpipj(i3,ip11) + dpipj(i4,ip12))/2
		else
		    piDpj(i1,i2) = (dpipj(i4,ip11) + dpipj(i3,ip12))/2
		endif
		piDpj(i2,i1) = piDpj(i1,i2)
   12	    continue
*
*	    we are only left with p11.p12 etc.
*
	    if ( min(abs(dpipj(ip14,ip8)),abs(dpipj(ip7,ip6))) .le.
     +		 min(abs(dpipj(ip7,ip8)),abs(dpipj(ip14,ip6))) ) then
		piDpj(ip11,ip12) = (dpipj(ip7,ip6) + dpipj(ip14,ip8))/2
	    else
		piDpj(ip11,ip12) = (dpipj(ip7,ip8) + dpipj(ip14,ip6))/2
	    endif
	    piDpj(ip12,ip11) = piDpj(ip11,ip12)
   10	continue
*  #] 4point indices:
*###] ffdot5:
	end
*###[ ffpi54:
	subroutine ffpi54(xpi4,dpipj4,piDpj4,xpi,dpipj,piDpj,inum)
***#[*comment:***********************************************************
*									*
*	Gets the dotproducts pertaining to the fourpoint function with  *
*	s_i missing out of the five point function dotproduct array.	*
*									*
*	Input:	xpi	real(20)	si.si,pi.pi			*
*		dpipj	real(15,20)	xpi(i) - xpi(j)			*
*		piDpj	real(15,15)	pi(i).pi(j)			*
*		inum	integer		1--5				*
*									*
*	Output:	xpi4	real(13)					*
*		dpipj4	real(10,13)					*
*		piDpj4	real(10,10)					*
*									*
***#]*comment:***********************************************************
*  #[ declarations:
	implicit none
*
*	arguments
*
	integer inum
	RealType xpi(20),dpipj(15,20),piDpj(15,15),xpi4(13),
     +		dpipj4(10,13),piDpj4(10,10)
*
*	local variables
*
	integer i,j,iplace(11,5),isigns(11,5)
	save iplace,isigns
*
*	common blocks
*
#include "ff.h"
*
*	data
*
	data iplace /
     +		2,3,4,5, 07,08,09,15, 12,13, 17,
     +		1,3,4,5, 11,08,09,10, 14,13, 18,
     +		1,2,4,5, 06,12,09,10, 14,15, 19,
     +		1,2,3,5, 06,07,13,10, 11,15, 20,
     +		1,2,3,4, 06,07,08,14, 11,12, 16/
*
	data isigns /
     +		+1,+1,+1,+1, +1,+1,+1,+1, -1,+1, +1,
     +		+1,+1,+1,+1, +1,+1,+1,+1, +1,+1, +1,
     +		+1,+1,+1,+1, +1,+1,+1,+1, +1,-1, +1,
     +		+1,+1,+1,+1, +1,+1,+1,+1, -1,-1, +1,
     +		+1,+1,+1,+1, +1,+1,+1,+1, -1,+1, +1/
*  #] declarations:
*  #[ distribute:
*
*	copy p5-p11
*
	do 20 i=1,11
	    xpi4(i) = xpi(iplace(i,inum))
	    do 10 j=1,10
		dpipj4(j,i) = dpipj(iplace(j,inum),iplace(i,inum))
   10	    continue
   20	continue
*
*	these cannot be simply copied I think
*
	xpi4(12) = -xpi4(5)+xpi4(6)-xpi4(7)+xpi4(8)+xpi4(9)+xpi4(10)
	xpi4(13) = xpi4(5)-xpi4(6)+xpi4(7)-xpi4(8)+xpi4(9)+xpi4(10)
*
*	and the differences
*
	do 40 i=12,13
	    do 30 j=1,10
		dpipj4(j,i) = xpi4(j) - xpi4(i)
   30	    continue
   40	continue
*
*	copy the dotproducts (watch the signs of p9,p10!)
*
	do 60 i=1,10
	    do 50 j=1,10
		piDpj4(j,i) = isigns(j,inum)*isigns(i,inum)*
     +			piDpj(iplace(j,inum),iplace(i,inum))
   50	    continue
   60	continue
*  #] distribute:
*###] ffpi54:
	end
*###[ ffxe0r:
	subroutine ffxe0r(ce0,cd0i,xpi,ier)
***#[*comment:***********************************************************
*									*
*	Tries all 12 permutations of the 5pointfunction			*
*									*
***#]*comment:***********************************************************
*  #[ declarations:
	implicit none
	integer ier,nrot
	parameter(nrot=12)
	RealType xpi(20),xqi(20)
	ComplexType ce0,cd0i(5),ce0p,cd0ip(5),cd0ipp(5)
	integer inew(20,nrot),irota,ier1,i,j,k,icon,ialsav,init
	logical lcon
	parameter (icon=3)
	save inew,init,lcon
#include "ff.h"
	data inew
     +	       /1,2,3,4,5, 6,7,8,9,10,11,12,13,14,15, 16,17,18,19,20,
     +		2,1,3,4,5, 6,11,8,9,15,7,14,13,12,10, 16,18,17,19,-20,
     +		1,3,2,4,5, 11,7,12,9,10,6,8,15,14,13, -16,17,19,18,20,
     +		1,2,4,3,5, 6,12,8,13,10,14,7,9,11,15, 16,-17,18,20,19,
     +		1,2,3,5,4, 6,7,13,9,14,11,15,8,10,12, 20,17,-18,19,16,
     +		5,2,3,4,1, 15,7,8,14,10,13,12,11,9,6, 17,16,18,-19,20,
     +		2,1,4,3,5, 6,14,8,13,15,12,11,9,7,10, 16,-18,17,20,-19,
     +		1,3,2,5,4, 11,7,15,9,14,6,13,12,10,8, -20,17,-19,18,16,
     +		5,2,4,3,1, 15,12,8,11,10,9,7,14,13,6, 17,-16,18,-20,19,
     +		2,1,3,5,4, 6,11,13,9,12,7,10,8,15,14, 20,18,-17,19,-16,
     +		5,3,2,4,1, 13,7,12,14,10,15,8,6,9,11, -17,16,19,-18,20,
     +	      1,3,5,2,4, 11,13,15,12,14,10,7,9,6,8,-20,-17,-19,-16,-18/
	data init /0/
*  #] declarations:
*  #[ open console for some activity on screen:
	if ( init .eq. 0 ) then
	    init = 1
	    lcon = .FALSE.
	endif
*  #] open console for some activity on screen:
*  #[ calculations:
	ce0 = 0
	ier = 999
	ialsav = isgnal
	do 30 j = -1,1,2
	    do 20 irota=1,nrot
		do 10 i=1,20
		    if ( inew(i,irota) .lt. 0 ) then
			xqi(-inew(i,irota)) = 0
		    else
			xqi(inew(i,irota)) = xpi(i)
		    endif
   10		continue
		print '(a,i2,a,i2)','---#[ rotation ',irota,
     +			': isgnal ',isgnal
		if (lcon) write(icon,'(a,i2,a,i2)') 'rotation ',irota,
     +                  ', isgnal ',isgnal
		ier1 = 0
		ner = 0
		id = id + 1
		isgnal = ialsav
		call ffxe0(ce0p,cd0ip,xqi,ier1)
		ier1 = ier1 + ner
		print '(a,i1,a,i2)','---#] rotation ',irota,': isgnal ',
     +			isgnal
		print '(a,2g28.16,i3)','e0 = ',ce0p,ier1
		do 15 k=1,5
		    cd0ipp(k) = cd0ip(inew(k,irota))
		    print '(a,2g28.16,i3)','d0 = ',cd0ipp(k),k
   15		continue
		if (lcon) write(icon,'(a,2g28.16,i3)')'e0 = ',ce0p,ier1
		if ( ier1 .lt. ier ) then
		    ce0 = ce0p
		    do 19 k=1,5
			cd0i(k) = cd0ipp(k)
   19		    continue
		    ier = ier1
		endif
   20	    continue
	    ialsav = -ialsav
   30	continue
*  #] calculations:
*###] ffxe0r:
	end

