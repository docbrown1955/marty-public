* lt.F
* the LoopTools command-line interface to Aget, Bget, Cget, Dget, Eget
* this file is part of LoopTools
* last modified 30 Jan 14 th

#include "externals.h"
#include "types.h"

#include "defs.h"


	program LoopTools
	implicit none

#include "lt.h"

#if U77EXT
	integer iargc
	external iargc
#endif

	integer argc
	character argv*100
	RealType x(Pee)
	integer i, npoint, fail
	memindex b

	memindex Aget, Bget, Cget, Dget, Eget
	integer getdebugkey
	external Aget, Bget, Cget, Dget, Eget, getdebugkey

	integer npara(5), key(5)
	data npara /Paa, Pbb, Pcc, Pdd, Pee/
	data key /KeyA0, KeyBget, KeyC0, KeyD0, KeyEget/

	argc = iargc() 
	do npoint = 1, 5
	  if( argc .eq. npara(npoint) .or.
     &        argc .eq. npara(npoint) + 1 ) goto 1
	enddo

999	print *, "Usage:  lt `parameters' [versionkey]"
	print *, "computes the n-point one-loop integrals"
	print *, "n depends on `parameters':"
	print *, "  n = 1: m"
	print *, "  n = 2: p m1 m2"
	print *, "  n = 3: p1 p2 p1p2 m1 m2 m3"
	print *, "  n = 4: p1 p2 p3 p4 p1p2 p2p3 m1 m2 m3 m4"
	print *, "  n = 5: p1 p2 p3 p4 p5 p1p2 p2p3 p3p4 p4p5 p5p1"//
     &                   " m1 m2 m3 m4 m5"
	print *, "versionkey can be one of:"
	print *, "  0 = compute version a (same as no versionkey)"
	print *, "  1 = compute version b"
	print *, "  2 = compute a and b, compare, return a"
	print *, "  3 = compute a and b, compare, return b"
	call exit(1)

1	do i = 1, npara(npoint)
	  call getarg(i, argv)
	  read(argv, *, iostat=fail, err=999) x(i)
	enddo

	call ltini

	if( argc .eq. i ) then
	  call getarg(i, argv)
	  read(argv, *, iostat=fail, err=999) i
	  if( i .lt. 0 .or. i .gt. 3 ) goto 999
	  print *, "using versionkey =", i
	  versionkey = ishft(i, key(npoint))
	endif

	call setdebugkey(ior(getdebugkey(), 2**(npoint - 1)))

	if( npoint .eq. 1 ) then
	  b = Aget(x(1))
	else if( npoint .eq. 2 ) then
	  b = Bget(x(1), x(2), x(3))
	else if( npoint .eq. 3 ) then
	  b = Cget(x(1), x(2), x(3), x(4), x(5), x(6))
	else if( npoint .eq. 4 ) then
	  b = Dget(x(1), x(2), x(3), x(4), x(5), x(6),
     &      x(7), x(8), x(9), x(10))
	else
	  b = Eget(x(1), x(2), x(3), x(4), x(5), x(6),
     &      x(7), x(8), x(9), x(10),
     &      x(11), x(12), x(13), x(14), x(15))
	endif

	call ltexi
	end

