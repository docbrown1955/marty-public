:Evaluate: BeginPackage["LoopTools`"]

:Evaluate:
	A0i::usage = "A0i[id, m] is the generic one-point loop integral.
	A0i[aa0, m] is the scalar function A_0, A0i[aa00, m] is the tensor coefficient A_00.
	m is the mass squared.";
	Aget::usage = "Aget[m] returns a list of all one-point coefficients."

:Evaluate:
	B0i::usage = "B0i[id, p, m1, m2] is the generic two-point loop integral which includes both scalar and tensor coefficients, as well as certain derivatives.
	B0i[bb0, ...] is the scalar function B_0, B0i[bb11, ...] the tensor coefficient function B_11 etc.
	p is the external momentum squared and m1 and m2 are the masses squared.";
	Bget::usage = "Bget[p, m1, m2] returns a list of all two-point coefficients."

:Evaluate:
	C0i::usage = "C0i[id, p1, p2, p1p2, m1, m2, m3] is the generic three-point loop integral which includes both scalar and tensor coefficients, specified by id.
	C0i[cc0, ...] is the scalar function C_0, C0i[cc112, ...] the tensor coefficient function C_112 etc.
	p1, p2, and p1p2 are the external momenta squared and m1, m2, m3 are the masses squared.";
	Cget::usage = "Cget[p1, p2, p1p2, m1, m2, m3] returns a list of all three-point coefficients."

:Evaluate:
	D0i::usage = "D0i[id, p1, p2, p3, p4, p1p2, p2p3, m1, m2, m3, m4] is the generic four-point loop integral which includes both scalar and tensor coefficients, specified by id.
	D0i[dd0, ...] is the scalar function D_0, D0i[dd1233, ...] the tensor function D_{1233} etc.
	p1...p4 are the external momenta squared, p1p2 and p2p3 are the squares of external momenta 1 + 2 and 2 + 3, respectively, and m1...m4 are the masses squared.";
	Dget::usage = "Dget[p1, p2, p3, p4, p1p2, p2p3, m1, m2, m3, m4] returns a list of all four-point coefficients."

:Evaluate:
	E0i::usage = "E0i[id, p1, p2, p3, p4, p5, p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5] is the generic five-point loop integral which includes both scalar and tensor coefficients, specified by id.
	E0i[ee0, ...] is the scalar function E_0, E0i[ee3444, ...] the tensor function E_{3444} etc.
	p1...p5 are the external momenta squared, pipj are the squares of (pi + pj), and m1...m5 are the masses squared.";
	Eget::usage = "Eget[p1, p2, p3, p4, p5, p1p2, p2p3, p3p4, p4p5, p5p1, m1, m2, m3, m4, m5] returns a list of all five-point coefficients."

:Evaluate: PaVe::usage = "PaVe[ind, {pi}, {mi}] is the generalized Passarino-Veltman function used by FeynCalc.
	It is converted to A0i, B0i, C0i, D0i, or E0i in LoopTools."

:Evaluate: Li2::usage = "Li2[x] returns the dilogarithm of x."

:Evaluate: Li2omx::usage = "Li2omx[x] returns the dilogarithm of 1 - x."

:Evaluate:
	SetMudim::usage = "SetMudim[m^2] sets the renormalization scale squared.";
	GetMudim::usage = "GetMudim[] returns the current value for the renormalization scale squared."

:Evaluate:
	SetDelta::usage = "SetDelta[d] sets the numerical value of Delta which replaces the finite part of the divergence 2/(4 - D) - EulerGamma + Log[4 Pi] in LoopTools.";
	GetDelta::usage = "GetDelta[] returns the current numerical value of Delta which replaces the finite part of the divergence 2/(4 - D) - EulerGamma + Log[4 Pi] in LoopTools."

:Evaluate:
	SetUVDiv::usage = "SetUVDiv[uvdiv] turns the UV part of the eps^-1 component off (uvdiv = 0) and on (uvdiv = 1).";
	GetUVDiv::usage = "GetUVDiv[] returns whether the UV part of the eps^-1 component is currently off (0) or on (1)."

:Evaluate:
	SetLambda::usage = "SetLambda[l^2] sets the infrared regulator mass squared.";
	GetLambda::usage = "GetLambda[] returns the current value for the infrared regulator mass squared."

:Evaluate:
	SetMinMass::usage = "SetMinMass[m^2] sets the collinear cutoff mass squared.";
	GetMinMass::usage = "GetMinMass[] returns the current value for the collinear cutoff mass squared."

:Evaluate:
	ClearCache::usage = "ClearCache[] clears the internal LoopTools caches.";
	MarkCache::usage = "MarkCache[] marks the current positions of the internal LoopTools caches.";
	RestoreCache::usage = "RestoreCache[] restores the internal LoopTools caches to the position when the last MarkCache was issued."

:Evaluate:
	SetMaxDev::usage = "SetMaxDev[d] sets the maximum relative deviation a result and its alternate derivation may have before a warning is issued.";
	GetMaxDev::usage = "GetMaxDev[d] returns the maximum relative deviation a result and its alternate derivation may have before a warning is issued."

:Evaluate:
	SetWarnDigits::usage = "SetWarnDigits[n] sets the number of LoopTools' warning digits.
	If the number of digits presumed lost by FF is larger than the warning digits, either an alternate version is tried (if available) or a warning is issued.";
	GetWarnDigits::usage = "GetWarnDigits[] returns the number of LoopTools' warning digits.
	If the number of digits presumed lost by FF is larger than the warning digits, either an alternate version is tried (if available) or a warning is issued."

