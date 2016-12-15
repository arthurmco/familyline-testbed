@echo off
echo Copying data files...
cd ..

mkdir Debug\shaders
mkdir Release\shaders
mkdir Debug\materials
mkdir Release\materials
mkdir Debug\models
mkdir Release\models

copy shaders Debug\shaders
copy shaders Release\shaders
copy materials Debug\materials
copy materials Release\materials
copy models Debug\models
copy models Release\models

copy test.taif Debug
copy test.taif Release

copy *.bmp Debug
copy *.bmp Release
copy *.png Debug
copy *.png Release

cp *.md Debug
cp *.md Release

echo Data files copied