#!/bin/bash

RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'
BLUE='\033[1;34m'
NC='\033[0m' # No Color
BOLD='\033[1m'
NORMAL='\033[0m'

# gcc-7 g++-7 gfortran-7
# problems can be encountered with loopTools with higher versions of gfortran
# like gfortran-9...
defaultCompilerVersion=7

if [ "$MARTY_COMPILER_VERSION" == "" ]
then
    v=-$defaultCompilerVersion
else
    if [ "$MARTY_COMPILER_VERSION" != "0" ]
    then
        v=-$MARTY_COMPILER_VERSION
    fi
fi

EXIT_CMD=
exitInstaller()
{
    echo -e "${RED}${BOLD}Error: Installation aborted.${NC}${NORMAL}"
    if [[ $0 != ${BASH_SOURCE} ]]
    then
        EXIT_CMD=return
    else
        EXIT_CMD=exit
    fi
}

message()
{
    echo -e $1
    sleep 0.2
}

missingdep()
{
    echo -e "${RED}${BOLD}Missing dependency: ${NORMAL}${NC}"
}

getVersionOf()
{
    echo "$($1 --version | perl -pe '($_)=/([0-9]+([.][0-9]+)+)/')"
}

testDependencies_Linux()
{
    echo "Testing Linux dependencies ..."
    dep_ok=1
    cutils=`dpkg -l | grep coreutils`
    if [ "$cutils" == "" ]
    then
            missingdep
	    echo "linux coreutils is needed for this installation script"
	    echo "Consider installing coreutils using (Ubuntu / Debian)"
	    echo -e " ${BLUE}${BOLD}$ sudo apt-get install coreutils${NC}${NORMAL}"
            dep_ok=0
    fi
    message "Testing dependencies"
    GCC=`command -v gcc$v`
    GPP=`command -v g++$v`
    GFORTRAN=`command -v gfortran$v`
    if [ "$GCC" == "" ] || [ "$GPP" == "" ] || [ "$GFORTRAN" == "" ]
    then
            missingdep
	    echo "gcc$v, g++$v and gfortran$v are needed to compile MARTY and its dependencies"
	    echo "Consider installing default compilers using (Ubuntu / Debian)"
	    echo -e " ${BLUE}${BOLD}$ sudo apt-get install gcc$v g++$v gfortran$v -y${NC}${NORMAL}"
            dep_ok=0
    fi
    gsl=`dpkg -l | grep libgsl`
    gslcblas=`dpkg -l | grep libgslcblas`
    gslcblasexists=`apt-cache search libgslcblas0`
    if [ "$gsl" == "" ]
    then
            missingdep
	    echo "GSL (GNU Scientific Library for C/C++) should be installed on your computer"
	    echo "Consider installing libgsl using (Ubuntu / Debian)"
	    echo -e " ${BLUE}${BOLD}$ sudo apt-get install libgsl-dev -y${NC}${NORMAL}"
            dep_ok=0
    fi
    if [ "$gslcblas" == "" ] && [ "$gslcblasexists" != "" ]
    then
            missingdep
	    echo "GSL (GNU Scientific Library for C/C++) should be installed on your computer"
	    echo "Consider installing libgslcblas using (Ubuntu / Debian)"
	    echo -e " ${BLUE}${BOLD}$ sudo apt-get install libgslcblas0 - y${NC}${NORMAL}"
            dep_ok=0
    fi

    libqt=`dpkg -l | grep libqt5`
    qtcreat=`command -v qtcreator`
    if [ "$libqt" == "" ] || [ "$qtcreat" == "" ]
    then
            missingdep
	    echo "Qt5 (Free software for Desktop application development) should be installed on your computer for GRAFED to work"
	    echo "Consider installing qt5-default and qtcreator using (Ubuntu / Debian)"
	    echo -e " ${BLUE}${BOLD}$ sudo apt-get install qt5-default qtcreator -y${NC}${NORMAL}"
            dep_ok=0
    fi
    dvipng_com=`command -v dvipng`
    if [ "$dvipng_com" == "" ]
    then
            missingdep
	    echo "dvipng should be installed on your computer for GRAFED to work"
	    echo "Consider installing dvipng using (Ubuntu / Debian)"
	    echo -e " ${BLUE}${BOLD}$ sudo apt-get install dvipng -y${NC}${NORMAL}"
            dep_ok=0
    fi
    if [ "$dep_ok" == "0" ]
    then
        exitInstaller $@
    else
        message "${GREEN}${BOLD}[1 / 5] All dependencies are ready for compilation${NC}${NORMAL}"
    fi
}

