# Object Guide

This is a guide made for you that wishes to create a new object for Tribalia to render.

Please note that, for now, you'll have to patch the code for do it, but in the future, you will be able to do everything on this guide without it.

You'll need all development packages on [README](../README.md) and g++.

## Object creation

The object needs to have these things:
1. *Be in a cpp and hpp file.* It can be in any folder inside Tribalia source, but we recommend that it lies on the objects folder.
2. *Inherit from `Logic::Building` or `Logic::Unit`* This depends of the object type.
3. *Have a mesh*. 
	Please check [the model guidelines](model_contrib.md) for how to create meshes. You need to add it to the assets file, but the format is straightforward. 

## File guidelines

### Header

The header (.hpp) needs to be like this:

```c++
#include "../logic/Building.hpp"
#include "ObjectMacros.h"

#pragma once

namespace Tribalia {

class <OBJECT_CLASS_NAME> : public Logic::Building {
private:
    static const int TID = <THE_OBJECT_TYPE_ID>
public:

	<OBJECT_CLASS_NAME>() : Building(0, TID, "<OBJECT_NAME>"){};
	
    <OBJECT_CLASS_NAME>(int oid, float x, float y, float z);

    /* Called on object initialization */
    virtual bool Initialize();

    /* Called on each engine loop, when an action is performed */
    virtual bool DoAction(void);

	ADD_CLONE_MACRO(<OBJECT_CLASS_NAME>, NULL)
};

}
```

- The object class name and the object name can differ, but they don't have to
- I specified `pragma once` for the sake of easy development, but you can use the include guards if you know what they are.
- The 'include' paths are relatve to the src/objects folder

### Source
The source (.cpp) needs to be like this

```c++
#include "<HEADER_FILE>"

using namespace Tribalia;
using namespace Tribalia::Logic;

/*  250 HP, no baseAtk, 1.0 baseArmor, 1.0 building material, 0.95 bulding
    strength, 2 units of garrison capacity */
<OBJECT_CLASS_NAME>::<OBJECT_CLASS_NAME>(int oid, float x, float y, float z)
    : Building(oid, TID, <OBJECT_NAME>, x, y, z, 250, 0, 1.0, 1.0, 0.95, 2) {
        DEF_MESH("<MESH_ASSET_NAME>");

    }


/* Called on object initialization */
bool <OBJECT_CLASS_NAME>::Initialize() { return true; }

/* Called on each engine loop, when an action is performed */
bool <OBJECT_CLASS_NAME>::DoAction(void) {return true; }
```
- The comments are auto-explicative about what the method means. I simply copied these from an object file.
- These numbers (HP, baseAtk...) don't make much sense now, but they will. I will document the meaning here when it happens.

### Patching
To definitely insert your object into the game, open src/Tribalia.cpp and find the lines that start with `ObjectFactory::GetInstance()->AddObject`. Put your object in place or below the already added objects, using `new <OBJECT_CLASS_NAME>


If you are a C++ programmer, you can make these things differently, but it needs to be compatible.
 

