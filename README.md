ELEGOO PROJECT
===============================
Requirements
===============================
This project uses [gattlib](https://github.com/labapart/gattlib) to communicate with an Elegoo OwlBot Smart Tank from a `.c` program.
Make sure to have `gattlib` installed before compiling.

If you want `elegoo_tank_mapping` to be a to be a top-level project that consumes `gattlib` check the **Consuming Gattlib** section. However this
is inadvisable because `gattlib`'s source examples are supposed to be built as part of the enclosing `gattlib`'s CMake project, rather
than a top-level project.

Therefore the `elegoo_tank_mapping` directory should be place inside the `examples` one, and the library used as an environment for your projects.
This requires a small modification to the library's the top-level `CMakeLists.txt`.

Build project - step 1: adding target for top level CMake
===============================
In the top-level `CMakeLists.txt`, find the block that adds the examples' code subdirectories:

```
		...
add_subdirectory(examples/advertisement_data)
add_subdirectory(examples/ble_scan)
add_subdirectory(examples/discover)
		...
```
then add the following line:
```
add_subdirectory(examples/elegoo_tank_mapping)
```
and place the `elegoo_tank_mapping` directory in the `gattlib/examples` folder. 

Build project - step 2: inserting your own Tank's address
===============================
In `elegoo_tank.h`, replace the address indicated in
```
		...
/*Device's address*/
static const char OWL_BOT_ADDR[] = "AA:BB:CC:DD:EE:FF";
		...
```
with the address of your own Tank OwlBot. You can find it by using command-line tools such as `bluetoothctl` or `gatttool`. 

Build project - step 3: preparing the mapping folder
===============================
Before building the library again, you should decide where you
want the mappiing-related `.csv` and `.svg` to be generated. The default path can be found at the beginning of `elegoo_tank_mapping/include/draw.h`:

```
		...
#define ROUTE_PATH      "./route.html"
#define DIRECTIONS_PATH "./directions.csv"
		...
```
You can replace the *dot* with a path of your choice. If you leave it as it is, the files will be created in the same folder where the executable gets
generated. This might be annoying because you'd have to manually delete them or add a target rule for the `make clean` command. 

Build project - step 4: rebuilding the library
===============================
Now rebuild the library. From `gattlib/build` directory:

```
cmake ..
make
```
Afterwards you will find your executable, called `elegoo_main`, in `gattlib/build/examples/elegoo_tank_mapping`.

Build project - step 5: change the code on the Arduino Board
===============================
- Download [Elegoo' source code for the OwlBot](https://www.elegoo.com/blogs/arduino-projects/elegoo-owlbot-smart-robot-tank-tutorial).
- Navigate to `ELEGOO OwlBot Tank Tutorial 2021.05.08/ELEGOO OwlBot Tank Tutorial 2021.05.08/02 Manual & Main Code & APP/02 Main Program/TB6612/OwlBotTankKit_Driver_20201210` (or `ELEGOO OwlBot Tank Tutorial 2021.05.08/ELEGOO OwlBot Tank Tutorial 2021.05.08/02 Manual & Main Code & APP/02 Main Program/DRV8835/OwlBotTankKit_Driver_20201210`, depending on which chip is on your board)
- Replace the original file `ApplicationFunctionSet_xxx0.cpp`with the one in this repo
- Connect the Bot's board to the Arduino IDE (the process to do it is explained in detail in Elegoo's tutorial)
- Upload the modified code
- Remember to switch back to Bluetooth mode when you are ready to run the program

Run executable
===============================
From `gattlib/build/examples/elegoo_tank_mapping`, run the executable.
```
./elegoo_main
```

**Notice that the bluetooth connection might take a while (up to 10 seconds) to establish**.
When the OwlBot is connected, you'll see it avoiding obstacles with less velocity and more durability compared to the original program. From the terminal you'll be able to follow the mode switching (10 in total). 

Map Creation
===============================
After running the program you will find, in the directory you chose at the beginning, a `.csv` file holding all the informations about the mode switching, and a `.html` file with the resulting SVG.

Known Limitations
===============================
This project served as a base for a more complex one (see my sphero repo). Therefore many features are only approximated: the angles are not calculated, the mode switching is not the most precise way to collect
data about obstacles, and the generated SVG is little more than a sketch, little attention was given to best practices and private sections. In the future all of these aspects might be improved and dealt with; also
the `gattlib` library might get replaced with a more memory-safe tool. 

CONSUMING GATTLIB
===============================

Installing a Gattlib Provider
===============================

*In case you have trouble using gattlib, try to to install it in a chosen directory. It can for example be named `gattlib_provider`.
Follow these steps:

```
cd gattlib_provider
git clone git@github.com:labapart/gattlib.git
cd gattlib
cmake -B build -DCPACK_PACKAGE_INSTALL_DIRECTORY=$(pwd)/install
cmake --build build
cmake --install build --prefix install
```

* at this point, the output should be something like this:

```
-- Install configuration: "Debug"
-- Installing: /some/path/gattlib_provider/gattlib/install/lib64/libgattlib.so
-- Installing: /some/path/gattlib_provider/gattlib/install/include/gattlib.h
-- Installing: /some/path/gattlib_provider/gattlib/install/lib64/pkgconfig/gattlib.pc
```


* with either `lib` or `lib64` depending on your system. Now export the var:

```
export PKG_CONFIG_PATH=$(pwd)/install/lib64/pkgconfig
```
OR
```
export PKG_CONFIG_PATH=$(pwd)/install/lib/pkgconfig
```
if that's the output you've got.

Building examples (both gattlib's and your own)
=================================================

Having done this, you should be able to generate the build files for any of the source code files located into the `examples` folder, including new ones made by yourself, with some little modification to their `CMakeLists.txt` (from now on referred to as **CML**). 

* To build `discover.c`, for example, its CML should look like this:

```
cmake_minimum_required(VERSION 3.22.0)

find_package(PkgConfig REQUIRED)

pkg_search_module(GATTLIB REQUIRED IMPORTED_TARGET gattlib)
pkg_search_module(PCRE REQUIRED IMPORTED_TARGET libpcre)

set(discover_SRCS discover.c)

add_executable(discover ${discover_SRCS})
target_link_libraries(discover PRIVATE PkgConfig::GATTLIB PkgConfig::PCRE pthread)
```

* to give another example, the CML of `read_write.c` should look like this:

```
cmake_minimum_required(VERSION 3.22.0)

find_package(PkgConfig REQUIRED)

pkg_search_module(GATTLIB REQUIRED IMPORTED_TARGET gattlib)
pkg_search_module(PCRE REQUIRED IMPORTED_TARGET libpcre)
pkg_search_module(GLIB REQUIRED IMPORTED_TARGET glib-2.0)

include_directories(${GLIB_INCLUDE_DIRS})
include_directories(include)
set(read_write_SRCS read_write.c)

add_executable(read_write ${read_write_SRCS})
target_link_libraries(read_write PRIVATE PkgConfig::GATTLIB PkgConfig::GLIB PkgConfig::PCRE m pthread)
```
You can add examples to `gattlib_provider`, **maintaing the expected tree**. 

Let's use `discover.c` to see how an example can be built:

```
cd gattlib_provider/gattlib/examples/discover

cmake -B build -DCMAKE_C_FLAGS="-DGATTLIB_LOG_LEVEL=3" 
#the flag is only necessary if the .c makes use of that macro

cmake --build build 
#this should create an executable in discover/build
```

Building from another directory
====================================

You can generate the build files directly in another directory if you want. After following the steps in the section
**Installing a Gattlib Provider**, you can copy paste `discover.c` and its CML (or any other example, or your own `.c` and CML, following the indications of section **Building Examples**) into your preferred directory, for example `gattlib_consumer/project1`, and generate the build files from there:

```
export PKG_CONFIG_PATH=/some/path/gattlib_provider/gattlib/install/lib64/pkgconfig 
# since you're in another dir, you can't use $(pwd): you need the complete path. If you forget this step you'll probably
# have some cmake errors; in this case, you need to check if an incomplete build directory has been generated all the same.
# If it has, delete it with all its content before trying again
									
cmake -B build -DCMAKE_C_FLAGS="-DGATTLIB_LOG_LEVEL=3" 
#the flag is only necessary if the .c makes use of that macro
cmake --build build 
#this should create an executable in discover/build

```
These sections' instructions have been put together with the help of [mr. Gamberini](https://www.kitware.com/vito-gamberini/)

Less minimalistic projects
================================

As you can see, `discover`, like all other examples, only consists in a `.c` file, but the steps to build a more structured project are the same, as long as the CML is written correctly. For example a project like this:
```
+ project
    |-- src
         |------ main.c	
    |-- include
         |------ main.h
```

would require a CML similar to this:
```
cmake_minimum_required(VERSION 3.22.0)

find_package(PkgConfig REQUIRED)

pkg_search_module(GATTLIB REQUIRED IMPORTED_TARGET gattlib)
pkg_search_module(PCRE REQUIRED IMPORTED_TARGET libpcre)
pkg_search_module(GLIB REQUIRED IMPORTED_TARGET glib-2.0)

include_directories(${GLIB_INCLUDE_DIRS})
include_directories(include)
set(main_SRCS src/main.c)

add_executable(main ${main_SRCS})
target_link_libraries(main PRIVATE PkgConfig::GATTLIB PkgConfig::GLIB PkgConfig::PCRE m pthread)
```

Updating the executable
==============================

after updating the source code, you can re-run:
```
cmake --build build
```

from the parent directory of your build directory. This will update the executable.
