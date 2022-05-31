# MARTY

### Summary

`MARTY` is a C++ program automating perturbative calculations for beyond the Standard Model scenarios.
Models can be built using the user interface. For any model, squared amplitudes and Wilson coefficients
can be calculated at the tree level or the one-loop level.

`MARTY` is a code generator. Analytical expressions, squared amplitudes or Wilson coefficients are 
converted into C++ code in a self-contained library compiled independently of `MARTY`.
This code can therefore be used for numerical evaluation in different scenarios to perform a 
phenomenological analysis. 

More information can be found on the [website](https://marty.in2p3.fr).

### Installation

Installation of `MARTY`:

```
source setup.sh <marty-installation-path>
make
make install
```

If installing in standard location (`/usr/local`)
```
source ./setup.sh 
make
sudo make install
```

For compilation on multiple cores:
```
make -j N
```
with N number of CPUs. To compile a program using `MARTY`:
