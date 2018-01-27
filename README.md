# Tribalia

Tribalia is a real-time strategy game focused on Ancient Era, from since the
neolythic period (~ 10.000 AD) to the death of Christ (~ 30 BD).

In it, you will guide a family, that goes from the end of neolythic until its kingdom 
inclusion to the Roman Empire. You will trade with other people, war, mount strategies
and (if the money begin to appear) do a *lot* of quests.

<img src="icons/tribalia-logo.png" height="256px" width="256px" />

## Supported OSs

 - Linux: full
 
 - Windows: The server doesn't compile, but the game do.
 
 - FreeBSD: The last time I tried (+/- half a year), it complained about template things 
   (it was the FreeBSD 11.0-CURRENT version). I need to check support. It would be 
   interesting to support FreeBSD, a lot of games only care about Linux :P
   
 - OS X: Might work, but I need some $$$ to test it :/

## Needed Software

* *GLM:* for vector mathematics
* *GLEW:* for dealing with OpenGL extensions
* *SDL:* for dealing with our window. Version 2.0 is required.
* *devIL:* for loading our textures.
* *cairo:* for drawing the interfaces

If you are building under Windows, download the packages in their respective sites.

If you are building on Linux, type the following command:

`# apt-get install libglm-dev libglew1.10 libglew-dev libsdl2-2.0.0 libsdl2-dev libdevil1c2 libdevil-dev
libcairo2 libcairo2-dev`

## Documentation

See the docs/ folder. It talks about some things (planned or not), some 
manuals to the potential contributors, and some guides I wrote for myself.

Or you can read the code. They say the code is the most right documentation,
because it's what the program itself uses...

## Caveats

 - Your compiler needs to support C++17.

 - On Windows, you will might need to use the environment variable `TRIBALIA_FIND_PKG` to tell cmake where some dependencies can be found.
 
 - On FreeBSD, you need to install pkg-config, it finds some dependencies this way ( use `pkg install pkgconf`)

 - Only clang++ > 5 is supported (something to do with template deduction, supported in gcc7 and in libstdc++  but not in clang5).


## Use Boost!

I hate that library. It's weird, unnecessary and heavyweight. Tribalia will never use Boost.

## Rewrite in Rust!

No.
