# CPSC 453 W26 Assignment 3 Boilerplate

When you submit your assignment, replace this README with your own, as specified
by the assignment instructions!

As with Assignment 2, you might find it useful to create new files for new
or existing code.
**Note that, when you create a new file, you will need to re-run CMake so that
it knows to include the new files when building!**

## Changes from the A2 Boilerplate:

This boilerplate is quite similar to the A2 boilerplate, with the following changes:

- The `time` and `game` subdirectories have been removed.
- `Panel.h` has been added. This is currently an extremely minimal file that
  provides an interface that you can use to easily swap which ImGui panel you
  render based on the current scene, as hinted at in `main.cpp`. The current
  `main.cpp` creates a single panel using this interface, but you can create
  multiple and choose which one to use.
  - You may also find it helpful to move/add additional ImGui code to this file.
- The ImGui version has been updated. 
- The `Window` class has changed to better support a mixture of ImGui widgets
  and GLFW callbacks.
- The `Geometry` class and the shaders more closely resemble those of
  Assignment 1, passing colours instead of UV coordinates to the GPU. 
- `main.cpp` has been changed to render a few sample control points and a sample GUI.
- The `GLDebug` namespace has been changed to ignore certain unimportant OpenGL
  debug information, so as to not flood the terminal with repeat messages.
- `CMakeLists.txt` has undergone some small changes, e.g. to support the above.

For Assignment 3 instructions and due date, see D2L.

