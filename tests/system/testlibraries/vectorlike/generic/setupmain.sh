if [ $# -lt 11 ]
then

    while [ $# -gt 1 ]
    do
      echo $1
      shift
    done
    echo "This programs needs 11 arguments:"
    echo " 1. Library name"
    echo " 2. First quark Q1"
    echo " 3. Second quark Q2"
    echo " 4. Use model 5 (boolean)"
    echo " 5. Use model 6 (boolean)"
    echo " 6. Use model 7 (boolean)"
    echo " 7. Use model 8 (boolean)"
    echo " 8. Use model 9 (boolean)"
    echo " 9. Use model 10 (boolean)"
    echo " 10. Use model 11 (boolean)"
    echo " 11. Boson filter (None, SMExtension, NoBoson)"
    exit 1
fi

LIBRARYNAME=$1
shift
Q1=$1
shift
Q2=$1
shift
MODEL5=$1
shift
MODEL6=$1
shift
MODEL7=$1
shift
MODEL8=$1
shift
MODEL9=$1
shift
MODEL10=$1
shift
MODEL11=$1
shift
BOSONFILTER=$1

cp generic/main.cpp scripts/
sed -i "s/#LIBRARYNAME/$LIBRARYNAME/g" scripts/main.cpp
sed -i "s/#Q1/$Q1/g" scripts/main.cpp
sed -i "s/#Q2/$Q2/g" scripts/main.cpp
sed -i "s/#MODEL5/$MODEL5/g" scripts/main.cpp
sed -i "s/#MODEL6/$MODEL6/g" scripts/main.cpp
sed -i "s/#MODEL7/$MODEL7/g" scripts/main.cpp
sed -i "s/#MODEL8/$MODEL8/g" scripts/main.cpp
sed -i "s/#MODEL9/$MODEL9/g" scripts/main.cpp
sed -i "s/#MODEL10/$MODEL10/g" scripts/main.cpp
sed -i "s/#MODEL11/$MODEL11/g" scripts/main.cpp
sed -i "s/#BOSONFILTER/$BOSONFILTER/g" scripts/main.cpp
