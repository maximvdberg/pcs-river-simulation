/**
 * Here functions are implemented to handle SDL input.
 *
 * @file input.hpp
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 08-01-2020
 */

#pragma once

#include <map>
#include <SDL2/SDL.h>


namespace pcs {

    /**
     * And input data holder struct. This is updated with updateInput() below.
     *
     * @see pcs::updateInput()
     */
    struct InputData {
        bool windowSizeChanged = true;
        bool quit = false;

        // And keyboard input map, where the value corresponding to a key key
        // is 2 if the button was pressed this frame, 1 if the button is held
        // down, and 0 otherwise.
        std::map<SDL_Scancode, char> keyMap;
    };

    /**
     * Update the input data by polling al the pending SDL events.
     * @see pcs::InputData
     * @param intputData The input data to update.
     */
    void updateInput( InputData& inputData );
}
