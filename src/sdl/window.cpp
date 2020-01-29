/**
 * Creation and destruction of SDL windows.
 *
 * @file window.cpp
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 08-01-2020
 */

#include "window.hpp"

#include <SDL2/SDL_image.h>

#include "../print.hpp"

using namespace pcs;


Window pcs::createOpenGLWindow( const std::string& title, int width, int height ) {

    Window window;
    window.width = width;
    window.height = height;
    window.sizeChanged = true;

    // Init SDL and it's window.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        print(INFO_, "SDL failed to initialise! SDL Error:",
              SDL_GetError());
        return {nullptr, nullptr};
    }

    // Initialize SDL image.
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        print(INFO_, "SDL_image could not initialise! "
              "SDL_image Error:", IMG_GetError());
        return {nullptr, nullptr};
    }

    window.sdlData = SDL_CreateWindow(title.c_str(),
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      window.width, window.height,
                                      SDL_WINDOW_SHOWN |
                                      SDL_WINDOW_RESIZABLE |
                                      SDL_WINDOW_OPENGL);

    if (window.sdlData == nullptr) {
        print(INFO_, "SDL window could not be created! "
              "SDL Error:", SDL_GetError());
        return {nullptr, nullptr};
    }

    // Create the OpenGL context.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    window.glContext = SDL_GL_CreateContext(window.sdlData);

    if (window.glContext == nullptr) {
        print(INFO_, "The OpenGL context could not be created! "
              "SDL Error:", SDL_GetError());
        return window;
    }

    // Set VSync.
    const bool vsync = true;
    if (!vsync) {
        SDL_GL_SetSwapInterval(0);
        // No vysnc.
    }
    else if (SDL_GL_SetSwapInterval(-1) >= 0) {
        // Adapative vsync
    }
    else if (SDL_GL_SetSwapInterval(1) >= 0) {
        // Normal vsync.
    }
    else {
        // No vsync :o
        print(INFO_, "Unable to set VSync! SDL Error:", SDL_GetError());
        SDL_GL_SetSwapInterval(0);
    }

    return window;
}


void pcs::destroyWindow( Window& window ) {

    // Destroy the render context and the window.
    SDL_GL_DeleteContext(window.glContext);
    SDL_DestroyWindow(window.sdlData);

    window.sdlData = nullptr;
    window.glContext = nullptr;
}
