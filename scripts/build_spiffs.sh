#!/bin/bash

# Create a temporary folder 
mkdir -p buildfs

# copy the local project data to this folder
cp -r data/* buildfs/

# copy the IotCore library to this folder
cp -r lib/IotCore/data/* buildfs/

# create the spiffs image
../tools/mkspiffs -c buildfs -b 4096 -p 256 -s 0x100000 spiffs.bin

# cleanup the temporary folder
rm -rf buildfs
