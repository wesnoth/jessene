#!/bin/bash
ln -s lua/src luasrc
cd luasrc
make -j2 MYCFLAGS="-fPIC" linux || { echo "Failed to build lua!"; exit 1; }
cd ../..

git clone --recursive https://github.com/GodotNativeTools/godot-cpp.git godot-cpp || { echo "Failed to clone godot-cpp! Exiting..."; exit 1; }
cd godot-cpp
scons platform=linux generate_bindings=yes bits=64 -j 2 || { echo "Building godot-cpp failed!"; exit 1; }
cd ..

mkdir Godot-Lua-Library
mkdir Godot-Lua-Library/bin
mkdir Godot-Lua-Library/src
cd Godot-Lua-Library/src

ln -s ../../jessene-travis jessene
cd ..
ln -s ../godot-cpp godot-cpp
ln -s src/jessene/SConstruct SConstruct

scons platform=linux bits=64 || { echo "Building Haldric's lua module failed!"; exit 1; }
