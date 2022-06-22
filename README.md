# MARTY

## Summary

`MARTY` is a C++ program automating perturbative calculations for beyond the Standard Model scenarios.
Models can be built using the user interface. For any model, squared amplitudes and Wilson coefficients
can be calculated at the tree level or the one-loop level.

`MARTY` is a code generator. Analytical expressions, squared amplitudes or Wilson coefficients are
converted into C++ code in a self-contained library compiled independently of `MARTY`.
This code can therefore be used for numerical evaluation in different scenarios to perform a
phenomenological analysis.

More information can be found on the [website](https://marty.in2p3.fr).

## Get Started

In the following some possibilities to get started easily with `MARTY`, once installed, are presented. For installation instructions, see the following section.

## Installation

The following procedure is relevant since `MARTY-2.0`. To build and install older versions please refer to the [website](https://marty.in2p3.fr/download.html).

### Dependencies

Since `MARTY-2.0` the dependency installations are longer supported by the automated build procedure, it is the user responsibility to install the required dependencies on his/her particular system.

Library dependencies (needed at compile-time):
 - `Qt5` (needed for the `GRAFED` Graphical User Interface)
 - `GSL` + `GSLCBLAS` (C numerical scientific library)

System dependencies (needed at run-time as system commands):
 - `cmake`
 - `latex`
 - `lualatex`
 - `dvipng`
 - `convert` (part of `ImageMagick`)

 On `Ubuntu` for example these dependencies can be installed with
 ``` bash
    sudo apt-get install libgsl-dev libgslcblas0 texlive texlive-luatex dvipng coreutils -y
    sudo apt-get install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools -y
    sudo apt-get install cmake -y
 ```
 On `MacOS`
 ``` bash
    brew install coreutils gsl qt5 cmake imagemagick texlive
 ```

### Compilers for MARTY

MARTY links C++ code with Fortran code from the [LoopTools](http://www.feynarts.de/looptools/) library.
MARTY takes care of compiling and linking `LoopTools` but the compilers must be chosen carefully to link C++ and fortran codes.

For this reason, the MARTY installation procedure requires to have the GNU compiler collection, version greater than 7. Furthermore, the versions of `gcc/g++/gfortran` should be the same. The minimal set of compilers required to compile `MARTY` is therefore
```
    gcc-7
    g++-7
    gfortran-7
```

If the default compilers on the system do not meet the above requirements, compilers must be explicitely defined before going further following:
``` bash
    export CXX=g++-7
    export CC=gcc-7
    export FC=gfortran-7
```
Any version greater than 7 can be used but all the three compilers must use **the same version**.

### Build and install `MARTY`


Once the dependencies have been installed and that the `C`, `C++` and `Fortran` compilers meet the requirements above (either by default or after having exported the `CC`, `CXX` and `FC` environment variables), clone this repository:
```bash
    git clone https://github.com/docbrown1955/marty-public.git
```
Then, build and install `MARTY`!
``` bash
    cd marty-public
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=<installation-path-for-marty>
    make
    make install
```

If the `CMAKE_INSTALL_PREFIX` is not given, `cmake` will automatically set this variable to `/usr/local`. In case the installation path is not in `/home` (on Unix systems), the installation command requires `sudo`:
``` bash
    sudo make install
```
## Environment variables

In case `MARTY` is not installed in a standard location (not in `/usr/local/`) it is necessary to set properly environment variables. Given that the installation path is `<install/path>`, Linux users should add this in their `${HOME}/.bashrc` file:

``` bash
    export PATH=$PATH:<install/path>/bin
    export CPATH=$CPATH:<install/path>/include
    export C_INCLUDE_PATH=$C_INCLUDE_PATH:<install/path>/include
    export LIBRARY_PATH=$LIBRARY_PATH:<install/path>/lib
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<install/path>/lib
```

Although not recommended, if MacOS users install MARTY in a non-standard location the appropriate environment variables are (also to put in `${HOME}/.bashrc`)

``` bash
    export PATH=$PATH:<install/path>/bin
    export CPATH=$CPATH:<install/path>/include
    export C_INCLUDE_PATH=$C_INCLUDE_PATH:<install/path>/include
    export LIBRARY_PATH=$LIBRARY_PATH:<install/path>/lib
    export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:<install/path>/lib
```

After the first installation (and adding the environment variables in the `.bashrc` file) it is necessary to launch a new terminal or reload the bash session:

``` bash
   source ${HOME}/.bashrc
```

## The specific case of `MacOS`

On `MacOS` the System Integrity Protection (SIP) can cause issues if files are not installed in `/usr/local`. We therefore recommand to install `MARTY` in this location (default):
``` bash
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
```

## In case of trouble installing `MARTY`

In you have issues to install dependencies or build `MARTY`, please see the open and closed [issues](https://github.com/docbrown1955/marty-public/issues). If not one solves your problem, open a new issue describing the bug:
 - Which operating system (including the version)
 - Provide a clear description of the procedure followed before the bug happened
 - Copy the error message if any
