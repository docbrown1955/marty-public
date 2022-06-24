# Test suite for MARTY >= 1.3

## Generalities

MARTY program launching calculations are stored in `scripts/` and can be compiled using 

    > make

When relevant, numerical scripts using generated libraries are stored in `libscripts/` and benchmark data files used for comparison in `data/`.

To launch all tests at once, type

    > ./launch_tests.x

This will link to a global MARTY library (make sure to have installed it in a standard location) and launch all tests separately.

## Amplitude tests

### Conjugation matrix

#### Conjugation matrix general behavior

 - **File**: testMajoranaLoop.x
 - **Purpose**: Test that the conjugation matrix behaves well in loops with Majorana and non-Majorana fermions, i.e. that it simplifies out naturally before trace evaluation.
 - **Test**: Program termination. A calculation at 1-loop with 4 scalar external legs is launched (containing fermion loops). If the calculation gets to the end it means that the conjugation matrix indeed has been simplified.
 - **Sensibility**: SGL behavior with the conjugation matrix, and possibly Feynman rules initial calculations. Fermionic external legs do not participate here.

#### RSIF (Relative Signs of Interfering Feynman diagrams)

 - **File**: testConjugation.x
 - **Purpose**: Testing relative signs in Feynman diagrams from Majorana and fermion number violating couplings.
 - **Source**: Section 3.3 of [Feynman rules for fermion-number-violating interactions](https://cds.cern.ch/record/238903/files/th-6549-92.pdf) (A. Denner, 1992).
 - **Test**: Value test. For a scalar boson and a vector boson, test the number resulting of the squared amplitude for the ff->fMb (fermion, Majorana, boson) from numbers obtained with verified amplitudes. A different relative sign will change interference and modify this number.
 - **Sensibility**: Conjugation matrix properties in SGL, PolarizationField, and possibly initial Feynman rules calculations.

## Squared amplitude tests

### Tree-level partial decay widths

 - **File**: testWidths.x
 - **Source**: Section 50. in PSG 2020 (can be found at least in PDG > 2016).
 - **Numerical program**: example_widths_lib.cpp
 - **Benchmark**: widths_with_hBB_1L.txt
 - **Test**: Value test. One point is compared to the PDG's expression.
 
#### h -> WW in the SM
 - **Sensibility**: Spin sums for vector bosons.

#### h -> ZZ in the SM
 - **Sensibility**: Spin sums for vector bosons and combinatorial factor for indistinguishable external particles.

#### h -> bb in the SM
 - **Sensibility**: Spin sums for fermions, simple fermion traces.

#### W -> lnu in the SM
 - **Sensibility**: Spin sums for fermions and vector bosons, simple fermion traces.

#### Z -> ss in the SM
 - **Sensibility**: Spin sums for fermions and vector bosons, simple fermion traces.
 
### One-loop partial decay widths

 - **File**: testWidths.x
 - **Source**: [Higgs to Gamma Gamma beyond the Standard Model](https://arxiv.org/abs/0901.0927v2) (Cacciapaglia, Deandrea 2009).
 - **Numerical program**: example_widths_lib.cpp
 - **Benchmark**: widths_with_hBB_1L.txt
 - **Test**: Value test. One point is compared to the paper's expression.
 
#### h -> AA (top quark loop) in the SM
 - **Sensibility**: Strong sensibility to local terms correctness.

#### h -> gg (top quark loop) in the SM
 - **Sensibility**: Strong sensibility to local terms correctness, SU(3) simple weights.

### 2 -> 2 processes

#### ee -> A,Z -> mu mu in the SM

 - **File**: testZmumu.x
 - **Source**: [Physics at LEP-1 and LEP-2](https://cds.cern.ch/record/850586/files/008cartagena1.pdf) (E.Fernandez, 2000). Qualitative comparisons and some 'by-hand' calculations have been used to verify an exact benchmark with MARTY that is then used for comparison.
 - **Numerical program**: example_zmumu.cpp

##### Full cross-section
 - **Benchmark**: ee_mumu_tot.txt
 - **Test**: Value test. The full integrated cross-section is compared to the benchmark in a range of input center of mass energy.
 - **Sensibility**: General fermion spin-sums and traces, photon / Z interference, Breit-Wigner propagator (non-zero width).

##### Chirality-specific contributions
 - **Benchmark**: ee_mumu_LR.txt
 - **Test**: Value test. The chirality-specific (left- or right-handed incoming electrons) cross-section is compared to the benchmark in a range of the polar angle theta.
 - **Sensibility**: Chiral traces, 4D-epsilon symbol contraction properties.
 
##### Forward-Backward asymmetry
 - **Benchmark**: ee_mumu_AFB.txt
 - **Test**: Value test. Test the value, for one value of theta, of the difference of forward and backward amplitudes for a given range of energies.
 - **Sensibility**: Same sensitivity as the full cross-section, but is sensible to more different terms (odd terms in cos(theta)).

#### gg -> tt in the SM

 - **File**: reinterpretation.x
 - **Numerical program**: example_gg_to_tt.cpp
 - **Benchmark**: reinterpretation.txt
 - **Test**: Value test. Total cross-section as a function of the center of mass energy.
 - **Sensibility**: SU(3) weights, f_ABC coupling, ghost squared amplitude, and most of all derivative interaction (FeynruleMomentum, momentum mapping treatment etc).
 
## Wilson coefficients tests

### 2-fermions operators
#### C7, top quark contributions only, in the SM Feynman gauge
 - **File**: C7_SM.x
 - **Source**: [SuperIso manual](http://superiso.in2p3.fr/superiso4.1.pdf) (Mahmoudi, Neshatpour 2020) appendix C.
 - **Numerical program**: example_c7_sm.x
 - **Benchmark**: C7_SM.txt 
 - **Test**: Value test. Coefficients values compared to benchmark in a range of the top quark mass.
 - **Sensibility**: Wilson coefficient calculation, Goldstone contributions, Dirac equation, 3-boson vertex WWA.
#### C8, top quark contributions only, in the SM Unitary gauge
 - **File**: C8_SM.x
 - **Source**: [SuperIso manual](http://superiso.in2p3.fr/superiso4.1.pdf) (Mahmoudi, Neshatpour 2020) appendix C.
 - **Numerical program**: example_c8_sm.x
 - **Benchmark**: C8_SM.txt
 - **Test**: Value test. Coefficients values compared to benchmark in a range of the top quark mass.
 - **Sensibility**: Wilson coefficient calculation, Unitary gauge (more momenta), Dirac equation.
#### C7, C8, stop-chargino contribution only, in the MSSM
 - **Files**: C7_PMSSM.x and C8_PMSSM.x
 - **Source**: [SuperIso manual](http://superiso.in2p3.fr/superiso4.1.pdf) (Mahmoudi, Neshatpour 2020) appendix C.
 - **Numerical programs**: example_c7_pmssm.x and example_c8_pmssm.x
 - **Benchmarks**: C7_marty_tanb50_ss.txt and C8_marty_tanb50_ss.txt
 - **Test**: Value test. Coefficients values compared to benchmark in a 2D-range of mu and M2.
 - **Sensibility**: PMSSM model, spectrum generator (stop, charginos), correct behavior of fermion number violating chargino interactions. 

### 4-fermions operators
#### Coming soon !

## Miscellaneous tests

### Tensor reduction for momentum integral
 - **File**: FItests.x
 - **Test**: Symbolic test. Some simple reductions are tested, compared to analytical results.
 - **Sensibility**: Not much, should probably extend those tests. Still sensible to simple tensor reduction algorithms.
