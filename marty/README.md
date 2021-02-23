***
<h2>Welcome to the documentation !</h2>
The documentation is an interactive content, explaining a little bit more in 
details objects and functions in *MARTY*. If you are interested by the underlying
computer algebra system, *CSL*, please note that its documentation is separated 
from the physics part presented here. References to *CSL* objects are present
in *MARTY*'s documentation and links will automatically bring you to their 
documentation (in *CSL*) if needed.

This documentation is probably not complete. It is a work in progress, but it 
represents a massive amount of work and time constraints prevented me from 
writing a comprehensive documentation. There is still an important quantity 
of information you may get here, in particular on important topics about
*MARTY*.

If you have any question that this documentation and the manual do not answer, 
please feel free to ask the author: <g.uhlrich@ipnl.in2p3.fr>.

If you want to come back on *MARTY* main page, you can 
[click here](https://marty.in2p3.fr) or click on the logo at the top of the page
at any time.

***
<h2>Get Started with the documentation</h2>

If you are in the documentation for the first time, you should already have a
pretty precise idea of what *MARTY* is doing without knowing really how. For a 
code overview, the manual or the initial publication are probably better. 

It is still very valuable to learn how to navigate in the documentation, and
in particular where to start. Then if you want to use a specific object in your
code, the documentation is really the best help you can get as even if not all 
function are documented, they all appear here.

<h3>Data structures</h3>
To have an idea of what information you can get here, you may be interested 
in the tab [Data Structures/Data Structure Index](classes.html). It contains all
the classes (object abstractions) in the physics part of *MARTY*.  You may found 
there what is a mty::QuantumField, a high energy physics mty::Model, a 
mty::Lagrangian, a mty::Propagator and more.

<h3>Files</h3>
Another good starting point is to go in [File/File List](files.html) that shows 
all the header files in the physics part of *MARTY*. There exists a good 
correspondance between files and objects. You may found the mty::QuantumField in 
quantumField.h, the mty::Model in model.h, mty::Lagrangian in lagrangian.h etc.

***
<h2>Useful links</h2>
This part aims to give directly the links to the main features of *MARTY*, 
sorted by topic. 

<h3>User interface</h3>
Most *MARTY*'s features are callable from class methods. The file 
mrtInterface.h contains a number of functions directly callable, without having
to know what object owns the method. All features are not reimplemented in this
file, but the main ones are, and it is then a good starting point to know what 
you can do with *MARTY*.

<h3>CSL</h3>
*CSL* is the computer algebra system integrated in *MARTY*. You may 
[click here](https://marty.in2p3.fr/doc/csl/html/index.html) to go to its 
documentation.
<h3>Model Building</h3>
Model building is a very important topic for a user as it is almost the only
part he/she will have to do him/herself. The mty::Model class is actually built
in three layers, each one containing a different part of the final features:
- mty::ModelData is the first layer, containing all functions allowing to 
modify the content of the model. Adding a particle, a gauge group, getting the
content are features implemented in this class. The lagrangian also belongs to 
it.
- mty::ModelBuilder inherits from mty::ModelData and adds all model building 
utilities that modify the model. Symmetry breaking, replacements, renaming...
- mty::Model inherits from mty::ModelBuilder and adds the final computational
features. In this class are implemented the functions allowing to launch 
different calculations: Feynman rules, amplitudes, squared amplitudes...

<h3>Lagrangian</h3>
This part is less important as the Lagrangian is meant to be fully encapsulated
by the model classes. If one wants to know how it works inside, the main objects
to see are:
- mty::InteractionTerm is the representation of a term in the Lagrangian in the 
code. It is a little bit more than a mathematical expression, and
knows better about what particles are inside etc.
- mty::Lagrangian is the set of all interaction terms, separated in three parts.
Kinetic, mass, and interaction terms.

<h3>Quantum Field Theory</h3>
Quantum Field Theory is really what *MARTY* does. There is of course many
different files and objects related to it. Here are the main ones:
- mty::QuantumField is the object getting into mathematical expressions, 
representing a quantum field. In particular it inherits from
csl::TensorFieldElement. 
- mty::QuantumFieldParent is the parent object of mty::QuantumField (inheriting 
from csl::TensorFieldParent). It contains all the quantum data about the field.
Spin, mass, width, gauge / flavor representation...
- mty::ScalarBoson, mty::WeylFermion, mty::DiracFermion, mty::VectorBoson, 
mty::GaugeBoson, mty::GhostBoson are specializations of mty::QuantumFieldParent
that handle more specifically their properties. The Majorana fermion has not its
own class as it is simply a Dirac fermion that is its own conjugate.
- quantumFieldTheory.h is a file containing all analytical expressions for 
propagators, kinetic / mass terms for particles in Lagrangian, and external legs
insertions.
- graph.h is the file containing the algorithm applying the Wick theorem. The
algorithm is a bit complex because optimized to run as fast as possible, 
avoiding to find the same diagrams many times.
- expander.h is the file that expands the lagrangian to a given order to find 
all possible diagrams, applying the Wick theorem on every possible set of 
interactions. It is also a well optimized algorithm to avoid any unnecessary 
computation but may be interesting to look at.
- amplitude.h contains the mty::AmplitudeCalculator class that is the main 
simplification unit for *MARTY*. In particular, all the simplification 
procedures (Dirac algebra, group algebra, momentum integral reductions, 
Dirac equation, ...) are called from mty::AmplitudeCalculator::simplify().
- diracology.h contains the Dirac space and all its content. Gamma matrices, 
trace identities, are contained in this file.

<h3>Group Theory</h3>
Group theory is a more deep feature, as a standard user will only be interested
in creating gauge groups, feature that is encapsulated in the mty::ModelBuilder
class. However, if someone wants to get deeper, understand
how representations and group algebra are handled, here are the main links:
- semiSimpleAlgebra.h contains the algorithms that allow to store any 
reprensentation of a semi-simple algebra, and calculate the decomposition of a 
product of representations. Some group theory weights could also be taken from
this machinery. In particlar quadratic Casimirs come from this file.
- group.h contains a structural link between semi-simple algebras (A, B, C, D 
etc) and groups (SU, SO, Sp, etc).
- gaugedGroup.h contains all the machinery for gauged groups in *MARTY*. In 
particular, a mty::GaugedGroup handles its own coupling constant (mathematical
expression Expr), gauge bosons (mty::GaugeBoson), ghosts (mty::GhostBoson), 
and generators (mty::Generator). All generator properties (index contraction)
are implemented in this file.
- colorSpace.h contains the machinery to calculate traces of group generators,
using group indices and reference tensors.
- groupIndices.h contains all group indices, used in the calculation of traces.
- traceIdentities.h contains the trace identities for all groups.
