/**
 * Here functions are implemented to handle SDL input.
 *
 * @file input.cpp
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 08-01-2020
 */

#include "input.hpp"


void pcs::updateInput( InputData& data ) {

    // Update the keypresses.
    for (const std::pair<SDL_Scancode, char>& v : data.keyMap) {
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
                data.windowSizeChanged = true;
            }
            break;

        case SDL_KEYDOWN:
            data.keyMap[e.key.keysym.scancode] = 2;
            break;

        case SDL_KEYUP:
            data.keyMap[e.key.keysym.scancode] = 0;
            break;

        default:
            break;
        }
    }
}
