# Familyline

<img src="icons/tribalia-logo.png" height="128px" width="128px" />

[![pipeline status](https://gitlab.com/arthurmco/Familyline/badges/master/pipeline.svg)](https://gitlab.com/arthurmco/Familyline/commits/master)
[![coverage report](https://gitlab.com/arthurmco/Familyline/badges/master/coverage.svg)](https://gitlab.com/arthurmco/Familyline/commits/master)

Familyline is a real-time strategy game focused on Ancient Era, from since the
neolythic period (~ 10.000 AD) to the death of Christ (~ 30 BD).

In it, you will guide a family, that goes from the end of neolythic until its kingdom 
inclusion to the Roman Empire. You will trade with other people, war, mount strategies
and (if the money begin to appear) do a *lot* of quests.

It's made with a homebrew engine. The major objective of this game is
fun, to play and develop. Expect it to have a lot of humor!

## Supported OSs

 - Linux: full  
   Surprisingly, it's the most supported operating system. (For a
   game, this is quite a bit new)
 
 - Windows: full  
   You will have to download the libraries (the `.lib` files) and the headers separately.
 
 - FreeBSD: The last time I tried (+/- half a year), it complained about template things 
   (it was the FreeBSD 11.0-CURRENT version). I need to check support. It would be 
   interesting to support FreeBSD, a lot of games only care about Linux :P
   
 - OS X: Might work, but I don't have enough $$$ to have an Apple

## Needed Software

* *GLM:* for vector mathematics
* *GLEW:* for dealing with OpenGL extensions
* *SDL:* for dealing with our window. Version 2.0 is required.
* *devIL:* for loading our textures.
* *cairo:* for drawing the interfaces
* *libyaml:* For parsing the `assets.yaml` file, aka the asset list.

If you are building under Windows, download the packages in their respective sites.

If you are building on Linux, type the following command. This will
install the dependencies.

`# apt-get install libglm-dev libglew1.10 libglew-dev libsdl2-2.0.0 libsdl2-dev libdevil1c2 libdevil-dev
libcairo2 libcairo2-dev libyaml libyaml-dev`

## Documentation

See the docs/ folder. It talks about some things (planned or not), some 
manuals to the potential contributors, and some guides I wrote for myself.

Or you can read the code. They say the code is the most right documentation,
because it's what the program itself uses...

## Caveats

 - Your compiler needs to support C++17, because of that namespace thing

 - On Windows, you will might need to use the environment variable
   `TRIBALIA_FIND_PKG` to tell cmake where some dependencies can be
   found.  
   If you build with VS 2017 and its integrated cmake builder, you
   need to change the `CMakeSettings.json` to where you downloaded the
   libraries and includes.
 
 - On FreeBSD, you need to install pkg-config, it finds some dependencies this way ( use `pkg install pkgconf`)

 - Only clang++ > 5 is supported (something to do with template
   deduction, supported in gcc7 and in libstdc++  but not in clang5). 
   (I might be wrong, however. I only tried once)

## Packaging

To generate a package, simply run these two commands:

```
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_RELATIVE_PATH=off -DDO_TESTS=off .
make package
```

The first will generate a makefile that don't compile the tests and don't use
relative path builds (they are useful while debugging). The last one installs
the package. 

By default, it generates only a DEB file. To generate a RPM one, you need `rpmbuild`, and
to add a `-DCPACK_GENERATOR=RPM` before the dot, in the first command

## Rewrite in Rust!

Now it's opensource. This means **YOU CAN REWRITE, DAWG**

## License

Every file in this game is licensed under the MIT license, the best in
existence.

Copyright (C) 2016-2018 Arthur Mendes.


