Installation of MARTY:

source setup.sh <marty-installation-path>
make
make install

If installing in standard location (/usr/local)

source ./setup.sh 
make
sudo make install

Parallel compilation enable:
make -j N
with N number of CPUs

To compile a program using marty:

export LD_LIBRARY_PATH=<marty-installation-path>/lib
g++ -std=c++17 main.cpp -o main.x \
    -I<marty-installation-path>/include \
    -L<marty-installation-path>/lib \
    -lmarty

Paths are not required if MARTY is installed in a standard location, or if proper environment variables are set in the .bashrc file.

And finally to run it:
./main.x

More information on https://marty.in2p3.fr !

