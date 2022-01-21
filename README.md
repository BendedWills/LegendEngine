# Legend Engine 

Legend Engine is a game engine made using C++.

There is also Legendary, the UI editor for LegendEngine.

Currently, Legend Engine only works for Linux, but a Windows version will also eventually be available.

# Compilation

The `Build.sh` script will build LegendEngine, Legendary, and all tests for both of them. Build files will then be in the `Build` directory.

But if you want to build the project without using the shell script, here is how:

# Linux build instructions
If you are on linux and want to build this project, you've come to the right place.

First, run `git submodule update --init` to clone all dependencies.

Then, run CMake to generate the Makefile like so: 

	cmake -S . -B Build

Then, run the make command to compile the executable:

	make -j8 -C Build all

The -C option tells make that the makefile is located in the "Build" directory.

The -j8 option tells make to use 8 threads to build the executable. Replace the 8 with however many cores you have on your computer.

And there you go! All projects and tests will now be compiled and can be run by their executables in the build directory.