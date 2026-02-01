# Legend Engine 

Legend Engine is a game engine made using C++.

There is also Legendary, the UI editor for LegendEngine.

# How to build

First, clone the repository with something like `git clone https://github.com/BendedWills/LegendEngine`

LegendEngine must be built with some graphics functionality. This is chosen with the CMake options `LGENG_VULKAN_API` for Vulkan and `LGENG_OPENGL_API` for OpenGL (currently unimplemented).

## With `LGENG_VULKAN_API = ON`

Install the Vulkan SDK from https://vulkan.lunarg.com/, or use the vulkan-sdk apt package on debian based linux distros, then continue to the final step.

## Final step

Run `cmake .` in the root directory of the repository. Then, build using the chosen CMake generator, like Visual Studio, Ninja, or makefiles.