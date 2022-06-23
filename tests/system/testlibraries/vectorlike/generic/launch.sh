if [ $# -lt 1 ]
then
    echo "This programs needs 1 arguments:"
    echo " 1. Library name"
    exit 1
fi

LIBRARYNAME=$1

make -j 4 main.x &>/dev/null &&
bin/main.x &>/dev/null &&
cd $LIBRARYNAME &&
make -j 4 &>/dev/null &&
bin/example_${LIBRARYNAME,,}.x &>/dev/null &&
tail -1 data.txt >> ../coefficients.txt &&
cd ..
