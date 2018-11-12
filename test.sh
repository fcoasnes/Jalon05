#!/bin/sh


DIR=./build/jalon05
GEO=90x30
WAIT=1
cd $DIR;
make;

./RE216_JALON05_SERVER 8080 &
lxterminal --working-directory=$DIR --geometry=$GEO -e "./RE216_JALON04_CLIENT localhost 8080" &
lxterminal --working-directory=$DIR --geometry=$GEO -e "./RE216_JALON04_CLIENT localhost 8080" &
lxterminal --working-directory=$DIR --geometry=$GEO -e "./RE216_JALON04_CLIENT localhost 8080";

