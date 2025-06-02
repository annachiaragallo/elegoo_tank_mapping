SPHERO PROJECT
===============================
Requirements
===============================
This project uses [gattlib](https://github.com/labapart/gattlib) to communicate with a Sphero Bolt from a `.c` program.
Make sure to have `gattlib` installed before compiling.

If you want `sphero` to be a to be a top-level project that consumes `gattlib` check the **Consuming Gattlib** section. However this
is inadvisable because `gattlib`'s source examples are supposed to be built as part of the enclosing `gattlib`'s CMake project, rather
than a top-level project.

Therefore the `sphero` directory should be place inside the `examples` one, and the library used as an environment for your projects.
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
add_subdirectory(examples/sphero)
```
and place the `sphero` directory in the `gattlib/examples` folder. 

Build project - step 2: preparing the sensor_data_collection folder
===============================
Before building the library again, you should decide where you
want the sensor-related `.csv`s to be generated. The default path can be found at the beginning of `sphero/include/output.h`:

```
		...
static const char base_dir[] = "./sensor_data_collection/";
		...
```
You can replace the *dot* with a path of your choice. If you leave it as it is, you have to place the `sensor_data_collection` 
folder in the same path where the executable file will be produced (`gattlib/build/examples/sphero`). Otherwise, place it in
the path you replaced the *dot* with.

The `sensor_data_collection` folder contains:
- a `test` folder containing some `.csv` files produced from a normal execution of the program
- a `.ipynb` file that graph the content of said `.csv`s.

The `.ipynb` file can be easily run from IDE sch as **VSC**, but it requires a Python Virtual Environment. **VSC** detects the
requirement and lets you link the file to a `.venv` automatically; if you can't do this automatically, simply add a virtual
environment yourself with

```
python -m venv
```

Build project - step 3: rebuilding the library
===============================
Now rebuild the library. From `gattlib/build` directory:

```
cmake ..
make
```
Afterwards you will find your executable, called `main`, in `gattlib/build/examples/sphero`.


Run executable
===============================
From `gattlib/build/examples/sphero`, run the executable with the following usage:

```
./main <output_option> argument required
options: "terminal", "directory", "notify_off"
if "directory" is chosen, additional argument <directory_name> is required
```
meaning you can choose to turno on or off the receiving of notifications, and whether the notification data is displayed on terminal
or printed on `.csv` files for the Jupyter Notebook to graph. If you choose the latter, then a third parameter is required, the name
of the directory (which will be automatically placed in `sensor_data_collection`) that will hold the `.csv`s.

A typical choice of running command might be:

```
./main directory roll_straight
```
**Notice that the bluetooth connection might take a while (up to 10 seconds) to establish**.
When the Sphero Bolt is connected, you'll be able to use the arrows on your keyboards to make it move. 
**The direction of movement depends on the Bolt's current orientation**, not the one it had at the time of the first command (more on 
this in the **Known Limitations** section).

Graphs Creation
===============================
After running the program with a command such as this:
```
./main directory roll_straight
```
, you will find a new folder called `roll_straight` inside of `sensor_data_collection`, holding all the newly generated `.csv`s.
At this point you can open the `.ipynb` file and replace the first line:

```
folder = "./test"
```
with the name of the new directory, so in our example
```
folder = "./roll_straight"
```

At this point, run the entire `.ipynb` to generate the graphs.
Good ways to check if all is working as expected in both `sphero` and `notebook` are:

- Run the program with the `directory` option. Quickly make the Bolt do a squared-shaped path. A square should appear in the "Position" graph.
- Run the program with the `directory` option. When the Bolt is connected, place your hand on it and then slowly raise your hand. The "AmbientLight"
graph should show the increase of exposure.

Known Limitations
===============================
- The Bolt's locator operates with the motors to establish the change in location/direction. This means that pressing the arrow keys changes the
 direction of the Bolt based on its **current** location and direction, not the original one. For example, the "go backward" command makes the Bolt
rotate on itself, not towards a specific direction.
- In order to maximize time performance, the buffer that holds the notifications' data is statically allocated. This means that beyond a certain amount
of time, data will go lost.

Possible improvements
===============================
- Using a different tool than `gattlib` would make the project more memory-safe
- The project is not currently compatible with Windows, but it could easily be by adding supported solutions for the arrow keys detection
- User could be made able to choosee the speed of the Bolt by adding detection of other keys, for instance + and -
- User colud be made able to choose which sensors should be considered by reintroducing the use of the `mask` functions. The coretime sensor should always
  be included unless the `print_sensors_data` function is rewritten accordingly
- User could be made able to let the Bolt follow a pre-established set of commands instead of interactive ones, by reactivating the `set_commands` block

CONSUMING GATTLIB
===============================

Installing a Gattlib Provider
===============================

* In order to consume gattlib, you need to install it in a chosen directory. It can for example be named `gattlib_provider`.
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

Having done this, you should be able to generate the build files for any of the source code files located into the `examples` folder, including new ones made by yourself, with some little modification to their `CMakeLists.txt` (from now on referred to as **CML**. 

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
     ------ src
          ------ main.c	
     ------ include
          ------ main.h
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
