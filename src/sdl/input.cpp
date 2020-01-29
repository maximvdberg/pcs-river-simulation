/**
 * Implementation of some function for handling SDL input.
 *
 * @file input.cpp
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 08-01-2020
 */

#include "input.hpp"


void pcs::updateInput( Window& window, InputData& data ) {

    // Update the keypresses from just pressed to being held down.
    for (const std::pair<int, char>& v : data.keyMap) {
        data.keyMap[v.first] = v.second == 0 ? 0 : 1;
    }

    // Poll and handle all the pending SDL input events.
    SDL_Event e;
    while (SDL_PollEvent(&e)){

        switch (e.type) {
        case SDL_QUIT:
            data.quit = true;
            break;

        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                window.sizeChanged = true;
                SDL_GL_GetDrawableSize(window.sdlData,
                                       &window.width,
                                       &window.height);
            }
            break;

        case SDL_KEYDOWN:
            data.keyMap[e.key.keysym.scancode] = 2;
            break;

        case SDL_KEYUP:
            data.keyMap[e.key.keysym.scancode] = 0;
            break;

        case SDL_MOUSEMOTION:
            data.cursorX = e.button.x;
            data.cursorY = window.height - e.button.y;
            break;

        case SDL_MOUSEBUTTONDOWN:
            data.keyMap[e.button.button] = 2;
            break;

        case SDL_MOUSEBUTTONUP:
            data.keyMap[e.button.button] = 0;
            break;

        default:
            break;
        }
    }
}