testDependency_MACOS()
{
    echo "Searching for $1 ..."
    if [ $# -ge 2 ]
    then
        brew list --$2 $1 &>/dev/null || brew install --$2 $1
    else
        brew list $1 &>/dev/null || brew install $1
    fi
    brew cleanup
}

testDependencies_MACOS()
{
    echo "Testing MACOS dependencies"
    vmac=${v:1:${#v}-1}
    testDependency_MACOS coreutils
    testDependency_MACOS gcc@$vmac
    testDependency_MACOS gsl
    testDependency_MACOS qt5
    testDependency_MACOS qt-creator cask
    message "${GREEN}${BOLD}[1 / 5] All dependencies are ready for compilation${NC}${NORMAL}"
}

testDependencies()
{
    unameOut="$(uname -s)"
    echo -e "${GREEN}${BOLD}Only Linux and MacOS are supported for now${NC}${NORMAL}"
    case "${unameOut}" in
        Linux*)     testDependencies_Linux;;
        Darwin*)    testDependencies_MACOS;;
        CYGWIN*)    exitInstaller $@;;
        MINGW*)     exitInstaller $@;;
        *)          exitInstaller $@
    esac
}

ensure_path()
{
    if [ ! -e $1 ]
    then
        mkdir -p $1
        # echo "Directory $1 does not exist and will be created. Confirm ? [y/n, default: y]"
        # read c
        # if [ "$c" == "" ] || [ "$c" == "Y" ] || [ "$c" == "y" ] || [ "$c" == "yes" ] || [ "$c" == "Yes" ]
        # then
        #     mkdir -p $1
        #     if [ ! -e $1 ]
        #     then
        #         echo -e "${RED}${BOLD}Path $1 is invalid, please provide a valid path.${NORMAL}${NC}"
        #         exitInstaller $@
        #         return
        #     fi
        # else
        #     echo -e "${RED}${BOLD}Path $1 is invalid, please provide a valid path.${NORMAL}${NC}"
        #     exitInstaller $@
        #     return
        # fi
    fi
}

ask_compiler_replacement()
{
    echo "Compiler $cc already defined. Replace it ? [y/n, default: y]"
    read c
    if [ "$c" == "" ] || [ "$c" == "Y" ] || [ "$c" == "y" ] || [ "$c" == "yes" ] || [ "$c" == "Yes" ]
    then
        fres=1
    else
        fres=0
    fi
}

message "${GREEN}${BOLD}MARTY installation script${NC}${NORMAL}"

testDependencies
[ ! -z $EXIT_CMD ] && $EXIT_CMD 1

