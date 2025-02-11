# SDL_EXT_GLSL
SDL2 Static Library Extension to Simplify using OpenGL and OpenGL Shaders

* [Install](#install)
* [Use](#use)
  * [Setup](#setup)
  * [Docs](#docs)
* [Build](#build)
  * [Testing](#testing)
  * [Building](#building)
  * [Package/Distribute](#packagedistribute)
* [Contribute](#contribute)



## Install
* Unpack `SDL_EXT_GLSL` somewhere easy to reference by your compiler.



## Use

### Setup
Link library and include alongside SDL2 and OpenGL in your project.

Refer to [Docs](docs) for how to use.

### Docs
TODO: setup doxygen or similar to create markdowns in a doc folder or webpages in another repo to host on github-pages


## Build
Working with source code or building for a specific system.

### Testing
> Note: This is an option step, you can jump straight to [Buidling](#building) without needing to link SDL2 or OpenGL.

#### Setup
Configure include and linker as if any other SDL2 + OpenGL project.

The following sections assume you at least have SDL2 and OpenGL installed on your system:
> Note: see [wiki.libsdl.org/SDL2/FrontPage](https://wiki.libsdl.org/SDL2/FrontPage) for more info on installing and using SDL2.

##### Preparing Build Directory
* Create `build` directory in project's root folder
* Create `resources` directory in `build`
* Copy files (excluding `README.md`) from `./test_resources/` into `./build/resources/`
> Note: Be sure to add `SDL2.dll` to `build` if using Windows (pick `x86` or `x64` as needed)

##### Windows (cl.exe)
* Set working dir: `.\src`
* Flags: `/EHsc /nologo /Ox`
* Source targets to build: `test_lib.c sdl_gl.c glsl_shader.c glsl_ext.c`
* Build output: `/Fe..\build\test.exe`
* Add SDL2 include dir: `/I"<full-path-SDL2-include-dir>"`
* Configure Linker: `/link`
  * Set as console: `/SUBSYSTEM:CONSOLE`
  * Add Libraries: `Shell32.lib SDL2.lib SDL2main.lib OpenGL32.lib`
  * Add SDL2 lib dir: `/LIBPATH:"<full-path-SDL2-lib-dir>"`
    > Note: `<full-path-SDL2-lib-dir>` can be `x86` or `x64` (depends only on `cl.exe` selected for build)

##### Linux (gcc)
> Note: Run `$ sdl2-config --cflags --libs` to get SDL args for gcc
* Set working dir: `./src`
* Flags: `-O3 -DNO_SHARED_MEMORY -D_REENTRANT -D_THREAD_SAFE`
* Add SDL2 include dir: `-I<full-path-SDL2-include-dir>`
* Add Libraries: `-lm -lSLD2 -lGL`
* Build output: `-o ../build/test`
* Source tagets to build: `test_lib.c sdl_gl.c glsl_shader.c glsl_ext.c`

##### Running Test
Execute `.\build\test.exe`

Controls:
* Move: `W` `A` `S` `D` `Shift` `CTRL`
* Rotate: `Arrow Keys`
* Select Shader: `[` `]`
* Select Texture: `;` `'`
* Toggle Smooth Texturing: `L`
* Toggle Quad Spin: `Spacebar`

Expected four BMP textures.  First and last should have alpha channels.

### Building
Building the static library.
> Note: SDL2 and OpenGL linking not required for this step

#### Windows (cl.exe/lib.exe)
* Build Objects (`cl.exe`):
  * Set working dir: `.\src`
  * Flags: `/EHsc /nologo /Ox /c`
  * Source targets to build: `sdl_gl.c glsl_shader.c glsl_ext.c`
  * Add SDL2 include dir: `/I"<full-path-SDL2-include-dir>"`
* Build Library (`lib.exe`):
  * Set working dir: `.\src`
  * Build output: `/OUT:..\build\SDL_EXT_GLSL.lib`
  * Obj targets to build: `sdl_gl.obj glsl_shader.obj glsl_ext.obj`

#### Linux (gcc/ar)
* Build Objects (`gcc`)
   > Note: Run `$ sdl2-config --cflags` to get SDL args for gcc
  * Set working dir: `./src`
  * Flags: `-O3 -c -DNO_SHARED_MEMORY -D_REENTRANT -D_THREAD_SAFE`
  * Add SDL2 include dir: `-I<full-path-SDL2-include-dir>`
  * Source tagets to build: `sdl_gl.c glsl_shader.c glsl_ext.c`
* Build Library (`ar`)
  * Flags: `rcs`
  * Build output: `../build/libSDL_EXT_GLSL.so`
  * Obj target to build: `sdl_gl.o glsl_shader.o glsl_ext.o`

### Package/Distribute

#### Windows
Run batch script: `scripts\package_for_dist.bat`
> Note: Can run from project root or `scripts` directory

#### Linux
Run script: `scripts\package_for_dist.sh`
> Note: Can run from project root or `scripts` directory

After scripts, `dist` with subdirectory `SDL_EXT_GLSL` should exist.
Enter `dist`, compress `SDL_EXT_GLSL` with versioning.
Distribute as needed.



## Contribute
Nothing at this time