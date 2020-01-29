/**
 * Model setup and OpenGL communication.
 *
 * @file lbm.cpp
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 15-01-2020
 */

#pragma once

#include "../opengl/opengl.hpp"
#include "../sdl/input.hpp"

namespace pcs {


    /**
     * The LatticeBoltzmann class contains all logic for handling communication
     * between the SDL and OpenGL instances concerning the LBM model, such as
     * initialisation, input handling and data extraction. All computations are
     * done within the OpenGL instance.
     *
     * The actual implementation of the (modified) LBM can be found in
     * `lbm.frag`, and for more information this file should be referenced.
     */
    class LatticeBoltzmann {

        // Total textures used to store all LBM data on the GPU. See below.
        static constexpr size_t textureCount = 7;

        /**
         * A wrapper for the OpenGL textures which contain all relevant data
         * for the LBM implementation. Note that to facilitate double precision
         * within the exclusively 32 bit textures, two 32 integers are used
         * instead. Additionally, the OpenGL Frame Buffer Object (fbo) is
         * coupled as well.
         *
         * The textures are mapped as follows:
         * +----------+---------------+---------------+
         * | Texture  | Mapped to     | input/output  |
         * +----------+---------------+---------------+
         * | 0.r        Walls temp      In            |
         * | 0.g        Indestructible  In/out        |
         * | 0.b        Flow source     In/out        |
         * | 0.a        Walls           In/out        |
         * | 1.rg       u flow x        Out           |
         * | 1.ba       u flow y        Out           |
         * | 2.rg       rho             Out           |
         * | 2.ba       f0 (0)          In/out        |
         * | 3.rg       f1 (E)          In/out        |
         * | 3.ba       f2 (N)          In/out        |
         * | 4.rg       f3 (W)          In/out        |
         * | 4.ba       f4 (S)          In/out        |
         * | 5.rg       f5 (NE)         In/out        |
         * | 5.ba       f6 (NW)         In/out        |
         * | 6.rg       f7 (SW)         In/out        |
         * | 6.ba       f8 (SE)         In/out        |
         * +------------------------------------------+
         */
        struct Buffers {
            GLuint texture[textureCount];
            GLuint fbo;
        };


    public:

        /**
         * The constructor loads the specified river bitmap and initialises the
         * two `Buffer` structs (one to render from and one to render to)
         * according to the specified bitmap. Afterwards, it compiles the OpenGL
         * shaders `lbm.frag` (for all computations) and `visual.frag` (for
         * rendering) and performs the rendering setup.
         */
        LatticeBoltzmann( GLRenderer& renderer, const std::string& riverFile );

        /**
         * Deconstruct the `Buffer` structs and OpenGL programs>
         */
        void close();

        /**
         * The update loop of the simulation. It advances the simulation with
         * `framestep` frames, after which it renders the current system state
         * to the screen. Additionally, it renders the cursor and current
         * settings.
         *
         * Calls the `handleInput` and `readPixels` functions.
         */
        void update( GLRenderer& renderer, InputData& input,
                     int width, int height );

    private:

        /**
         * Redirect user input to the model. For a list of the inputs handled
         * within this function, see the first keymap list within the
         * `README.md` file.
         */
        void handleInput( GLRenderer& renderer, InputData& input );

        /**
         * Data extraction according to user input. Handles the "Sherlock"
         * pointer which the user can use to extract numerical data from the
         * simulation. For more information, see the last keymap list within
         * the `README.md` file.
         */
        void readPixels( GLRenderer& renderer, InputData& input );


        // Dimensions of the river texture.
        int width, height;

        // OpenGL references
        GLuint programs[2]; // Contains the lbm and visual fragment shaders.
        GLuint u_textures[textureCount]; // The uniform texture locations.
        GLuint backgroundTexture;

        // Flags for flow settings. Contains (in order)
        // [enable flow, enable corrosion, enable sedimentation].
        GLuint u_settings;


        // Buffers objects as described above. One
        // to render from and one to render to.
        Buffers buffers[2];

        // Frames to process every update loop, and the frame counter.
        unsigned framestep;
        unsigned frame;

        // Settings, as described above.
        bool paused;
        bool runFrame;
        bool settings[4];

        // Viewport and cursor positions.
        float screenX, screenY, screenScale;
        int cursorX, cursorY;
    };
}