:Evaluate:
	SetErrDigits::usage = "SetErrDigits[n] sets the number of LoopTools' error digits.
	If the number of digits presumed lost by FF is larger than the error digits, the alternate result is used instead of the FF result.";
	GetErrDigits::usage = "GetErrDigits[] returns the number of LoopTools' error digits.
	If the number of digits presumed lost by FF is larger than the error digits, the alternate result is used instead of the FF result."

:Evaluate:
	SetVersionKey::usage = "SetVersionKey[key] sets the LoopTools version key.
	It determines which version of a loop integral is returned, and whether checks are performed.";
	GetVersionKey::usage = "GetVersionKey[] returns the LoopTools version key.
	It determines which version of a loop integral is returned, and whether checks are performed."

:Evaluate:
	SetDebugKey::usage = "SetDebugKey[key] sets the LoopTools debug key.
	It determines how much debug information is printed for a loop integral.";
	GetDebugKey::usage = "GetDebugKey[] returns the LoopTools debug key.
	It determines how much debug information is printed for a loop integral.";
	SetDebugRange::usage = "SetDebugRange[from, to] sets the LoopTools debug range.
	The integrals printed out on screen as determined by the debug key are numbered consecutively.
	Setting a debug range restricts printing to the given range."

:Evaluate:
	SetCmpBits::usage = "SetCmpBits[bits] sets the number of bits compared in cache lookups.
	Setting it to less than 64 (double precision) makes the comparison more robust against numerical noise.";
	GetCmpBits::usage = "GetCmpBits[] returns the number of bits compared of each real number in cache lookups."

:Evaluate:
	SetDiffEps::usage = "SetDiffEps[diffeps] sets the tolerance in comparing two numbers, i.e. a and b are considered equal if |a - b| < diffeps.";
	GetDiffEps::usage = "GetDiffEps[] returns the tolerance used for comparing two numbers.";
	SetZeroEps::usage = "SetZeroEps[zeroeps] sets the tolerance in determining that a number is zero, i.e. a is considered zero if |a| < zeroeps.";
	GetZeroEps::usage = "GetZeroEps[] returns the tolerance used in testing numbers for zero."

:Evaluate:
	LTini::usage = "LTini[] (re-)initializes LoopTools.";
	LTexi::usage = "LTexi[] gives a summary of all errors and warnings since the last LTini[].";
	LTnop::usage = "LTnop[] does nothing.";
	LTwrite::usage = "LTwrite[s] is the function LoopTools calls for printing s."

:Evaluate:
	DRResult::usage = "DRResult[c0, c1, c2] arranges the coefficients of DR1eps into the final returned to the user.";
	DR1eps::usage = "DR1eps represents 1/eps where D = 4 - 2 eps."

:Evaluate:
	LTids = Thread[# -> 3 Range[Length[#]] - 2]&/@ {
	  {aa0, aa00},
	  {bb0, bb1, bb00, bb11, bb001, bb111,
	    dbb0, dbb1, dbb00, dbb11, dbb001},
	  {cc0, cc1, cc2, cc00, cc11, cc12, cc22,
	    cc001, cc002, cc111, cc112, cc122, cc222,
	    cc0000, cc0011, cc0012, cc0022, cc1111, cc1112,
	    cc1122, cc1222, cc2222},
	  {dd0, dd1, dd2, dd3, dd00, dd11, dd12, dd13, dd22,
	    dd23, dd33, dd001, dd002, dd003, dd111, dd112, dd113,
	    dd122, dd123, dd133, dd222, dd223, dd233, dd333,
	    dd0000, dd0011, dd0012, dd0013, dd0022, dd0023,
	    dd0033, dd1111, dd1112, dd1113, dd1122, dd1123,
	    dd1133, dd1222, dd1223, dd1233, dd1333, dd2222,
	    dd2223, dd2233, dd2333, dd3333, dd00001, dd00002,
	    dd00003, dd00111, dd00112, dd00113, dd00122, dd00123,
	    dd00133, dd00222, dd00223, dd00233, dd00333, dd11111,
	    dd11112, dd11113, dd11122, dd11123, dd11133, dd11222,
	    dd11223, dd11233, dd11333, dd12222, dd12223, dd12233,
	    dd12333, dd13333, dd22222, dd22223, dd22233, dd22333,
	    dd23333, dd33333},
	  {ee0, ee1, ee2, ee3, ee4, ee00, ee11, ee12, ee13, 
	    ee14, ee22, ee23, ee24, ee33, ee34, ee44, ee001,
	    ee002, ee003, ee004, ee111, ee112, ee113, ee114,
	    ee122, ee123, ee124, ee133, ee134, ee144, ee222,
	    ee223, ee224, ee233, ee234, ee244, ee333, ee334,
	    ee344, ee444, ee0000, ee0011, ee0012, ee0013, ee0014,
	    ee0022, ee0023, ee0024, ee0033, ee0034, ee0044, ee1111,
	    ee1112, ee1113, ee1114, ee1122, ee1123, ee1124, ee1133,
	    ee1134, ee1144, ee1222, ee1223, ee1224, ee1233, ee1234,
	    ee1244, ee1333, ee1334, ee1344, ee1444, ee2222, ee2223,
	    ee2224, ee2233, ee2234, ee2244, ee2333, ee2334, ee2344,
	    ee2444, ee3333, ee3334, ee3344, ee3444, ee4444} }

:Evaluate:
	KeyAll = Plus@@ ({KeyA0, KeyBget, KeyC0, KeyD0, KeyE0,
	  KeyEget, KeyCEget} = 4^Range[0, 6]);
	DebugAll = Plus@@ ({DebugA, DebugB, DebugC, DebugD, DebugE} =
	  2^Range[0, 4])

