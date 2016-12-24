@echo off
echo Copying data files...
cd ..

mkdir Debug\shaders
mkdir Release\shaders
mkdir Debug\materials
mkdir Release\materials
mkdir Debug\models
mkdir Release\models
mkdir Debug\textures
mkdir Release\textures
mkdir Debug\docs
mkdir Release\docs

copy shaders Debug\shaders
copy shaders Release\shaders
copy materials Debug\materials
copy materials Release\materials
copy models Debug\models
copy models Release\models
copy textures Release\textures
copy docs Release\docs

copy assets.taif Debug
copy assets.taif Release

copy *.bmp Debug
copy *.bmp Release
copy *.png Debug
copy *.png Release

copy *.md Release

echo Data files copied
