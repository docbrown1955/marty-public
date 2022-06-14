* perm.h
* equivalent permutations for C and D functions
* this file is part of LoopTools
* last modified 9 Mar 11 th


* C-permutations
	integer p123, p231, p312
	parameter (p123 = 83)		! O'123'
	parameter (p231 = 153)		! O'231'
	parameter (p312 = 202)		! O'312'

* D-permutations
	integer p1234, p1243, p2134, p2143, p1324, p1342
	integer p3124, p3142, p1423, p1432, p4123, p4132
	integer p2314, p2341, p3214, p3241, p2413, p2431
	integer p4213, p4231, p3412, p3421, p4312, p4321
	parameter (p1234 = 175301276)	! O'123456 1234'
	parameter (p1243 = 242623139)	! O'163542 1243'
	parameter (p2134 = 226051164)	! O'153624 2134'
	parameter (p2143 = 208360547)	! O'143265 2143'
	parameter (p1324 = 718320340)	! O'526413 1324'
	parameter (p1342 = 734405346)	! O'536142 1342'
	parameter (p3124 = 701318740)	! O'516324 3124'
	parameter (p3142 = 751408738)	! O'546231 3142'
	parameter (p1423 = 643085075)	! O'462513 1423'
	parameter (p1432 = 591848218)	! O'432156 1432'
	parameter (p4123 = 558848083)	! O'412365 4123'
	parameter (p4132 = 626628698)	! O'452631 4132'
	parameter (p2314 = 362329292)	! O'254613 2314'
	parameter (p2341 = 327636193)	! O'234165 2341'
	parameter (p3214 = 294577804)	! O'214356 3214'
	parameter (p3241 = 378902177)	! O'264531 3241'
	parameter (p2413 = 883471627)	! O'645213 2413'
	parameter (p2431 = 866469145)	! O'635124 2431'
	parameter (p4213 = 833497227)	! O'615342 4213'
	parameter (p4231 = 850499737)	! O'625431 4231'
	parameter (p3412 = 472573706)	! O'341256 3412'
	parameter (p3421 = 506808081)	! O'361524 3421'
	parameter (p4312 = 490350794)	! O'351642 4312'
	parameter (p4321 = 439572689)	! O'321465 4321'

