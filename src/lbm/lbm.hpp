#pragma once

#include "../opengl/opengl.hpp"
#include "../sdl/input.hpp"

namespace pcs {


    struct LatticeBoltzmann {

        static constexpr size_t textureCount = 7;

        /**
         */
        struct Buffers {
            GLuint texture[textureCount];
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

        void readPixels( GLRenderer& renderer, InputData& input );

        int width, height;

        GLuint programs[2]; // {<stream>, <collide>, <boundary>, <visual>}
        GLuint boundaryProgram;

        GLuint u_textures[textureCount]; // Uniform texture locations

        Buffers buffers[2];

        GLuint backgroundTexture;

        unsigned frame;
        bool paused;

        int cursorX, cursorY;
    };
}