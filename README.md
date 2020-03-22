# Familyline

<p align="center">
	<img src="icons/familyline-logo.png" height="384px" width="384px" />
</p>

<p align="center">
<a href="https://travis-ci.org/arthurmco/familyline" title="travis badge">
  <img src="https://api.travis-ci.org/arthurmco/familyline.svg?branch=master" />
</a>
</p>

Familyline is a real-time strategy game focused on Ancient Era, from since the
neolythic period (~ 10.000 AD) to the death of Christ (~ 30 BD).

In it, you will guide a family, that goes from the end of neolythic until its kingdom
inclusion to the Roman Empire. You will trade with other people, war, mount strategies
and (if the money begin to appear) do a *lot* of quests.

It's made with a homebrew engine, for me to exclusively learn while
bringing joy to its players. The major objective of this game is
fun, to play and develop. 

Expect it to have a lot of good humor!

## Build and Installation

The game runs in Linux and Windows. Other platforms need to be tested.

### Penguins

On Linux, you will need the softwares below:

 - *GLM:* for vector mathematics
 - *GLEW:* for dealing with OpenGL extensions
 - *SDL:* for dealing with our window. Version 2.0 is required.
 - *devIL:* for loading our textures.
 - *cairo:* for drawing the interfaces
 - *libyaml:* For parsing the `assets.yaml` file, aka the asset list.

Please build in a separate directory from the source. For exemple,
when you clone the repository, you can create a directory named
`build` and run cmake from there. Like:

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_RELATIVE_PATH=off
-DDO_TESTS=off ..
make familyline
```

The relative path tells that some files, like assets, are in the same
directory as the executable rather than the default path (/usr/share)

### Windows

On Windows, you can use `vcpkg`. Use [these
instructions](https://docs.microsoft.com/cpp/vcpkg?view=vs-2017).
You will need to have vcpkg integrated with Visual Studio.

Execute the following command:

`> vcpkg install glm glew sdl2 devil cairo libyaml fmt`

Go make a coffee and buy needed things in the market. Order some
food. This will take some time.

When it finishes, add
`-DCMAKE_TOOLCHAIN_FILE={VCPKG_PATH}\scripts\buildsystems\vcpkg.cmake`
to your cmake command line. Replace `{VCPKG_PATH}` to where you
downloaded vcpkg.
If you used that Open Folder thing from Visual Studio -- I used --,
add the following lines into your CMakeSettings.json, inside the
variables key

```json5
{
    "name": "CMAKE_TOOLCHAIN_FILE",
    "value": "{VCPKG_PATH}\\scripts\\buildsystems\\vcpkg.cmake"
}
```

Then, cmake it


### Fruits

Follow the same steps as the penguins one, because penguins and fruits are made
by the same God, the God of UNIX compatibility

However, there are some differences:

 - You may want to install your dependencies with homebrew
 - Xcode is needed, because the OpenGL headers come with it.  
   (yes, we still use it, but Vulkan support is coming :wink:)

### Build

To build the game, type `make familyline` to compile the game, `make
familyline-server` to compile the server (that doesn't work yet) or
`make familyline-tests` to make the tests.

(On Windows, inside Visual Studio, the targets might appear as projects
inside the solutions).

We use doxygen to document the source, so if you want to generate some
source documentation, type `make docs`. The docs will be generated
inside the `docs/doxygen` folder

## Running

The executables have the same names as the targets. Have fun!

## Packaging

To generate a package, simply run these three commands:

```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_RELATIVE_PATH=off -DDO_TESTS=off ..
make package
```

The first will generate a makefile that don't compile the tests and don't use
relative path builds (they are useful while debugging). The last one installs
the package.

By default, it generates a DEB and a RPM file.

Note that, unless you customize the `CMAKE_INSTALL_PREFIX`, it will
install in the `/usr/local` directory when you run `make install`

## Things to do

Aside from issues, there are things I would like to do very soon

 - ~~Continuous integration~~
 - Increase test coverage
 - ~~Add support for vcpkg in Windows~~
 - Test build in clang
 - Try to build in a Mac, for those hipsters who use OS x (I kid!
   :wink:)
 - Support animated meshes
 
## License

Every file in this game is licensed under the MIT license, the best in
existence.

Copyright (C) 2016-2018 Arthur Mendes.
