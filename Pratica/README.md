# opengl-quake

A simple Quake map viewer in OpenGL originally made by Johan Gardhage and further improved by Bruno Hayek.

The main goal for this fork is to render the Quake map viewer using the Fast Inverse Square Root, a traditional calculation method or a Taylor series approximation. More details are give in the following repository: https://github.com/BrunoEAH/FISR-3D-Rendering. We have further improved the building file so that it also generates a executable for Windows. In the end, we adapted the code to C++20 and also added a window to see the benchmark for each calculation method.

![Screenshot](/screenshots/quake.png "quake")

![GIF](/screenshots/lv_0_20250524193135.gif "quake")


## Prerequisites

To build opengl-quake, you must first install the following tools:

- [SDL 2.0.4](https://www.libsdl.org/download-2.0.php) (or later)
- [GLU](https://en.wikipedia.org/wiki/OpenGL_Utility_Library)
- [Ninja 1.5](https://ninja-build.org/) (or later)
- C++20

We also used ImGUI (https://github.com/ocornut/imgui), but its files are located in the third_party folder.

### Install dependencies

#### openSUSE

`$ sudo zypper install ninja gcc-c++ libSDL2-devel glu-devel`

#### Ubuntu

`$ sudo apt install ninja-build g++ libsdl2-dev libglu1-mesa-dev`

## Build instructions

To build the quake demo program, run:

`$ ninja`

A `build` directory will be created, containing the demo program.

## Usage

```
Usage: quake [OPTION]...

Options:
 -h, --help         Display this text and exit
 -w, --window       Render in a window
     --fullwindow   Render in a fullscreen window
 -f, --fullscreen   Render in fullscreen
 -c, --showcursor   Show mouse cursor
     --nocursor     Hide mouse cursor
Press F to turn on the FISR algorithm
Press T to turn on the traditional algorithm
```

## License

Licensed under MIT license. See [LICENSE](LICENSE) for more information.

## Authors

* Johan Gardhage
* Bruno Hayek
* Gustavo Ciola
* Caio Alexandre de Andrade
* Nicolas Melnik
