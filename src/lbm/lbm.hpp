#pragma once

#include "../opengl/opengl.hpp"
#include "../sdl/input.hpp"

namespace pcs {


    struct LatticeBoltzmann {
        /**
         */
        struct Buffers {
            GLuint texture[3];
            GLuint fbo;
        };

        LatticeBoltzmann( GLRenderer& renderer );

        void close();

        void update( GLRenderer& renderer, InputData& input,
                     int width, int height );

        int width, height;

        GLuint programs[2]; // {<stream>, <collide>, <boundary>, <visual>}
        GLuint boundaryProgram;

        GLuint u_textures[3];

        Buffers buffers[2];

        unsigned frame;
    };
}