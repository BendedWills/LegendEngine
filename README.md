[![Build and test (Linux)](https://github.com/BendedWills/LegendEngine/actions/workflows/tests.yml/badge.svg)](https://github.com/BendedWills/LegendEngine/actions/workflows/tests.yml)

# Legend Engine 

Legend Engine is a WIP game engine made using C++.



The goal is for it to be a fast, versatile, and feature-rich game 
engine that runs on Windows and Linux.
An important goal for the engine is to be usable either through its GUI frontend
called Legendary, as well as its API in code!

All window management and input is done from scratch and handled by a library called [Tether](https://github.com/BendedWills/Tether).

## How to build

Dependencies:
* CMake
* Some toolchain compatible with CMake like Ninja, Makefile, Visual Studio, etc.
* Vulkan SDK (if building with `LGENG_VULKAN_API`, enabled by default)

As well as:
* googletest (if building tests)
* Tether
* Vulkan Memory Allocator (VMA), if `LGENG_VULKAN_API` is enabled

which the above should be fetched automatically by CMake.

After you have the necessary dependencies, simply clone the project and run `cmake .` in the root directory.
You may need to install the [Vulkan SDK](https://vulkan.lunarg.com/), or run this command if you're on a debian-based linux:
```
sudo apt install libvulkan-dev vulkan-validationlayers vulkan-tools glslc
```

## Technologies used

* Vulkan and eventually more modern Graphics APIs
* GitHub Actions for building and testing the project automatically
* Kanban-style planners for managing development internally.