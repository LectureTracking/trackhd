#!/bin/bash

echo "Installing TRACK4K" 
jFlag="-j"
numCores=`cat /proc/cpuinfo | grep processor | wc -l`

echo Cores: $numCores

echo "STAGE 1/4: Removing previous builds..." 
rm -rf build
mkdir build
cd build

echo "STAGE 2/4: Generating make files..." 
cmake ../source/

echo "STAGE 3/4: Executing make operation..." 
make $jFlag$numCores

echo "STAGE 4/4: Installing..."
sudo make install

echo "Complete!"
