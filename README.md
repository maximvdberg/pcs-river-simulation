# Simulating river meandering using a modified Lattice Boltzmann method.
## Introduction
Project Computation Science at the UvA - Simulating river flow and meandering


## Installation





## Running the simulation

Type `./build.main.o {{river bitmap file}}` in the root directory to run the program.
While the program is running, the user can use the following keys to interact with the simulation:
- `Q` toggles the water source.
- `W` toggles erosion.
- `E` toggles sedimentation.
- `T` activates both erosion and sedimentation.
- `R` toggles the wall display between pure white and pressure-based.
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

Reports and a overview poster will be available in the `reports` folder, at a later date.

## Notes for reproducing the figures
The model used some parameters.

In most experiments were we used erosion and sedementation, we started by waiting for the flow the stabilize before activating erosion and sedementation simultaniously.

### Viscosity demonstration

### Bumpy


### Inward flow bias demonstration

### Evolution of a simple river

### Just for fun
Try enabling the corrosion and sedimentation _before_ the flow has stabilised. Because of the no-slip condition, there will actually be _more_ flow against the walls like this, which leads to some interesting patterns.
