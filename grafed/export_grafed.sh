#!/bin/bash

directory=$1
target=$2

cp $(find $directory -name "grafed-gui") $target
