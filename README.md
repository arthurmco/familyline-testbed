# Familyline

<p align="center">
	<img src="icons/familyline-logo.png" height="384px" width="384px" />
</p>

Familyline is a real-time strategy game focused on Ancient Era, from since the
neolythic period (~ 10.000 AD) to the death of Christ (~ 30 BD).

In it, you will guide a family, that goes from the end of neolythic until its kingdom
inclusion to the Roman Empire. You will trade with other people, war, mount strategies
and (if the money begin to appear) do a *lot* of quests.

It's made with a homebrew engine. The major objective of this game is
fun, to play and develop. Expect it to have a lot of good humor!

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

### Windows

On Windows, you can use `vcpkg`. Use [these
instructions](https://docs.microsoft.com/cpp/vcpkg?view=vs-2017).
You will need to have vcpkg integrated with Visual Studio.

Execute the following command:

`> vcpkg install glm glew sdl2 devil cairo libyaml`

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

Then, cmake it and build everything

## Running

Type `make familyline` to compile the game, `make
familyline-server` to compile the server (that doesn't work yet) or
`make familyline-tests` to make the tests.

The executables have the same names as the targets. Have fun!

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
