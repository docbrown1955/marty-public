***
<h2>Welcome to the documentation !</h2>
The documentation is an interactive content, explaining a little bit more in 
details objects and functions in *CSL*. *CSL* is the computer algebra system 
embedded in *MARTY*, although it is logically separated and has its own manual
and documentation.

This documentation is probably not complete. It is a work in progress, but it 
represents a massive amount of work and time constraints prevented me from 
writing a comprehensive documentation. There is still an important quantity 
of information you may get here, in particular on important topics about
*CSL*.

If you have any question that this documentation and the manual do not answer, 
please feel free to ask the author: <marty.uhlrich@gmail.com>.

If you want to come back on *MARTY* main page, you can 
[click here](https://marty.in2p3.fr) or click on the logo at the top of the page
at any time. To get to *MARTY*'s documentation, please 
[click here](https://marty.in2p3.fr/doc/marty/html/index.html).

***
<h2>Get Started with the documentation</h2>

If you are in the documentation for the first time, you should already have a
pretty precise idea of what *CSL* is doing without knowing really how. For a 
code overview, the manual or the initial publication are probably better. 

It is still very valuable to learn how to navigate in the documentation, and
in particular where to start. Then if you want to use a specific object in your
code, the documentation is really the best help you can get as even if not all 
function are documented, they all appear here.

<h3>Data structures</h3>
To have an idea of what information you can get here, you may be interested 
in the tab [Data Structures/Data Structure Index](classes.html). It contains all
the classes (object abstractions) in *CSL*.  You may found 
there what is an Expr (symbolic expression), csl::Sum, csl::Integer, csl::Tensor
and more.

<h3>Files</h3>
Another good starting point is to go in [File/File List](files.html) that shows 
all the header files in the physics part of *CSL*. File names are clear and 
the user should be able to guess what lies inside each (at least a bit). For 
example, csl::Cos, csl::Exp, csl::Sin are mathematical (symbolic) functions and
are in the file mathFunctions.h. 

***
<h2>Useful links</h2>
This part aims to give directly the links to the main features of *CSL*, 
sorted by topic. 

<h3>User interface</h3>
Most *CSL*'s features are callable from class methods. The files 
interface.h and interface_indicial.h contain a number of functions directly 
callable, without having to know what object owns the method. All features are
not reimplemented in this file, but the main ones are, and it is then a good 
starting point to know what you can do with *CSL*.

<h3>Basic expression types</h3>
A mathematical expression in *CSL* is expressed as a pointer to an abstract base
class. The pointer may point to any specialization (a sum, an integer, a 
product...). The ownership of an expression is shared, i.e. several expressions
may own another one (a variable 'x' may appear in several functions for 
example). The pointer is then a std::shared_ptr that expresses the fact that the
underlying ressource is shared. When the last shared_ptr pointing to a resource
is deleted, this ressource is automatically destroyed.

In *CSL*, the abstract base class is csl::Abstract and the shared pointer to it
is encapsulated in the class Expr. Both classes are in the file abstract.h. In
the csl::Abstract class, you may see all functions callable for a variable of 
type Expr using expr->func(<args>). In the Expr type is implemented a bit of 
interface allowing to use operator[] for example on an expression.

A list of all types in *CSL* is in the file enum.h.

<h3>Scalar expression types</h3>
As we said, a mathematical expression (Expr) is a shared pointer to the base 
class csl::Abstract, but the underlying object is specialized in a valid 
object. These objects can be of different types (here are just the scalar 
types):  
- numerical.h contains numerical types.
- literal.h contains literal types, i.e. csl::Constant, csl::Variable, and 
csl::Imaginary (the imaginary i in expressions respecting i*i = -1).
- mathFunctions.h contains mathematical function types (csl::Exp, csl::Cos, 
csl::Log etc).
- operations.h contains mathematical operations: +, *, -, / and ^. Those
operations are contained in the classes csl::Sum, csl::Prod, and csl::Pow. There
is no such thing as a - b or a / b directly, as these objects may be represented
by sums, products and exponents only. No need for other objects. In the cases
presented here, there is just a + (-1)*b and a * b^(-1).
 
<h3>Tensorial expression types</h3>
Expressions may be tensorial as well, i.e. carrying indices. 
- index.h contains the csl::Index and csl::IndexStructure classes. The first is
simply the internal representation of an index, and the second of an index list, 
that is the set of indices for a given tensor.
- csl::TensorElement is the specialization of csl::Abstract for a generic 
indexed tensor.
- csl::TensorParent is the parent object, unique in the program for each tensor,
that contains all the properties of the tensor. Each element keeps a pointer to
its parent to access its properties and do not carry them itself. This allows to
have only one copy of a tensor's properties in the program rather than for all
tensor instances in mathematical expressions.
- csl::Tensor is a shared pointer to csl::TensorParent. It follows the same 
principle as the Expr class for csl::Abstract, and it is the object the user
will manipulate as tensor parent.
- csl::ISum is a specialization of csl::Sum to handle indicial expressions. It
checks in particular if the different terms have the same free index structure.
- csl::IProd is a specialization of csl::Prod to handle indicial expressions. It
takes care of contracting all repeted indices whenever it is necessary to keep 
expressions well-formed.

<h3>Expression modifiers</h3>
Expressions may of course be modified by *CSL*. You may call at any moment 
member functions of the obecjt you manipulate, but also interface functions 
doing specific actions on expressions:
- utils.h contains functions to copy, refresh expressions, as well as functions
replacing parts of expressions, indices, in a bigger one.
- interface.h contains functions to factor, expand expressions, and more.
- algo.h contains algorithms that walk through mathematical expressions, 
applying a user-defined function on every sub-expression. Algorithms are very
practical when *CSL* built-in functions are not enough to do what the user wants 
to do.

<h3>Options</h3>
Several general options can be found in the file options.h.
