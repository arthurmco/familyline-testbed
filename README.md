# Familyline

<span style="text-align: center">
	<img src="icons/familyiline-logo.png" height="128px" width="128px" />
</span>

Familyline is a real-time strategy game focused on Ancient Era, from since the
neolythic period (~ 10.000 AD) to the death of Christ (~ 30 BD).

In it, you will guide a family, that goes from the end of neolythic until its kingdom 
inclusion to the Roman Empire. You will trade with other people, war, mount strategies
and (if the money begin to appear) do a *lot* of quests.

It's made with a homebrew engine. The major objective of this game is
fun, to play and develop. Expect it to have a lot of good humor!

## Build and Installation

The game runs in Linux and Windows. Other platforms need to be tested.

On Linux, you will need the softwares below:

 - *GLM:* for vector mathematics
 - *GLEW:* for dealing with OpenGL extensions
 - *SDL:* for dealing with our window. Version 2.0 is required.
 - *devIL:* for loading our textures.
 - *cairo:* for drawing the interfaces
 - *libyaml:* For parsing the `assets.yaml` file, aka the asset list.

On Windows, you'll have to install the .lib files and headers of the
libraries mentioned above (until I make it work with vcpkg). The dlls
are in the distribute/ folder.

I created a zip and put them
[here](https://drive.google.com/open?id=1gDIS9CkskJ25cwmuw7jCXTgVxZ08VCdA). (SHA1
is 695fee4fa4f59e6831509d00b7684bf2dc20b2f4 ).

To build it, you'll need a C++17 compiler. I build using Visual C++
2017 and GCC 8, so if you have anything better, you will be able to
build too.

If you build with VS 2017 and its integrated cmake builder, you
need to change the `CMakeSettings.json` to where you downloaded the
libraries and includes.
 
## Packaging

To generate a package, simply run these two commands:

```
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_RELATIVE_PATH=off -DDO_TESTS=off .
make package
```

The first will generate a makefile that don't compile the tests and don't use
relative path builds (they are useful while debugging). The last one installs
the package. 

By default, it generates a DEB and a RPM file.

Note that, unless you customize the `CMAKE_INSTALL_PREFIX`, it will
install in the `/usr/local` directory

## Things to do

Aside from issues, there are things I would like to do very soon

 - Continuous integration
 - Increase test coverage
 - Add support for vcpkg in Windows
 - Test build in clang
 - Try to build in a Mac, for those hipsters who use OS x (I kid! :wink:)

## License

Every file in this game is licensed under the MIT license, the best in
existence.

Copyright (C) 2016-2018 Arthur Mendes.


