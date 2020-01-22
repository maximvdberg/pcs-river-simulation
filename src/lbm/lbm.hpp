#pragma once

#include "../opengl/opengl.hpp"
#include "../sdl/input.hpp"

namespace pcs {


    struct LatticeBoltzmann {
        /**
         */
        struct Buffers {
            GLuint texture[4];
            GLuint fbo;
        };

        LatticeBoltzmann( GLRenderer& renderer );

        void close();

        void renderWall( GLRenderer& renderer, GLuint texture,
                         int posX, int posY,
                         int width, int height );

        void renderFlow( GLRenderer& renderer, GLuint texture,
                         int posX, int posY,
                         int width, int height );


        void update( GLRenderer& renderer, InputData& input,
                     int width, int height );

        int width, height;

        GLuint programs[2]; // {<stream>, <collide>, <boundary>, <visual>}
        GLuint boundaryProgram;

        GLuint u_textures[4]; // Uniform texture locations

        Buffers buffers[2];

        GLuint backgroundTexture;

        unsigned frame;
    };
}