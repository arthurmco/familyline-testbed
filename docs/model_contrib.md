# Model contribution

Tribalia supports, for now, two types of model formats: Wavefront OBJ and MD2, without animation 
(MD2 animation support is partially working on _graphical_ branch

Please note that the 'model unit 1' is equal to 1 meter, and the camera is +24m high, so take note of
the size of your model to it not appear so big on Tribalia than you expected (most md2 models I saw
are ~40m high). For comparison, the default cube on Blender, and probably on other modeling software, is 1m^3

But there are some guidelines you'll have to follow if you want Tribalia to correctly load your model. 
They are made specifically for Blender3D, 'cause it is what I use, but you can adapt it for other modelling 
software if you're smart.

## OBJ
* To export for OBJ format, the Forward direction needs to be -Z and the Up direction needs to be Y 
(Forward: -Z forward and Up: Y up)

* "Triangulate Faces" *must* be selected, otherwise the engine won't load your model correctly.

* The "Write Materials" option can be let checked, it will add an mtl file there. However, the usage of this option
will be discouraged in the future.

* OBJ animations are not supported and never will be.

* 1 mesh per file. There's support for more in the parser, but not in the mesh opener nor in the asset

* The other defaults are OK.

## MD2
* Before exporting, please rotate the model so the Y axis (the green arrow) points to the model up direction, and
the Z direction (the blue arrow) points to the back of your model.

* You need to specify the texture in the code. Tribalia doesn't support texture loading from MD2 and MD2 format
doesn't specify material names. But the UV coordinates are read correcly.

* MD2 animations are supported on graphical branch, but without respecting framerate (it will run at game framerate)

* The defaults are OK.
