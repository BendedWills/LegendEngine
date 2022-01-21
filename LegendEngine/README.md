# Legend Engine 

Legend Engine is a game engine made using C++.

Eventual goals:

* Support many different graphics APIs such as OpenGL, Vulkan, DirectX 11, and DirectX 12 (ambition much?).
* Have both 2d, and 3d modes.
* 2d lights
* 3d lights
* post processing
* 2d physics
* 3d physics
* Objects and Components
* Window customization
* 2d shadows
* Game save data management.
* Fully programmable but also have an optional UI called Legendary.

# Preprocessor definitions
Legend Engine has a number optional features that you can compile it with so that your applications are always as small as possible. These are enabled with preprocessor definitions that are defined at compile-time. Defined like so in CMake:
```cmake
add_definitions(-DFEATURE_HERE)
```

* **-DVULKAN_API** : Defined if the engine should be compiled with Vulkan support.
* **-DOPENGL_API** : Defined if the engine should be compiled with OpenGL support.
* **-DWINDOW_MANAGEMENT** : Defined if the engine should come with window management; e.g. if the engine has the ability to create and manage its own windows.
* **-DTHREE_DIM** : Defined if the engine should be compiled with 3d support.
* **-DTWO_DIM** : Defined if the engine should be compiled with 2d support.

# Dependencies

*If compiled with -DVULKAN_API:*
* vulkan

*If compiled with -DWINDOW_MANAGEMENT:*
* tether

*If compiled with -DOPENGL_API:*
* GL (aka opengl)

*If compiled with -DTHREE_DIM:*
* physx

*If compiled with -DTWO_DIM:*
* box2d