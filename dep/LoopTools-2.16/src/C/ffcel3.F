#include "externals.h"
#include "types.h"


*###[ ffcel3:
	subroutine ffcel3(del3,piDpj)
***#[*comment:***********************************************************
*									*
*	Calculate del3(piDpj) = det(si.sj)	with			*
*	the momenta as follows:						*
*	p(1-3) = s(i)							*
*	p(4-6) = p(i)							*
*									*
*	Input:	piDpj(6,6)	(real)					*
*									*
*	Output:	del3		(real)	det(si.sj)			*
*									*
***#]*comment:***********************************************************
*  #[ declarations:
	implicit none
*
*	arguments:
*
	ComplexType del3,piDpj(6,6)
*
*	local variables:
*
	integer mem,nperm
	parameter(mem=10,nperm=16)
	integer i,jj(6),iperm(3,nperm),imem,memarr(mem,3),memind,inow
	ComplexType s(6),del3p,cc
	RealType xmax,xmaxp,absc
	save iperm,memind,memarr,inow
*
*	common blocks:
*
#include "ff.h"
*
*	statement function
*
	absc(cc) = abs(Re(cc)) + abs(Im(cc))
*  #] declarations:
*  #[ data:
	data memind /0/
	data memarr /mem*0,mem*0,mem*1/
	data inow /1/
*
*	these are all permutations that give a non-zero result with the
*	correct sign.  This list was generated with getperm3.
*
	data iperm/
     +		1,2,3,  1,2,5,  1,6,2,  1,4,3,
     +		1,3,5,  1,4,5,  1,6,4,  1,5,6,
     +		2,4,3,  2,3,6,  2,4,5,  2,6,4,
     +		2,5,6,  3,4,5,  3,6,4,  3,5,6/
*  #] data:
*  #[ starting point in memory?:
*
*	see if we know were to start, if not: go on as last time
*
	do 5 i=1,mem
	    if ( id .eq. memarr(i,1) .and. idsub .eq. memarr(i,2) ) then
		inow = memarr(i,3)
		goto 6
	    endif
    5	continue
    6	continue
*  #] starting point in memory?:
*  #[ calculations:
	imem = inow
	del3 = 0
	xmax = 0

   10	continue

	jj(1) = iperm(1,inow)
	jj(3) = iperm(2,inow)
	jj(5) = iperm(3,inow)

	jj(2) = iperm(1,inow)
	jj(4) = iperm(2,inow)
	jj(6) = iperm(3,inow)

	s(1) = +piDpj(jj(1),jj(2))*piDpj(jj(3),jj(4))*piDpj(jj(5),jj(6))
	s(2) = +piDpj(jj(1),jj(4))*piDpj(jj(3),jj(6))*piDpj(jj(5),jj(2))
	s(3) = +piDpj(jj(1),jj(6))*piDpj(jj(3),jj(2))*piDpj(jj(5),jj(4))
	s(4) = -piDpj(jj(1),jj(2))*piDpj(jj(3),jj(6))*piDpj(jj(5),jj(4))
	s(5) = -piDpj(jj(1),jj(6))*piDpj(jj(3),jj(4))*piDpj(jj(5),jj(2))
	s(6) = -piDpj(jj(1),jj(4))*piDpj(jj(3),jj(2))*piDpj(jj(5),jj(6))

	del3p = 0
	xmaxp = 0
	do 20 i=1,6
	    del3p = del3p + s(i)
	    xmaxp = max(xmaxp,absc(s(i)))
   20	continue
	if ( absc(del3p) .lt. xloss*xmaxp ) then
	    if ( inow .eq. imem .or. xmaxp .lt. xmax ) then
		del3 = del3p
		xmax = xmaxp
	    endif
	    inow = inow + 1
	    if ( inow .gt. nperm ) inow = 1
	    if ( inow .eq. imem ) then
		goto 800
	    endif
	    goto 10
	endif
	del3 = del3p
	xmax = xmaxp
*  #] calculations:
*  #[ into memory:
  800	continue
	memind = memind + 1
	if ( memind .gt. mem ) memind = 1
	memarr(memind,1) = id
	memarr(memind,2) = idsub
	memarr(memind,3) = inow
*  #] into memory:
*###] ffcel3:
	end
