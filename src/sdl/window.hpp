/**
 * Here functions are supplied to create and destroy a SDL windows.
 *
 * @file window.hpp
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 08-01-2020
 */

#pragma once

#include <string>
#include <SDL2/SDL.h>


namespace pcs {

    /**
     * A window object data holder, which holds the SDL_Window data and the
     * OpenGL context created with the window. Beside this the width and
     * height of the window are also stored, but must be updated manually.
     * @see pcs::createOpenGLWindow()
     */
    struct Window {
        SDL_Window* sdlData;
        void* glContext;

        int width, height;
        bool sizeChanged;
    };

    /**
     * Create a SDL window along with an OpenGL context bound to this window.
     * This functions also initialises SDL, so this needs not be done
     * independently. A title, width and height of the window can be specified.
     * Windows should be destroyed using the destroyWindow() function below.
     *
     * @see pcs::Window
     * @see pcs::destroyWindow()
     *
     * @param title The title of the window.
     * @param width The width of the window.
     * @param height The height of the window.
     * @return A Window object storing the data of the created window.
     */
    Window createOpenGLWindow( const std::string& title, int width = 400, int height = 400 );

    /**
     * Destroy and free a window.
     *
     * @see pcs::createOpenGLWindow()
     * @see pcs::Window
     *
     * @param window The window to destroy.
     */
    void destroyWindow( Window& window );
}
