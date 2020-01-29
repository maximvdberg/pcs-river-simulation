# Simulating river meandering using a modified Lattice Boltzmann method.
## Introduction

This project is an implementation of a modified LBM method that runs on the GPU, using C++ and OpenGL. The method is modified to include erosion and sedimentation, with which we seek to simulate the meandering of rivers.

For more information on the project, including the motivation and research goals, see the `reports` folder.

This document describes how to install and run the program, as well as a guide on how to reproduce the figures that can be found within the poster and report.

_Made for the Project Computation Science Course at the UvA._


## Installation
This project was developed on Linux, but you should be able to build for Windows and Mac as well.

### Dependencies
In order to build the program SDL2 and OpenGL version 4.3 or higher are required. On Ubuntu SDL2 can be installed using

`sudo apt install libsdl2-dev libsdl2-image-dev`

### Building

Navigate to the root directory and type

`make clean`
`make`

to build the program into the `build` directory. And that should be it!

## Running the simulation

Type `./build/main.o {{river bitmap file}}` in the root directory to run the program.
While the program is running, the user can use the following keys to interact with the simulation:
- `Q` toggles the water source.
- `W` toggles erosion.
- `E` toggles sedimentation.
- `T` activates both erosion and sedimentation.
- `R` toggles a `slope', which pulls the fluid to the right.
- `P` pauses or unpauses the model.
- `O` resets the position of the walls.

To control the display, the following keys are available:
- `1`, `2`, ... ,`9` Zoom to increasingly smaller scales.
- `Arrow Keys` Move the viewport.
- `ESC` Resets the viewport to its default position.

The user can use the left mouse button to place and drag a crosshair. It can be removed using the right mouse button. Upon being placed, the user will be shown data for the specified lattice point, at that timestep. Additionally, placing a crosshair allows the user to use the following keys:
- `V` displays the data for the specified point. It can be held so that it continuously displays the updated information.
- `X` displays the rightward momentum of all non-wall lattice points on a vertical line with the crosshair.
- `Y` displays the upward momentum of all non-wall lattice points on a horizontal line with the crosshair.

## River bitmap files
A bitmap file must be specified as input for the program. This will decide the model's map. The following colors can be used to specify aspects of the map.
- _Green_ specifies the location of walls at the start of the model.
- _Yellow_ specifies the locations of indestructible walls.
- _Blue_ specifies the source points of the stream.
- _Black_ specifies empty space.





## Analysis and Reports

Reports and a overview poster will be available in the `reports` folder (some at a later date).

## Notes for reproducing the figures
### General
The model used some parameters which can only be edited internally. The implementation of the model is located at `src/lbm/lbm.frag`, and at line 31 the parameters in question can be found. Be warned that the model becomes numerically unstable at low viscosities, or high values of `u_0`. The values currently set in the files, and those detailed below, should be relatively stable.

When running the experiments, assume all figures utilise _flow from a source_ (toggled with `Q`), not a _slope_ (toggled with `R`), unless this is specified below.

For most experiment it is _necessary_ to wait for the flow to balance out before starting the erosion/sedimentation or data extraction, which might take some time. Additionally, erosion and sedimentation were started simultaneously using `T`.

### viscosity_high.png and viscosity_low.png
The specific viscosities used here were v = 0.005 (low) and v = 0.020 (high). The `viscosity` parameter needs to be edited internally as explained above. It can be then be run using

`./build/main.o assets/river.bmp`

### sed-ero-demonstration.png
Depending on exactly when sedementation and erosion are activated, the results may differ. The island is not always wiped away as shown in the figure.

`./build/main.o assets/bumpy.bmp`

### Omega.png
This picture was made using the


### river-developement.png
The figure shows the progression of the river at 3 points in time. The model can be run for the map in question using

`./build/main.o assets/river3.bmp`


### poiseuille_flow.png and bias_flow.png
The data for these graphs was gathered with the function of the `X`-key explained above. For both of these results it was important to wait for a long time so that the flow could settle. The python code used to generate the graphs is included in this Git, specifically, `flow_poisuelle.py` and `flow_bias.py`.

For poiseuille_flow.png, the wider tube in `assets/poiseuille.bmp` was used. Stream was not activated, instead the slope, activated with `R`, was used. Since the flow this generates is very weak, it may not be visible on screen. Here it also important to set the x flow variable `u0_x` in `src/lbm/lbm.cpp` to 0.1, which speeds up the initalisation.

`./build/main.o assets/poiseuille.bmp`

For bias_flow.png, the map `Omega.bmp` was used. The results  were measured at the peak of the bend. We repeated the experiment multiple times, using different viscosities, as shown in the graph. The viscosity will need to be edited as described above. It can be run using

`./build/main.o assets/Omega.bmp`


### Just for fun
Try enabling the corrosion and sedimentation _before_ the flow has stabilised. Because of the no-slip condition, there will actually be _more_ flow against the walls like this, which leads to some interesting patterns.

We mentioned the instability of the model when the viscocity becomes to small or the in-flow too high. This can actually create some fancy patterns, so feel free to try out viscosities around 0.00005.