default_path=/usr/local
if [ $# -ge 1 ]
then
    path=$(realpath -m $1)
else
    echo "Please enter the path where libraries must be installed (CSL, MARTY, GSL if local installation, and LoopTools)"
    read -p "(default if empty : $default_path): " tmppath
    if [ "$tmppath" != "" ]
    then
        path=$(realpath -m $tmppath)
    fi
fi

if [ "$path" = "" ]
then
    path=$(realpath -m $default_path)
fi

lib_path=$(realpath -m $path/lib)
inc_path=$(realpath -m $path/include)
bin_path=$(realpath -m $path/bin)

ensure_path $path
[ ! -z $EXIT_CMD ] && $EXIT_CMD 1
ensure_path $inc_path
[ ! -z $EXIT_CMD ] && $EXIT_CMD 1
ensure_path $lib_path
[ ! -z $EXIT_CMD ] && $EXIT_CMD 1
ensure_path $bin_path
[ ! -z $EXIT_CMD ] && $EXIT_CMD 1
message "${GREEN}${BOLD}[2 / 5] Installation path checked${NORMAL}${NC}"

martyEnvFile=marty_env.sh
echo "#!/bin/bash" > $martyEnvFile
replace_cc=1
replace_cxx=1
replace_fc=1
if [ "$CC" != "" ] && [ "$CC" != "gcc$v" ]
then
    cc="CC=$CC"
    ask_compiler_replacement
    replace_cc=$((fres&&replace_cc))
fi
if [ "$CXX" != "" ] && [ "$CXX" != "g++$v" ]
then
    cc="CXX=$CXX"
    ask_compiler_replacement
    replace_cxx=$((fres&&replace_cxx))
fi
if [ "$FC" != "" ] && [ "$FC" != "gfortran$v" ]
then
    cc="FC=$FC"
    ask_compiler_replacement
    replace_fc=$((fres&&replace_fc))
fi
if [ $replace_cc == 1 ]
then
    export CC=gcc$v
    echo "export CC=gcc$v" >> $martyEnvFile
fi
if [ $replace_cxx == 1 ]
then
    export CXX=g++$v
    echo "export CXX=g++$v" >> $martyEnvFile
fi
if [ $replace_fc == 1 ]
then
    export FC=gfortran$v
    echo "export FC=gfortran$v" >> $martyEnvFile
fi

MARTY_LOC="$(pwd)"
Makefile_setup="INSTALLDIR=$path\n\nCC  = $CC\nCXX = $CXX\nFC  = $FC"
cd dependencies/&& source setup.sh $path
cd $MARTY_LOC
cd marty&&        echo -e "$Makefile_setup" > Makefile&& cat Makefile.in >> Makefile
cd $MARTY_LOC
cd marty/models&& echo -e "$Makefile_setup" > Makefile&& cat Makefile.in >> Makefile
cd $MARTY_LOC
cd csl&&          echo -e "$Makefile_setup" > Makefile&& cat Makefile.in >> Makefile
cd $MARTY_LOC
cd grafed&&       echo -e "$Makefile_setup" > Makefile&& cat Makefile.in >> Makefile
cd $MARTY_LOC
cd jsonparser&&   echo -e "$Makefile_setup" > Makefile&& cat Makefile.in >> Makefile
cd $MARTY_LOC

export PATH=$PATH:/usr/local/opt/qt5/bin
savePATH=$PATH
grafed_debug=grafed/build-grafed-gui-Desktop_Qt_5_12_1_GCC_64bit-Debug
grafed_release=grafed/build-grafed-gui-Desktop_Qt_5_12_1_GCC_64bit-Release
unameOut="$(uname -s)"
case "${unameOut}" in
    Darwin*)
        mkdir -p $grafed_debug
        mkdir -p $grafed_release
        cd $grafed_debug && qmake -makefile -o Makefile "CONFIG+=debug" \
            "QMAKE_CXX=$CXX" "QMAKE_LINK=$CXX" ../grafed-gui/grafed-gui.pro;\
            cd $MARTY_LOC
        cd $grafed_release && qmake -makefile -o Makefile  \
            "QMAKE_CXX=$CXX" "QMAKE_LINK=$CXX" ../grafed-gui/grafed-gui.pro;\
            cd $MARTY_LOC
        conf_release=`ls $grafed_release/.qmake*`
        conf_debug=`ls $grafed_debug/.qmake*`
        if [ "$conf_debug" == "" ] || [ "$conf_release" == "" ]
        then
            echo -e "${RED}${BOLD}GRAFED qmake configuration failed...${NORMAL}${NC}"
            exitInstaller $@
            [ ! -z $EXIT_CMD ] && $EXIT_CMD 1
        fi
    ;;
esac
export PATH=$savePATH
message "${GREEN}${BOLD}[3 / 5] Grafed qmake config ready ${NORMAL}${NC}"

echo "export PATH=\$PATH:$bin_path" >> $martyEnvFile
echo "export CPATH=\$CPATH:$inc_path" >> $martyEnvFile
echo "export C_INCLUDE_PATH=\$C_INCLUDE_PATH:$inc_path" >> $martyEnvFile
unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)
        echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$lib_path" >> $martyEnvFile;
        echo "export LIBRARY_PATH=\$LIBRARY_PATH:$lib_path" >> $martyEnvFile;;
    Darwin*)
        echo "export DYLD_LIBRARY_PATH=\$DYLD_LIBRARY_PATH:$lib_path" >> $martyEnvFile;
        echo "export LIBRARY_PATH=\$LIBRARY_PATH:$lib_path" >> $martyEnvFile;;
esac

message "${GREEN}${BOLD}[4 / 5] Paths created in file \"$martyEnvFile\"${NORMAL}${NC}"

message "${GREEN}${BOLD}[5 / 5] MARTY setup completed${NORMAL}${NC}"

