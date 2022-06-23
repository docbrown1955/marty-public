if [ $# -lt 4 ]
then
    echo "This programs needs 4 arguments:"
    echo " 1. Library name"
    echo " 2. First quark Q1"
    echo " 3. Second quark Q2"
    echo " 4. Boson filter (None, SMExtension, NoBoson)"
    exit 1
fi

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[1;34m'
NC='\033[0m' # No Color
BOLD='\033[1m'
NORMAL='\033[0m'


LIBRARYNAME=$1
shift
Q1=$1
shift
Q2=$1
shift
BOSONFILTER=$1

intermediateresfile="coefficients.txt"
resfile="table.txt"

message()
{
    echo -e "${BLUE}${BOLD}$1${NC}${NORMAL}"
}

success()
{
    echo -e "${GREEN}${BOLD}$1${NC}${NORMAL}"
}

launch()
{
    message "Launching $1..."
    bash generic/setupmain.sh $LIBRARYNAME $Q1 $Q2 $2 $3 $4 $5 $6 $7 $8 $BOSONFILTER &&
    bash generic/launch.sh    $LIBRARYNAME &&
    echo -e "`tail -1 $intermediateresfile`" >> $resfile &&
    success "Calculation of $1 ended successfully!"
}

launch "MODEL 5"  1 0 0 0 0 0 0 &&
launch "MODEL 6"  0 1 0 0 0 0 0 &&
launch "MODEL 7"  0 0 1 0 0 0 0 &&
launch "MODEL 8"  0 0 0 1 0 0 0 &&
launch "MODEL 9"  0 0 0 0 1 0 0 &&
launch "MODEL 10" 0 0 0 0 0 1 0 &&
launch "MODEL 11" 0 0 0 0 0 0 1 &&

launch "MODEL 5+6"  1 1 0 0 0 0 0 &&
launch "MODEL 5+7"  1 0 1 0 0 0 0 &&
launch "MODEL 5+8"  1 0 0 1 0 0 0 &&
launch "MODEL 5+9"  1 0 0 0 1 0 0 &&
launch "MODEL 5+10" 1 0 0 0 0 1 0 &&
launch "MODEL 5+11" 1 0 0 0 0 0 1 &&

launch "MODEL 6+7"  0 1 1 0 0 0 0 &&
launch "MODEL 6+8"  0 1 0 1 0 0 0 &&
launch "MODEL 6+9"  0 1 0 0 1 0 0 &&
launch "MODEL 6+10" 0 1 0 0 0 1 0 &&
launch "MODEL 6+11" 0 1 0 0 0 0 1 &&

launch "MODEL 7+8"  0 0 1 1 0 0 0 &&
launch "MODEL 7+9"  0 0 1 0 1 0 0 &&
launch "MODEL 7+10" 0 0 1 0 0 1 0 &&
launch "MODEL 7+11" 0 0 1 0 0 0 1 &&

launch "MODEL 8+9"  0 0 0 1 1 0 0 &&
launch "MODEL 8+10" 0 0 0 1 0 1 0 &&
launch "MODEL 8+11" 0 0 0 1 0 0 1 &&

launch "MODEL 9+10" 0 0 0 0 1 1 0 &&
launch "MODEL 9+11" 0 0 0 0 1 0 1 &&

launch "MODEL 10+11" 0 0 0 0 0 1 1 &&

message "End of table calculation"
