===========
Asset file
===========

Links to all resources are stored in a file called the asset
file. Usually, and in the sources, is the assets.yml file.

Here we'll show how to store the supported resource types

Texture
=======

.. code-block:: yaml

   - name: test
     path: ${TEXTURES_DIR}/test.bmp
     type: texture

Mostly auto-explanatory. `${TEXTURES_DIR}` is a sort of definition
which is filled with the default texture path in game. And this path
is compiled.

Also `name` is the in-game name for this asset: if you want the
`test.bmp` bitmap, you will have to load the texture from the
`test` asset.

Meshes
======

The engine supports lots of model formats, and some of them supports
things others do not support, like multi-texturing, materials, lights
or level-of-detail.

The game compensates this allowing you to define certain things inside
the asset file.

.. code-block:: yaml
		
   - name: mesh-tent
     path: ${MODELS_DIR}/Tent.obj
     type: mesh
     mesh.name: tent01
     mesh.texture: test
     mesh.material: mtl/tent 

Also very straightforward. `${MODELS_DIR}` is a sort of definition
which is filled with the default model path in game.

mesh.name is the chosen name for the model. It will store a mesh named
tent01 inside an asset (who can have more than one mesh) named mesh-tent.

mesh.texture is the asset name of the mesh texture, mesh.material is the asset
name for the mesh material.
