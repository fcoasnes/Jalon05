#!/usr/bin/env bash
#this file creates supports files for your project (makefile you can use with make and eclipse project file you can use with eclipse CDT http://www.eclipse.org/cdt/)
#those supports files were created and tested with cmake 2.8 and eclipse cdt luna
rm -rf build
mkdir build
cd build
cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug ../src/
make
cd ..
cp ./src/jalon05/help.txt ./build/jalon05
cp ./src/jalon05/logo.jpg ./build/jalon05
mkdir ./build/jalon05/Download_file

