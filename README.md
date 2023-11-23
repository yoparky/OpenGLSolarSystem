# OpenGL Solar System

## Demo
### A Relative Model of the Solar System
![General Demo](demo-gifs/SolarSystem01.gif)

### Simulation Controls Demo
![Simulation Control](demo-gifs/SolarSystem02.gif)

## Description
This project is an OpenGL-based simulation of the Solar System. It demonstrates the use of OpenGL and GLUT for rendering a 3D scene with rotating planets around the Sun and also showcases basic lighting and texturing techniques.
The relative size (except the Sun due to it's massive size!), rotational & orbital period of each planets, and distance are scaled to be relatively accurate.
Lighting has been added to the Sun to simulate sunlight.
Pressing 1 and 2 toggles the background stars and orbital paths respectively.
Pressing O and P toggles between orthographic projection (maintains parallel lines, no sense of depth) and perspective projection (parallel lines skewed towards vanishing points, provides sense of depth)
Pressing F freezes the animation
Pressing ESC or Q quits the simulation

## System Requirements
- C++ Compiler (e.g., GCC, Clang, MSVC)
- OpenGL libraries
- GLEW (The OpenGL Extension Wrangler Library)
- FreeGLUT

## Installation

### Windows
1. Install a C++ compiler like Visual Studio, GCC, or Clang.
2. Download and install [GLEW](http://glew.sourceforge.net/).
3. Download and install [FreeGLUT](http://freeglut.sourceforge.net/) OR [GLFW](https://www.glfw.org/)
4. Clone or download this repository to your local machine.

### Linux
1. Install a C++ compiler and the build essentials.
2. Install GLEW and FreeGLUT using your distribution's package manager. For example, on Ubuntu:
   ```bash
   sudo apt-get install libglew-dev freeglut3-dev

### Running the Program
To run the program, compile the source code and execute the generated binary. 
For Windows using Visual Studio, open the solution file and build the project after installing
Glew and GLFW.

## License
[MIT](https://choosealicense.com/licenses/mit/)

## Acknowldegements
Thanks to the developers of OpenGL, GLew, and GLFW
Thanks to [NASA 3D Resources](https://nasa3d.arc.nasa.gov/images) for images and textures for the planets and the Sun 
