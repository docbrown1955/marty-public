#!/bin/bash

RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'
BLUE='\033[1;34m'
BOLD='\033[1m'
NORMAL='\033[0m'

source .setup_script.sh $1

if [ "$?" == "0" ]
then
    echo "Environment variables set"
    source marty_env.sh
    echo -e "${BOLD}You may now run ${BLUE}$ make${NC}${NORMAL}"
fi
