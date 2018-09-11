SAM-Viewer
==========

**Skin & Model Viewer - Version 0.3**

A simple 3d mesh viewer built with Irrlicht rendering engine.

Features
--------

* Wielded item or 'attachment' model support.
* Multiple textures, up to 6 material layers.
* Mesh debug view. (wire-frame, skeleton and normals)
* Animation playback amd frame controls.

Supported Mesh Formats
----------------------

* Irrlicht scenes (.irr)
* Irrlicht static meshes (.irrmesh)
* 3D Studio meshes (.3ds)
* B3D files (.b3d)
* Alias Wavefront Maya (.obj)
* Lightwave Objects (.lwo)
* COLLADA 1.4 (.xml, .dae)
* Microsoft DirectX (.x) (binary & text)
* Milkshape (.ms3d)
* OGRE meshes (.mesh)
* My3DTools 3 (.my3D)
* Pulsar LMTools (.lmts)
* Quake 3 levels (.bsp)
* Quake 2 models (.md2)
* Quake 3 models (.md3)
* DeleD (.dmf)
* FSRad oct (.oct)
* Cartography shop 4 (.csm)
* STL 3D files (.stl)

Supported Texture Formats
-------------------------

* JPEG File Interchange Format (.jpg)
* Portable Network Graphics (.png)
* Truevision Targa (.tga)
* Windows Bitmap (.bmp)
* Adobe Photoshop (.psd)
* Zsoft Paintbrush (.pcx)
* Portable Pixmaps (.ppm)
* Quake 2 textures (.wal)

Installation
------------

For now this assumes you are using some sane linux distro and have
a c++11 compliant compiler, although I am pretty sure it could be
made to work on any platform or device that meets the requirements.

**Requirements:** cmake, opengl, Irrlicht
```
cmake .
make -j2
```

**CMake options:** (defaults)
```
IRRLICHT_INCLUDE_DIR=/usr/include/irrlicht
IRRLICHT_LIBRARY="/usr/local/lib/libIrrlicht.so"
```

**Example:**
```
cmake . -DIRRLICHT_LIBRARY="/usr/lib/x86_64-linux-gnu/libIrrlicht.so"
```

Controls
--------

| Control                       | Action                                                         |
|-------------------------------|----------------------------------------------------------------|
| Left mouse button + move      | Trackball style rotation                                       |
| Mouse wheel                   | Zoom                                                           |
| +                             | Zoom in                                                        |
| -                             | Zoom out                                                       |
| Arrow keys                    | Rotate around X and Y axes in 15 degree steps                  |
| Z, X                          | Rotate around Z axis in 15 degree steps                        |
| Home                          | Reset zoom and rotation                                        |
| F5                            | Reload textures                                                |
| Space                         | Jump (experimental)                                            |

To Do
-----

* Basic lighting.
* Image capture tools.

Screenshot
----------

![Imgur](https://i.imgur.com/xIS7pRj.png)

