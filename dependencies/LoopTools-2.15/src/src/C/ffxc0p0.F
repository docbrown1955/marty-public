#include "externals.h"
#include "types.h"


*###[ ffxc0p0
	subroutine ffxc0p0(cc0, xpi)
***#[*comment:***********************************************************
*									*
*	C0 function for all three momenta^2 = 0				*
*	input parameters as for ffxc0					*
*									*
*	original code from David Garcia					*
*									*
***#]*comment:*********************************************************** 
*  #[ declarations:
	implicit none
*
*	arguments
*
	ComplexType cc0
	RealType xpi(6)

	RealType m1, m2, m3, m

#include "ff.h"

	m1 = xpi(1)
	m2 = xpi(2)
	m3 = xpi(3)

* sort the masses such that m1 >= m2 >= m3
* this is important to avoid complex logs later
	if( m1 .lt. m2 ) then
	  m = m2
	  m2 = m1
	  m1 = m
	endif
	if( m2 .lt. m3 ) then
	  m = m3
	  m3 = m2
	  m2 = m
	endif
	if( m1 .lt. m2 ) then
	  m = m2
	  m2 = m1
	  m1 = m
	endif

	m = (m1 + m2 + m3)*1D-6

	if( m3 .gt. m ) then

* non-zero masses:
	  if( m2 - m3 .gt. m ) then
	    if( m1 - m2 .gt. m ) then
* m1 != m2 != m3
	      cc0 = (log(m3/m2) + m1/(m3 - m1)*log(m3/m1) -
     &          m1/(m2 - m1)*log(m2/m1))/(m2 - m3)
	    else
* m1 = m2 != m3
	      cc0 = (1 - m3/(m2 - m3)*log(m2/m3))/(m3 - m2)
	    endif
	  else
	    if( m1 - m2 .gt. m ) then
* m1 != m2 = m3
	      cc0 = (1 - m1/(m2 - m1)*log(m2/m1))/(m1 - m2)
	    else
* m1 = m2 = m3
	      cc0 = -.5D0/m1
	    endif
	  endif

	else

* zero masses:
	  if( m1 - m2 .gt. m ) then
* m1 != m2, m3 = 0
	    cc0 = log(m2/m1)/(m1 - m2)
	  else
* m1 = m2, m3 = 0
	    cc0 = -1/m1
	  endif

	endif

	end

