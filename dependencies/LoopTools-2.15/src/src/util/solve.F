* solve-LU.F
* Solution of the linear system A.x = B by LU decomposition
* with partial pivoting
* this file is part of LoopTools
* last modified 9 Apr 18 th

* Author: Michael Rauch, 7 Dec 2004
* Reference: Folkmar Bornemann, lecture notes to
* Numerische Mathematik 1, Technical University, Munich, Germany

#include "externals.h"
#include "types.h"
#include "defs.h"

#define EPS 2D0**(-51)


************************************************************************
* XDecomp computes the LU decomposition of the n-by-n matrix A
* by Gaussian Elimination with partial pivoting;
* compact (in situ) storage scheme
* Input:
*   A: n-by-n matrix to LU-decompose
*   n: dimension of A
* Output:
*   A: mangled LU decomposition of A in the form
*     ( y11 y12 ... y1n )
*     ( x21 y22 ... y2n )
*     ( x31 x32 ... y3n )
*     ( ............... )
*     ( xn1 xn2 ... ynn )
*   where 
*     (   1   0 ...   0 )  ( y11 y12 ... y1n )
*     ( x21   1 ...   0 )  (   0 y22 ... y2n )
*     ( x31 x32 ...   0 )  (   0   0 ... y3n )  =  Permutation(A)
*     ( ............... )  ( ............... )
*     ( xn1 xn2 ...   1 )  (   0   0 ... ynn ) 
*   perm: permutation vector

	subroutine XDecomp(n, A,ldA, perm)
	implicit none
	integer n, ldA, perm(*)
	ArgQuad A(ldA,*)

	integer i, j, k, pj, invperm(MAXDIM)
	ArgQuad tmp
	RealQuad absA, pabsA

	do j = 1, n
	  invperm(j) = j
	enddo

	do j = 1, n
* do U part (minus diagonal one)
	  do i = 2, j - 1
	    tmp = 0
	    do k = 1, i - 1
	      tmp = tmp + A(i,k)*A(k,j)
	    enddo
	    A(i,j) = A(i,j) - tmp
	  enddo

* do L part (plus diagonal from U case)
	  pabsA = -1
	  pj = j
	  do i = j, n
	    tmp = 0
	    do k = 1, j - 1
	      tmp = tmp + A(i,k)*A(k,j)
	    enddo
	    A(i,j) = A(i,j) - tmp

* do partial pivoting ...
* find the pivot
	    absA = abs(A(i,j))
	    if( absA .gt. pabsA ) then
	      pabsA = absA
	      pj = i
	    endif
	  enddo

	  perm(invperm(pj)) = j

* exchange rows
	  if( pj .ne. j ) then
	    invperm(pj) = invperm(j)
	    do k = 1, n
	      tmp = A(j,k)
	      A(j,k) = A(pj,k)
	      A(pj,k) = tmp
	    enddo
	  endif

* division by the pivot element
	  if( abs(A(j,j)) .gt. EPS ) then
	    tmp = 1/A(j,j)
	    do i = j + 1, n
	      A(i,j) = A(i,j)*tmp
	    enddo
	  endif
	enddo
	end

************************************************************************
* XSolve computes the x in A.x = b from the LU-decomposed A.
* Input:
*   A: LU-decomposed n-by-n matrix A
*   b: input vector b in A.x = b
*   n: dimension of A
*   p: permutation vector from LU decomposition
* Output:
*   b: solution vector x in A.x = b

	subroutine XSolve(n, A,ldA, b)
	implicit none
	integer n, ldA
	ArgQuad A(ldA,*)
	ComplexQuad b(0:2,*)

	integer i, j
	ComplexQuad tmp(0:2)

* forward substitution L.y = b
	do i = 1, n
	  tmp = 0
	  do j = 1, i - 1
	    tmp = tmp + A(i,j)*b(:,j)
	  enddo
	  b(:,i) = b(:,i) - tmp
	enddo

* backward substitution U.x = y
	do i = n, 1, -1
	  tmp = 0
	  do j = i + 1, n
	    tmp = tmp + A(i,j)*b(:,j)
	  enddo
	  b(:,i) = (b(:,i) - tmp)/A(i,i)
	enddo
	end

************************************************************************
* Det computes the determinant of a matrix.
* Input:
*   A: n-by-n matrix A
*   n: dimension of A
* Output:
*   determinant of A
* Warning: A is overwritten

	subroutine XDet(n, A,ldA, det)
	implicit none
	integer n, ldA
	ArgQuad A(ldA,*), det

	integer i, j, s, perm(MAXDIM)

	call XDecomp(n, A,ldA, perm)
	det = 1
	s = 0
	do i = 1, n
	  det = det*A(i,i)
	  j = i
	  do while( perm(j) .ne. i )
	    j = j + 1
	  enddo
	  if( j .ne. i ) then
	    perm(j) = perm(i)
	    s = s + 1
	  endif
	enddo
	if( iand(s, 1) .ne. 0 ) det = -det
	end

************************************************************************
* Inverse computes the inverse of a matrix.
* Input:
*   A: n-by-n matrix A
*   n: dimension of A
* Output:
*   A: mangled LU decomposition of A
*   Ainv: inverse of A
*   perm: permutation vector

	subroutine XInverse(n, A,ldA, Ainv,ldAinv, perm)
	implicit none
	integer n, ldA, ldAinv, perm(*)
	ArgQuad A(ldA,*), Ainv(ldAinv,*)

	integer i, j, k
	ArgQuad tmp

	call XDecomp(n, A,ldA, perm)
	do i = 1, n
	  do j = 1, n
	    Ainv(j,i) = 0
	  enddo
	  Ainv(perm(i),i) = 1

* forward substitution L.y = b
	  do j = 1, n
	    tmp = 0
	    do k = 1, j - 1
	      tmp = tmp + A(j,k)*Ainv(k,i)
	    enddo
	    Ainv(j,i) = Ainv(j,i) - tmp
	  enddo

* backward substitution U.x = y
	  do j = n, 1, -1
	    tmp = 0
	    do k = j + 1, n
	      tmp = tmp + A(j,k)*Ainv(k,i)
	    enddo
	    Ainv(j,i) = (Ainv(j,i) - tmp)/A(j,j)
	  enddo
	enddo
	end

