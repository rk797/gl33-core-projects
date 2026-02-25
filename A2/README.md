# CPSC 453 W26 Assignment 2 Boilerplate

When you submit your assignment, replace this README with your own, as specified
by the assignment instructions!

Unlike Assignment 1, where it was reasonable to only modify `main.cpp`, the
general design of your solution might become easier if you create new files that
allow you to segment which code does what.

For example, it would be understandable if your planned design benefited from
moving the GLFW callbacks class to a separate .h/.cpp file, or creating a new
.h/.cpp pair of files for your sprites or players or enemies, or
transformations, etc.

**Note that, when you create a new file, you will need to re-run CMake so that
it knows to include the new files when building!**

While we have provided an example way of how you might call the `Game.cpp`
code from your render loop in `main.cpp`, you are free to also substantially 
change how you use `Game.h` and `Game.cpp` to ease your writing of the code,
and you may move parts of the boilerplate's existing code into other or new
files, though you need to make use of the constants in Game.hpp.

## Changes from the A1 Boilerplate:

This boilerplate is quite similar to the A1 boilerplate, with the following changes:

- ImGui has been added to the `thirdparty` directory.
- New classes have been added in subdirectories within the `453-skeleton` directory.
  - `SpriteQuad.h` and `SpriteQuad.cpp`:
    **You must use these to fulfill the last part of the assignment instructions!**
  - `Game.h` and `Game.cpp`:
    Contains some ImGui draws and numerical constants you must use!
  - `GameObject.h` and `GameObject.cpp`:
    Contains an incomplete starting example of what a class for your sprites could look like.
  - `FrameTime.h` and `FrameTime.cpp`:
    For setting a max fps (see main.cpp comments for justification) and finding the time that's passed since the last frame.
- `Geometry.h` and `Geometry.cpp` have some changes because we will now be assigning
  UV coordinates to vertices instead of colours, because we want to render textures.
- `Window.h` and `Window.cpp` have changed to support ImGui integration and anti-aliasing.
- `Texture.h` and `Texture.cpp` have changed to support reading textures that are either linear encoded or sRGB encoded.
- `main.cpp` has been changed to render a single textured quad and some text via ImGui.
  - The sample callback has been changed slightly to accomodate ImGui.
  - Some initialization around the window creation has likewise been altered to
    accomodate ImGui.
  - The geometry being rendered has changed.
- `CMakeLists.txt` has undergone some small changes, e.g. to support the above.

For Assignment 2 instructions and due date, see D2L.

