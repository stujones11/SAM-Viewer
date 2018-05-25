SAM-Viewer
==========

**Skin & Model Viewer - Version 0.1.0**

A simple 3d mesh viewer built with Irrlicht rendering engine.

Features
--------

* Wielded item or 'attachment' model support.
* Multiple textures, up to 6 material layers.
* Mesh debug view. (wire-frame, skeleton and normals)
* Animation playback amd frame controls.

Supported Formats
-----------------

Compatible with all mesh formats supported by Irrlicht engine.

* B3D files (.b3d)
* DirectX (.x)
* Alias Wavefront Maya (.obj)
* 3D Studio meshes (.3ds)
* Lightwave Objects (.lwo)
* COLLADA 1.4 (.xml, .dae)

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

* Improve file-browser.
* Basic lighting.
* Image capture tools.

Screenshot
----------

![Imgur](https://i.imgur.com/xIS7pRj.png)