:Evaluate:
	A0 = A0i[aa0, ##]&;
	A00 = A0i[aa00, ##]&;
	B0 = B0i[bb0, ##]&;
	B1 = B0i[bb1, ##]&;
	B00 = B0i[bb00, ##]&;
	B11 = B0i[bb11, ##]&;
	B001 = B0i[bb001, ##]&;
	B111 = B0i[bb111, ##]&;
	DB0 = B0i[dbb0, ##]&;
	DB1 = B0i[dbb1, ##]&;
	DB00 = B0i[dbb00, ##]&;
	DB11 = B0i[dbb11, ##]&;
	DB001 = B0i[dbb001, ##]&;
	C0 = C0i[cc0, ##]&;
	D0 = D0i[dd0, ##]&;
	E0 = E0i[ee0, ##]&

:Evaluate:
	If[!ValueQ[LTwrite], LTwrite = WriteString[$Output, #]&]

:Evaluate: Begin["`Private`"]

:Begin:
:Function:	mA0i
:Pattern:	A0i[id_, m_?r]
:Arguments:	{id /. LTids[[1]], N[m]}
:ArgumentTypes:	{Integer, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mA0ic
:Pattern:	A0i[id_, m_?c]
:Arguments:	{id /. LTids[[1]], N[Re[m]], N[Im[m]]}
:ArgumentTypes:	{Integer, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mAget
:Pattern:	Aget[m_?r]
:Arguments:	{N[m]}
:ArgumentTypes:	{Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mAgetc
:Pattern:	Aget[m_?c]
:Arguments:	{N[Re[m]], N[Im[m]]}
:ArgumentTypes:	{Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mB0i
:Pattern:	B0i[id_, p_?r, m1_?r, m2_?r]
:Arguments:	{id /. LTids[[2]], N[p], N[m1], N[m2]}
:ArgumentTypes:	{Integer, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mB0ic
:Pattern:	B0i[id_, p_?c, m1_?c, m2_?c]
:Arguments:	{id /. LTids[[2]], N[Re[p]], N[Im[p]],
		  N[Re[m1]], N[Im[m1]], N[Re[m2]], N[Im[m2]]}
:ArgumentTypes:	{Integer, Real, Real, Real, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mBget
:Pattern:	Bget[p_?r, m1_?r, m2_?r]
:Arguments:	{N[p], N[m1], N[m2]}
:ArgumentTypes:	{Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mBgetc
:Pattern:	Bget[p_?c, m1_?c, m2_?c]
:Arguments:	{N[Re[p]], N[Im[p]],
		  N[Re[m1]], N[Im[m1]], N[Re[m2]], N[Im[m2]]}
:ArgumentTypes:	{Real, Real, Real, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mC0i
:Pattern:	C0i[id_, p1_?r, p2_?r, p1p2_?r, m1_?r, m2_?r, m3_?r]
:Arguments:	{id /. LTids[[3]],
		  N[p1], N[p2], N[p1p2], N[m1], N[m2], N[m3]}
:ArgumentTypes:	{Integer, Real, Real, Real, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mC0ic
:Pattern:	C0i[id_, p1_?c, p2_?c, p1p2_?c, m1_?c, m2_?c, m3_?c]
:Arguments:	{id /. LTids[[3]],
		  N[Re[p1]], N[Im[p1]], N[Re[p2]], N[Im[p2]],
		  N[Re[p1p2]], N[Im[p1p2]], N[Re[m1]], N[Im[m1]],
		  N[Re[m2]], N[Im[m2]], N[Re[m3]], N[Im[m3]]}
:ArgumentTypes:	{Integer,
		  Real, Real, Real, Real, Real, Real,
		  Real, Real, Real, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mCget
:Pattern:	Cget[p1_?r, p2_?r, p1p2_?r, m1_?r, m2_?r, m3_?r]
:Arguments:	{N[p1], N[p2], N[p1p2], N[m1], N[m2], N[m3]}
:ArgumentTypes:	{Real, Real, Real, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mCgetc
:Pattern:	Cget[p1_?c, p2_?c, p1p2_?c, m1_?c, m2_?c, m3_?c]
:Arguments:	{N[Re[p1]], N[Im[p1]], N[Re[p2]], N[Im[p2]],
		  N[Re[p1p2]], N[Im[p1p2]], N[Re[m1]], N[Im[m1]],
		  N[Re[m2]], N[Im[m2]], N[Re[m3]], N[Im[m3]]}
:ArgumentTypes:	{Real, Real, Real, Real, Real, Real,
		  Real, Real, Real, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mD0i
:Pattern:	D0i[id_, p1_?r, p2_?r, p3_?r, p4_?r, p1p2_?r, p2p3_?r,
		  m1_?r, m2_?r, m3_?r, m4_?r]
:Arguments:	{id /. LTids[[4]],
		  N[p1], N[p2], N[p3], N[p4], N[p1p2], N[p2p3],
		  N[m1], N[m2], N[m3], N[m4]}
:ArgumentTypes:	{Integer,
		  Real, Real, Real, Real, Real, Real,
		  Real, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mD0ic
:Pattern:	D0i[id_, p1_?c, p2_?c, p3_?c, p4_?c, p1p2_?c, p2p3_?c,
		  m1_?c, m2_?c, m3_?c, m4_?c]
:Arguments:	{id /. LTids[[4]],
		  N[Re[p1]], N[Im[p1]], N[Re[p2]], N[Im[p2]],
		  N[Re[p3]], N[Im[p3]], N[Re[p4]], N[Im[p4]],
		  N[Re[p1p2]], N[Im[p1p2]], N[Re[p2p3]], N[Im[p2p3]],
		  N[Re[m1]], N[Im[m1]], N[Re[m2]], N[Im[m2]],
		  N[Re[m3]], N[Im[m3]], N[Re[m4]], N[Im[m4]]}
:ArgumentTypes:	{Integer,
		  Real, Real, Real, Real, Real, Real, Real, Real,
		  Real, Real, Real, Real, Real, Real, Real, Real,
		  Real, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mDget
:Pattern:	Dget[p1_?r, p2_?r, p3_?r, p4_?r, p1p2_?r, p2p3_?r,
		  m1_?r, m2_?r, m3_?r, m4_?r]
:Arguments:	{N[p1], N[p2], N[p3], N[p4], N[p1p2], N[p2p3],
		  N[m1], N[m2], N[m3], N[m4]}
:ArgumentTypes:	{Real, Real, Real, Real, Real, Real, Real,
		  Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mDgetc
:Pattern:	Dget[p1_?c, p2_?c, p3_?c, p4_?c, p1p2_?c, p2p3_?c,
		  m1_?c, m2_?c, m3_?c, m4_?c]
:Arguments:	{N[Re[p1]], N[Im[p1]], N[Re[p2]], N[Im[p2]],
		  N[Re[p3]], N[Im[p3]], N[Re[p4]], N[Im[p4]],
		  N[Re[p1p2]], N[Im[p1p2]], N[Re[p2p3]], N[Im[p2p3]],
		  N[Re[m1]], N[Im[m1]], N[Re[m2]], N[Im[m2]],
		  N[Re[m3]], N[Im[m3]], N[Re[m4]], N[Im[m4]]}
:ArgumentTypes:	{Real, Real, Real, Real, Real, Real, Real, Real,
		  Real, Real, Real, Real, Real, Real, Real, Real,
		  Real, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mE0i
:Pattern:	E0i[id_, p1_?r, p2_?r, p3_?r, p4_?r, p5_?r,
		  p1p2_?r, p2p3_?r, p3p4_?r, p4p5_?r, p5p1_?r,
		  m1_?r, m2_?r, m3_?r, m4_?r, m5_?r]
:Arguments:	{id /. LTids[[5]],
		  N[p1], N[p2], N[p3], N[p4], N[p5], 
		  N[p1p2], N[p2p3], N[p3p4], N[p4p5], N[p5p1],
		  N[m1], N[m2], N[m3], N[m4], N[m5]}
:ArgumentTypes:	{Integer,
		  Real, Real, Real, Real, Real, 
		  Real, Real, Real, Real, Real,
		  Real, Real, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mE0ic
:Pattern:	E0i[id_, p1_?c, p2_?c, p3_?c, p4_?c, p5_?c,
		  p1p2_?c, p2p3_?c, p3p4_?c, p4p5_?c, p5p1_?c,
		  m1_?c, m2_?c, m3_?c, m4_?c, m5_?c]
:Arguments:	{id /. LTids[[5]],
		  N[Re[p1]], N[Im[p1]], N[Re[p2]], N[Im[p2]],
		  N[Re[p3]], N[Im[p3]], N[Re[p4]], N[Im[p4]],
		  N[Re[p5]], N[Im[p5]], N[Re[p1p2]], N[Im[p1p2]], 
		  N[Re[p2p3]], N[Im[p2p3]], N[Re[p3p4]], N[Im[p3p4]],
		  N[Re[p4p5]], N[Im[p4p5]], N[Re[p5p1]], N[Im[p5p1]],
		  N[Re[m1]], N[Im[m1]], N[Re[m2]], N[Im[m2]],
		  N[Re[m3]], N[Im[m3]], N[Re[m4]], N[Im[m4]],
		  N[Re[m5]], N[Im[m5]]}
:ArgumentTypes:	{Integer, Real, Real, Real, Real, Real, 
		  Real, Real, Real, Real, Real, 
		  Real, Real, Real, Real, Real, 
		  Real, Real, Real, Real, Real, 
		  Real, Real, Real, Real, Real, 
		  Real, Real, Real, Real, Real} 
:ReturnType:	Manual
:End:

:Begin:
:Function:	mEget
:Pattern:	Eget[p1_?r, p2_?r, p3_?r, p4_?r, p5_?r,
		  p1p2_?r, p2p3_?r, p3p4_?r, p4p5_?r, p5p1_?r,
		  m1_?r, m2_?r, m3_?r, m4_?r, m5_?r]
:Arguments:	{N[p1], N[p2], N[p3], N[p4], N[p5],
		  N[p1p2], N[p2p3], N[p3p4], N[p4p5], N[p5p1],
		  N[m1], N[m2], N[m3], N[m4], N[m5]}
:ArgumentTypes:	{Real, Real, Real, Real, Real, 
		  Real, Real, Real, Real, Real,
		  Real, Real, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mEgetc
:Pattern:	Eget[p1_?c, p2_?c, p3_?c, p4_?c, p5_?c,
		  p1p2_?c, p2p3_?c, p3p4_?c,  p4p5_?c, p5p1_?c,
		  m1_?c, m2_?c, m3_?c, m4_?c, m5_?c]
:Arguments:	{N[Re[p1]], N[Im[p1]], N[Re[p2]], N[Im[p2]],
		  N[Re[p3]], N[Im[p3]], N[Re[p4]], N[Im[p4]],
		  N[Re[p5]], N[Im[p5]], N[Re[p1p2]], N[Im[p1p2]], 
		  N[Re[p2p3]], N[Im[p2p3]], N[Re[p3p4]], N[Im[p3p4]],
		  N[Re[p4p5]], N[Im[p4p5]], N[Re[p5p1]], N[Im[p5p1]],
		  N[Re[m1]], N[Im[m1]], N[Re[m2]], N[Im[m2]],
		  N[Re[m3]], N[Im[m3]], N[Re[m4]], N[Im[m4]],
		  N[Re[m5]], N[Im[m5]]}
:ArgumentTypes:	{Real, Real, Real, Real, Real,
		  Real, Real, Real, Real, Real,
		  Real, Real, Real, Real, Real,
		  Real, Real, Real, Real, Real,
		  Real, Real, Real, Real, Real,
		  Real, Real, Real, Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mLi2
:Pattern:	Li2[x_?r]
:Arguments:	{N[x]}
:ArgumentTypes:	{Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mLi2c
:Pattern:	Li2[x_?c]
:Arguments:	{N[Re[x]], N[Im[x]]}
:ArgumentTypes:	{Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mLi2omx
:Pattern:	Li2omx[x_?r]
:Arguments:	{N[x]}
:ArgumentTypes:	{Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mLi2omxc
:Pattern:	Li2omx[x_?c]
:Arguments:	{N[Re[x]], N[Im[x]]}
:ArgumentTypes:	{Real, Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetmudim
:Pattern:	SetMudim[mudim_?r]
:Arguments:	{N[mudim]}
:ArgumentTypes:	{Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetmudim
:Pattern:	GetMudim[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetdelta
:Pattern:	SetDelta[delta_?r]
:Arguments:	{N[delta]}
:ArgumentTypes:	{Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetdelta
:Pattern:	GetDelta[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetuvdiv
:Pattern:	SetUVDiv[uvdiv_?r]
:Arguments:	{N[uvdiv]}
:ArgumentTypes:	{Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetuvdiv
:Pattern:	GetUVDiv[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetlambda
:Pattern:	SetLambda[lambda_?r]
:Arguments:	{N[lambda]}
:ArgumentTypes:	{Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetlambda
:Pattern:	GetLambda[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetepsi
:Pattern:	GetEpsi[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetminmass
:Pattern:	SetMinMass[minmass_?r]
:Arguments:	{N[minmass]}
:ArgumentTypes:	{Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetminmass
:Pattern:	GetMinMass[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mclearcache
:Pattern:	ClearCache[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mmarkcache
:Pattern:	MarkCache[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mrestorecache
:Pattern:	RestoreCache[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetmaxdev
:Pattern:	SetMaxDev[maxdev_?r]
:Arguments:	{N[maxdev]}
:ArgumentTypes:	{Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetmaxdev
:Pattern:	GetMaxDev[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetwarndigits
:Pattern:	SetWarnDigits[warndigits_Integer]
:Arguments:	{warndigits}
:ArgumentTypes:	{Integer}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetwarndigits
:Pattern:	GetWarnDigits[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mseterrdigits
:Pattern:	SetErrDigits[errdigits_Integer]
:Arguments:	{errdigits}
:ArgumentTypes:	{Integer}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgeterrdigits
:Pattern:	GetErrDigits[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetversionkey
:Pattern:	SetVersionKey[versionkey_Integer]
:Arguments:	{versionkey}
:ArgumentTypes:	{Integer}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetversionkey
:Pattern:	GetVersionKey[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetdebugkey
:Pattern:	SetDebugKey[debugkey_Integer]
:Arguments:	{debugkey}
:ArgumentTypes:	{Integer}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetdebugkey
:Pattern:	GetDebugKey[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetdebugrange
:Pattern:	SetDebugRange[debugfrom_Integer, debugto_Integer]
:Arguments:	{debugfrom, debugto}
:ArgumentTypes:	{Integer, Integer}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetcmpbits
:Pattern:	SetCmpBits[cmpbits_Integer]
:Arguments:	{cmpbits}
:ArgumentTypes:	{Integer}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetcmpbits
:Pattern:	GetCmpBits[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetdiffeps
:Pattern:	SetDiffEps[diffeps]
:Arguments:	{diffeps}
:ArgumentTypes:	{Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetdiffeps
:Pattern:	GetDiffEps[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	msetzeroeps
:Pattern:	SetZeroEps[zeroeps]
:Arguments:	{zeroeps}
:ArgumentTypes:	{Real}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mgetzeroeps
:Pattern:	GetZeroEps[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mltini
:Pattern:	LTini[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mltexi
:Pattern:	LTexi[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Begin:
:Function:	mltnop
:Pattern:	LTnop[]
:Arguments:	{}
:ArgumentTypes:	{}
:ReturnType:	Manual
:End:

:Evaluate: r = Head[# + 1.] === Real &

:Evaluate: c = Head[# + 1. I] === Complex &

:Evaluate: A0i[_, 0] = 0

:Evaluate: MapThread[
	(Derivative[0,1,0,0][B0i][#1, args__] := B0i[#2, args])&,
	{{bb0, bb1, bb00, bb11, bb001},
	 {dbb0, dbb1, dbb00, dbb11, dbb001}} ]

:Evaluate: PaVe[i__Integer, {p___}, {m__}] :=
	ToExpression[#1 <> "0i"][
	  ToExpression[#2 <> #2 <> ToString/@ Sort[{i}]], p, m ]&[
	  FromCharacterCode[Length[{m}] + 64],
	  FromCharacterCode[Length[{m}] + 96] ]

:Evaluate:
	DRResult[c0_, c1_, c2_] := c0 + c1 DR1eps + c2 DR1eps^2;
	idlist[n_, x_] := MapThread[#1[[1]] -> #2 &, {LTids[[n]],
	  DRResult@@@ Partition[
	    Complex@@@ Partition[Chop[x, 10^-14], 2], 3]}]

:Evaluate:
	ltdef1[off_][id_, n_] :=
	  {"#define ", ToString[id], " ", ToString[n + off], "\n"};
	ltdef2[off_][id_, n_] := {##,
	  #1, ToUpperCase[#2], #3, #4, ":", ToString[n + off + 2], #5}&@@
	  ltdef1[off][id, n];
	ltdefs[off_, h_] := StringJoin@@ MapThread[{ h[off]@@@ #,
	  "#define ", #2, " ", ToString[3 Length[#]], "\n\n" }&,
	  {LTids, {"Naa", "Nbb", "Ncc", "Ndd", "Nee"}}];
	ltwritedefs[] := {
	  WriteString["for_looptools.h", ltdefs[0, ltdef1]];
	  WriteString["for_clooptools.h.in", ltdefs[-1, ltdef1]];
	  WriteString["for_defs.h", ltdefs[0, ltdef2]] }

:Evaluate: End[]

:Evaluate: EndPackage[]

:Evaluate:
	($Post := (LTnop[]; OwnValues[$Post] = #; $Post /. $Post -> Identity))& @
	  OwnValues[$Post];
	($Epilog := (LTexi[]; OwnValues[$Epilog] = #; $Epilog))& @
	  OwnValues[$Epilog];

/*
	LoopTools.tm
		provides the LoopTools functions in Mathematica
		this file is part of LoopTools
		last modified 28 Aug 20 th
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include "mathlink.h"
#ifndef MLCONST
#define MLCONST
#endif

#define CQUADSIZE 10
#include "clooptools.h"

typedef unsigned char byte;
typedef MLCONST char cchar;
typedef const int cint;
typedef const long clong;

#if QUAD
#define MLPutREAL MLPutReal128
static inline void MLPutREALList(MLINK mlp, CREAL *s, long n) {
  RealType d[n];
  int i;
  for( i = 0; i < n; ++i ) d[i] = ToReal(s[i]);
  MLPutReal128List(mlp, d, n);
}
#else
#define MLPutREAL MLPutReal
#define MLPutREALList MLPutRealList
#endif

static int stdoutorig = -1, stdoutpipe[2] = {2, 2}, stdoutthr = 0;
static byte *stdoutbuf = NULL;

//#define DEBUG
#ifdef DEBUG
static FILE *deb;
#define DEB(...) fprintf(deb, __VA_ARGS__)
#else
#define DEB(...)
#endif

static inline void mlPutFunction(MLINK mlp, cchar *fun, cint args) {
  MLPutFunction(mlp, fun, args);
  DEB("MLPutFunction(\"%s\", %d)\n", fun, args);
}

static inline void mlPutSymbol(MLINK mlp, cchar *sym) {
  MLPutSymbol(mlp, sym);
  DEB("MLPutSymbol(\"%s\")\n", sym);
}

static inline void mlPutReal(MLINK mlp, cRealType r) {
  MLPutReal(mlp, r);
  DEB("MLPutReal(%lg)\n", r);
}

static inline void mlPutInteger(MLINK mlp, cint i) {
  MLPutInteger(mlp, i);
  DEB("MLPutInteger(%d)\n", i);
}

static inline void mlPutByteString(MLINK mlp, byte *s, size_t len) {
  MLPutByteString(mlp, s, len);
  DEB("MLPutByteString(\"%*s\", %lu)\n", (int)len, s, len);
}

static inline void mlEndPacket(MLINK mlp) {
  MLEndPacket(mlp);
  DEB("MLEndPacket\n");
}

static inline void mlNewPacket(MLINK mlp) {
  MLNewPacket(mlp);
  DEB("MLNewPacket\n");
}

static inline void mlNextPacket(MLINK mlp) {
  MLNextPacket(mlp);
  DEB("MLNextPacket\n");
}

/******************************************************************/

#define CaptureStdout() \
  DEB("%s\n", __FUNCTION__); \
  dup2(stdoutpipe[1], 1)

#define ReleaseStdout() \
  dup2(stdoutorig, 1)

static inline void mlPutStdout(MLINK mlp) {
  long len;
  extern void FORTRAN(fortranflush)();
char buf[1024];

  FORTRAN(fortranflush)();
  fflush(stdout);

  if( stdoutthr ) {
    write(1, "", 1);
    read(1, &len, sizeof len);
    if( len > 1 ) {
      DEB("MLPutStdout <<\n");
      mlPutFunction(mlp, "EvaluatePacket", 1);
      mlPutFunction(mlp, "LoopTools`LTwrite", 1);
      mlPutByteString(mlp, stdoutbuf, len - 1);
      mlNextPacket(stdlink);
      mlNewPacket(stdlink);
      DEB(">>\n");
    }
  }

  ReleaseStdout();
}

/******************************************************************/

static void *capturestdout(void *pfd) {
  cint fd = ((int *)pfd)[0];
  long size = 0, len = 0, n;
  enum { unit = 10240 };

  do {
    if( size - len < 128 ) stdoutbuf = realloc(stdoutbuf, size += unit);
    len += n = read(fd, stdoutbuf + len, size - len);
    if( len > 0 && stdoutbuf[len-1] == 0 ) {
      write(fd, &len, sizeof len);
      len = 0;
    }
  } while( n > 0 );

  return NULL;
}

/******************************************************************/

#define SetReal(fun,val) \
  CaptureStdout(); \
  fun(val); \
  mlPutStdout(stdlink); \
  mlPutReal(stdlink, val); \
  mlEndPacket(stdlink)

#define GetReal(fun) \
  mlPutReal(stdlink, fun()); \
  mlEndPacket(stdlink)

#define SetInteger(fun,val) \
  CaptureStdout(); \
  fun(val); \
  mlPutStdout(stdlink); \
  mlPutInteger(stdlink, val); \
  mlEndPacket(stdlink)

#define GetInteger(fun) \
  mlPutInteger(stdlink, fun()); \
  mlEndPacket(stdlink)

/******************************************************************/

#define ReturnComplex(expr) \
  ComplexType result; \
  CaptureStdout(); \
  result = expr; \
  mlPutStdout(stdlink); \
  mlPutComplex(stdlink, result); \
  mlEndPacket(stdlink)

#define ReturnList(i, expr, n) \
  COMPLEX *list; \
  CaptureStdout(); \
  list = expr; \
  mlPutStdout(stdlink); \
  mlPutList(stdlink, i, list, n); \
  mlEndPacket(stdlink)

#define ReturnVoid() \
  mlPutSymbol(stdlink, "Null"); \
  mlEndPacket(stdlink)

#define _Id_(v) v
#define _Mr_(v) cRealType v
#define _Mri_(v) cRealType re_##v, cRealType im_##v
#define _Mc_(v) ToComplex2(re_##v, im_##v)

/******************************************************************/

static inline void mlPutComplex(MLINK mlp, cComplexType c) {
  DEB("MLPutComplex(%lg,%lg)\n", Re(c), Im(c));
  if( Im(c) == 0 ) MLPutREAL(mlp, Re(c));
  else {
    MLPutFunction(mlp, "Complex", 2);
    MLPutREAL(mlp, Re(c));
    MLPutREAL(mlp, Im(c));
  }
}

/******************************************************************/

static inline void mlPutList(MLINK mlp, cint i, COMPLEX *list, cint n) {
  DEB("MLPutList(%d, %d)\n", i, n);
  MLPutFunction(mlp, "LoopTools`Private`idlist", 2);
  MLPutInteger(mlp, i);
  MLPutREALList(mlp, (REAL *)list, 2*n);
}

/******************************************************************/

static void mA0i(cint i, AARGS(_Mr_)) {
  ReturnComplex(A0i(i-1, AARGS(_Id_)));
}

static void mA0ic(cint i, AARGS(_Mri_)) {
  ReturnComplex(A0iC(i-1, AARGS(_Mc_)));
}

/******************************************************************/

static void mAget(AARGS(_Mr_)) {
  ReturnList(1, Acache(Aget(AARGS(_Id_))), Naa);
}

static void mAgetc(AARGS(_Mri_)) {
  ReturnList(1, AcacheC(AgetC(AARGS(_Mc_))), Naa);
}

/******************************************************************/

static void mB0i(cint i, BARGS(_Mr_)) {
  ReturnComplex(B0i(i-1, BARGS(_Id_)));
}

static void mB0ic(cint i, BARGS(_Mri_)) {
  ReturnComplex(B0iC(i-1, BARGS(_Mc_)));
}

/******************************************************************/

static void mBget(BARGS(_Mr_)) {
  ReturnList(2, Bcache(Bget(BARGS(_Id_))), Nbb);
}

static void mBgetc(BARGS(_Mri_)) {
  ReturnList(2, BcacheC(BgetC(BARGS(_Mc_))), Nbb);
}

/******************************************************************/

static void mC0i(cint i, CARGS(_Mr_)) {
  ReturnComplex(C0i(i-1, CARGS(_Id_)));
}

static void mC0ic(cint i, CARGS(_Mri_)) {
  ReturnComplex(C0iC(i-1, CARGS(_Mc_)));
}

/******************************************************************/

static void mCget(CARGS(_Mr_)) {
  ReturnList(3, Ccache(Cget(CARGS(_Id_))), Ncc);
}

static void mCgetc(CARGS(_Mri_)) {
  ReturnList(3, CcacheC(CgetC(CARGS(_Mc_))), Ncc);
}

/******************************************************************/

static void mD0i(cint i, DARGS(_Mr_)) {
  ReturnComplex(D0i(i-1, DARGS(_Id_)));
}

static void mD0ic(cint i, DARGS(_Mri_)) {
  ReturnComplex(D0iC(i-1, DARGS(_Mc_)));
}

/******************************************************************/

static void mDget(DARGS(_Mr_)) {
  ReturnList(4, Dcache(Dget(DARGS(_Id_))), Ndd);
}

static void mDgetc(DARGS(_Mri_)) {
  ReturnList(4, DcacheC(DgetC(DARGS(_Mc_))), Ndd);
}

/******************************************************************/

static void mE0i(cint i, EARGS(_Mr_)) {
  ReturnComplex(E0i(i-1, EARGS(_Id_)));
}

static void mE0ic(cint i, EARGS(_Mri_)) {
  ReturnComplex(E0iC(i-1, EARGS(_Mc_)));
}

/******************************************************************/

static void mEget(EARGS(_Mr_)) {
  ReturnList(5, Ecache(Eget(EARGS(_Id_))), Nee);
}

static void mEgetc(EARGS(_Mri_)) {
  ReturnList(5, EcacheC(EgetC(EARGS(_Mc_))), Nee);
}

/******************************************************************/

static void mLi2(XARGS(_Mr_)) {
  ReturnComplex(Li2(XARGS(_Id_)));
}

static void mLi2c(XARGS(_Mri_)) {
  ReturnComplex(Li2C(XARGS(_Mc_)));
}

static void mLi2omx(XARGS(_Mr_)) {
  ReturnComplex(Li2omx(XARGS(_Id_)));
}

static void mLi2omxc(XARGS(_Mri_)) {
  ReturnComplex(Li2omxC(XARGS(_Mc_)));
}

/******************************************************************/

static void mclearcache(void) {
  clearcache();
  ReturnVoid();
}

static void mmarkcache(void) {
  markcache();
  ReturnVoid();
}

static void mrestorecache(void) {
  restorecache();
  ReturnVoid();
}

/******************************************************************/

static void msetmudim(cRealType mudim) {
  SetReal(setmudim, mudim);
}

static void mgetmudim(void) {
  GetReal(getmudim);
}

/******************************************************************/

static void msetdelta(cRealType delta) {
  SetReal(setdelta, delta);
}

static void mgetdelta(void) {
  GetReal(getdelta);
}

/******************************************************************/

static void msetuvdiv(cRealType uvdiv) {
  SetReal(setuvdiv, uvdiv);
}

static void mgetuvdiv(void) {
  GetReal(getuvdiv);
}

/******************************************************************/

static void msetlambda(cRealType lambda) {
  SetReal(setlambda, lambda);
}

static void mgetlambda(void) {
  GetReal(getlambda);
}

static void mgetepsi(void) {
  GetInteger(getepsi);
}

/******************************************************************/

static void msetminmass(cRealType minmass) {
  SetReal(setminmass, minmass);
}

static void mgetminmass(void) {
  GetReal(getminmass);
}

/******************************************************************/

static void msetmaxdev(cRealType maxdev) {
  SetReal(setmaxdev, maxdev);
}

static void mgetmaxdev(void) {
  GetReal(getmaxdev);
}

/******************************************************************/

static void msetwarndigits(cint warndigits) {
  SetInteger(setwarndigits, warndigits);
}

static void mgetwarndigits(void) {
  GetInteger(getwarndigits);
}

/******************************************************************/

static void mseterrdigits(cint errdigits) {
  SetInteger(seterrdigits, errdigits);
}

static void mgeterrdigits(void) {
  GetInteger(geterrdigits);
}

/******************************************************************/

static void msetversionkey(cint versionkey) {
  SetInteger(setversionkey, versionkey);
}

static void mgetversionkey(void) {
  GetInteger(getversionkey);
}

/******************************************************************/

static void msetdebugkey(cint debugkey) {
  SetInteger(setdebugkey, debugkey);
}

static void mgetdebugkey(void) {
  GetInteger(getdebugkey);
}

/******************************************************************/

static void msetdebugrange(cint debugfrom, cint debugto) {
  setdebugrange(debugfrom, debugto);
  mlPutFunction(stdlink, "List", 2);
  mlPutInteger(stdlink, debugfrom);
  mlPutInteger(stdlink, debugto);
  mlEndPacket(stdlink);
}

/******************************************************************/

static void msetcmpbits(cint cmpbits) {
  SetInteger(setcmpbits, cmpbits);
}

static void mgetcmpbits(void) {
  GetInteger(getcmpbits);
}

/******************************************************************/

static void msetdiffeps(cRealType diffeps) {
  SetReal(setdiffeps, diffeps);
}

static void mgetdiffeps(void) {
  GetReal(getdiffeps);
}

/******************************************************************/

static void msetzeroeps(cRealType zeroeps) {
  SetReal(setzeroeps, zeroeps);
}

static void mgetzeroeps(void) {
  GetReal(getzeroeps);
}

/******************************************************************/

static void mltini(void) {
  CaptureStdout();
  ltini();
  mlPutStdout(stdlink);
  ReturnVoid();
}

static void mltexi(void) {
  CaptureStdout();
  ltexi();
  mlPutStdout(stdlink);
  ReturnVoid();
}

static void mltnop(void) {
  CaptureStdout();
  mlPutStdout(stdlink);
  ReturnVoid();
}

/******************************************************************/

static inline void openstdout() {
  int fd = open("/dev/null", O_WRONLY);
  dup2(fd, 1);
  close(fd);
}

static void __attribute__((constructor(4711))) make_sure_stdout_is_open() {
  if( fcntl(1, F_GETFD) == -1 ) openstdout();
}

/******************************************************************/

int main(int argc, char **argv) {
  int mlerr;
  pthread_t stdouttid;
  void *thr_ret;

#ifdef DEBUG
  deb = fopen("/tmp/ltdebug.out", "w");
  setbuf(deb, NULL);
#endif

  if( getenv("LTFORCESTDERR") == NULL ) {
    stdoutorig = dup(1);
    if( stdoutorig == -1 && getenv("LTRESPAWN") == NULL ) {
      openstdout();
      putenv("LTRESPAWN=1");
      execv(argv[0], argv);
      exit(1);
    }
    stdoutthr =
      socketpair(AF_LOCAL, SOCK_STREAM, 0, stdoutpipe) != -1 &&
      pthread_create(&stdouttid, NULL, capturestdout, stdoutpipe) == 0;
  }

  CaptureStdout();
  ltini();
  ReleaseStdout();

  DEB("begin MLMain\n");
  mlerr = MLMain(argc, argv);
  DEB("end MLMain\n");

  if( stdoutthr ) {
    close(stdoutpipe[1]);
    pthread_join(stdouttid, &thr_ret);
  }

  return mlerr;
}

