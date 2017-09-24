# Tribalia

Tribalia is a real-time strategic game focused on Ancient Era, from since the
neolythic period (~ 10.000 AD) to the death of Christ (~ 30 BD).

<img src="icons/tribalia-logo.png" height="256px" width="256px" />

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


## Caveats

 - Your compiler needs to support C++17.

 - On Windows, you will might need to use the environment variable `TRIBALIA_FIND_PKG` to tell cmake where some dependencies can be found.
 
 - On FreeBSD, you need to install pkg-config, it finds some dependencies this way ( use `pkg install pkgconf`)

 - Only clang++ > 5 is supported (something to do with template deduction, supported in gcc7 and in libstdc++  but not in clang5).
