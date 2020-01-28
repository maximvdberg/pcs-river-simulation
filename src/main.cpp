/**
 * The Project Computational Science River Flow main file.
 *
 * @file main.cpp
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 08-01-2020
 */

#include <iostream>
#include <cmath>

#include <SDL2/SDL.h>

#include "print.hpp"
#include "sdl/window.hpp"
#include "sdl/input.hpp"
#include "opengl/opengl.hpp"

#include "lbm/lbm.hpp"

using namespace pcs;


int main( int argc, char** argv ) {

    print("~start~");

    // Get the river file we want to simulate.
    std::string riverFile = "assets/river.bmp";
    if (argc > 1)
        riverFile = argv[1];

    // Create a window.
    Window window = createOpenGLWindow("Cool :O");

    // Create the renderer object.
    GLRenderer renderer = GLRenderer();

    // Store the input data here, and initialise it.
    InputData input;

    LatticeBoltzmann boltzmann = LatticeBoltzmann(renderer, riverFile);


    // We now update untill the window gets closed.
    while (!input.quit) {

        // Update the input (like key presses, window events) we got this frame.
        updateInput(window, input);

        // Update the viewport if the window size has changed.
        if (input.windowSizeChanged) {
            input.windowSizeChanged = false;
            renderer.updateViewport(window.width, window.height);
        }

        // Clear the screen so we can draw the new frame.
        renderer.clear(0.f, 0.f, 0.5f, 1.f);

        boltzmann.update(renderer, input, window.width, window.height);

        // Swap the buffer we have rendered to with the display buffer.
        SDL_GL_SwapWindow(window.sdlData);
    }

    boltzmann.close();
    renderer.close();
    destroyWindow(window);
    print("~end~");
    return 0;
}
