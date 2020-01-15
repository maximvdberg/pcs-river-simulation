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


int main() {

    print("~start~");

    // Create a window.
    Window window = createOpenGLWindow("Cool :O");

    // Create the renderer object.
    GLRenderer renderer = GLRenderer();

    // Store the input data here, and initialise it.
    InputData input;

    // Create two textures.
    const int width = 500, height = 500;
    GLuint texture1 = gl::genTexture(width, height);
    renderer.renderToTexture(texture1);
    renderer.clear(1.f, 0.f, 0.f);
    renderer.renderToScreen();

    LatticeBoltzmann boltzmann = LatticeBoltzmann(renderer);

    // We now update untill the window gets closed.
    while (!input.quit) {

        // Update the input (like key presses, window events) we got this frame.
        updateInput(input);

        // Update the viewport if the window size has changed.
        if (input.windowSizeChanged) {
            input.windowSizeChanged = false;
            SDL_GL_GetDrawableSize(window.sdlData, &window.width, &window.height);
            renderer.updateViewport(window.width, window.height);
        }

        // Clear the screen so we can draw the new frame.
        renderer.clear(0.f, 0.f, 0.5f, 1.f);

        // Draw some stuff.
        renderer.renderToTexture(texture1);
        renderer.updateViewport(width, height);
        renderer.clear(0.5f, 0.f, 0.f, 1.0f);
        renderer.setRenderColor(0.4f, .8f, 3.f, 1.f);
        renderer.renderTexture(renderer.getBlankTexture(),
                               100, 100, 200, 200);
        renderer.renderToScreen();
        renderer.updateViewport(window.width, window.height);
        renderer.renderTexture(renderer.getBlankTexture(),
                        100, 100, 200, 200);

        // Draw some stuff.
        static int posX, posY;
        if (input.keyMap[SDL_SCANCODE_A] == 0) {
            posX = 140 + 100*std::cos(0.01f*SDL_GetTicks());
            posY = 100 + 60*std::sin(0.001f*SDL_GetTicks());
        }
        renderer.setRenderColor(1.f, 1.f, 1.f, 1.f);
        renderer.renderTexture(texture1,
                               posX,  posY, 500, 500);


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
