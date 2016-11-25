@echo off
echo Copying shaders...
cd ..
mkdir Debug\shaders
mkdir Release\shaders
copy shaders Debug\shaders
copy shaders Release\shaders
echo Shaders copied