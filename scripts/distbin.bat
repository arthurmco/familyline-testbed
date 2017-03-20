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
copy terrain_test.trtb Release\terrain_test.trtb

copy assets.taif Debug
copy assets.taif Release

copy *.md Release

robocopy /s Release/ distribute/ * /S

echo Data files copied
