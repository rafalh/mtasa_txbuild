Toxic Build
===========

Tool used for MTA resources compilation based on meta.xml.

Usage
-----
Define environment variable `TXBUILD` as path to directory where this repository has been cloned.

In every resource source directory create Makefile:

	PROJECT_NAME := <resource name>
	PROJECT_DIR  := <resource organizational directory>

	include $(TXBUILD)/generic.mak

In root source directory for all resources create `config.mak` file (customize example for your needs):

	SELF := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

	PREPROCESS     := 1
	COMPILE        := 1
	UTF8_BOM       := 0
	JOIN           := 0
	PROTECT        := 1
	LOADER_PATH    := $(SELF)loader.lua
	ifeq ($(windir),)
	 RESOURCES_PATH := /mnt/c/Program\ Files\ (x86)/MTA\ San\ Andreas\ 1.5/server/mods/deathmatch/resources/
	else
	 RESOURCES_PATH := C:/Program\ Files\ (x86)/MTA\ San\ Andreas\ 1.5/server/mods/deathmatch/resources/
	endif
	TEMP_DIR       := build

Compile resource using Make tool. On Windows you need mingw32-make (included in txbuild). For example:

	$(TXBUILD)\mingw32-make

Configuration properties
------------------------

Behaviour of txbuild can be defined in `config.mak` files in resource directory or up to 3 levels higher in filesystem hierarchy. Configuration files on higher level has lower priority. File can define any set of following optional variables:

* `PREPROCESS` - run preprocessor on every Lua file. See description of preprocessor below.
* `COMPILE` - compile Lua files using luac_mta tool
* `UTF8_BOM` - add UTF8 BOM to final Lua scripts
* `JOIN` - join all Lua scripts into one big script - recommended to increase difficulty of reverse engineering
* `PROTECT` - protect scripts by embedding them for loading by custom loader specified in `LOADER_PATH`
* `RESOURCES_PATH` - destination path for compiled resources
* `TEMP_DIR` - name of directory used for building

Preprocessor
------------

Preprocessor runs every line starting with `#` and evaluates expressions `$(...)`. See examples below.

Conditional compilation:

	#if FEATURE_A
	addCommandHandler('a', ...)
	#end

Including other file:

	#include 'a.lua'

Compile-time constants:

	#define TIMEOUT 200

	setTimer(function () end, $(TIMEOUT))

Protection with loader
----------------------
Loader is a preprocessed Lua script used for additional protection of scripts. If `PROTECT` is set to 1 every Lua script is returned as embedded in custom loader defined using `LOADER_PATH` variable in `config.mak`. When preprocessing loader file preprocessor variable `__LUA_CHUNK_TBL__` contains table of strings from input Lua file. Loader can use preprocessor to encrypt those lines and embedd them for future execution in runtime.

Building
--------
Toxic Build consists of some native components that needs to be built using a C++ compiler. The project uses CMake build system. Source code is in `src` directory.

On Windows Visual Studio can be used for building (it has CMake support built-in). Alternatively MinGW can be used with a separate CMake GUI tool. Building can be avoided by downloading `txbuild-bin-win32.zip` file from [Releases](https://github.com/rafalh/mtasa_txbuild/releases) page and unpacking it in the root directory (preserving `bin` subdirectory).

On Linux GCC and CMake can be installed from distribution package manager.

To build on Ubuntu run:

```bash
sudo apt-get install build-essentials cmake
mkdir build
cmake ../src -DCMAKE_BUILD_TYPE=Release
make
```

Copyright
---------
Copyright (c) 2012 Rafał Harabień
