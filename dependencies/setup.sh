LT_VERSION=2.15

if [ $# -lt 1 ]
then
    echo "Error: This program needs one argument: the location where the libraries and
header files must be copied."
    exit 2
fi

path=$1
lib_path=$path/lib
inc_path=$path/include

if [ ! -e $lib_path ]
then
    echo "Error, library path \"$lib_path\" does not exist. Please be sure to \
give a valid path."
    exit 1
fi
if [ ! -e $inc_path ]
then
    echo "Error, include path \"$inc_path\" does not exist. Please be sure to \
give a valid path."
    exit 1
fi

echo -e "INSTALLDIR=$path\n\nCC  = $CC\nCXX = $CXX\nFC  = $FC\n\n" > Makefile
cat Makefile.in >> Makefile
