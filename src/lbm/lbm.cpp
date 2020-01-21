
#include "lbm.hpp"
#include "../misc.hpp"

using namespace pcs;

LatticeBoltzmann::LatticeBoltzmann( GLRenderer& renderer ) {
    width = 1000;
    height = 500;
    frame = 0;

    // Create the buffers, storing the flow parameters f_i.
    for (Buffers& buff : buffers) {
        glGenFramebuffers(1, &buff.fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, buff.fbo);

        // Generate and bind the textures.
        int i = 0;
        for (GLuint& tex : buff.texture)  {
            tex = gl::genTexture(width, height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                   GL_TEXTURE_2D, tex, 0);
            ++i;
        }
    }

    programs[0] = gl::compileProgram(loadFile("src/opengl/main.vert"),
                                     loadFile("src/lbm/lbm.frag"));
    programs[1] = gl::compileProgram(loadFile("src/opengl/main.vert"),
                                     loadFile("src/lbm/visual.frag"));

    // These uniform locations are defined in the program using layout().
    u_textures[0] = 3;
    u_textures[1] = 4;
    u_textures[2] = 5;

    // Program setup.
    for (GLuint& program : programs) {
        renderer.useProgram(program);
        glUniform1i(u_textures[0], 0);
        glUniform1i(u_textures[1], 1);
        glUniform1i(u_textures[2], 2);

        renderer.setModelMatrix(0.f, 0.f, width, height);
        renderer.updateViewport(width, height);
    }

    // Render some base.
    renderer.resetProgram();
    renderer.updateViewport(width, height);

    // Clear the textures.
    for (Buffers& buff : buffers) {
        for (GLuint& tex : buff.texture)  {
            renderer.renderToTexture(tex);
            renderer.clear(0.f, 0.f, 0.f, 1.f);
        }
    }

    // Initialise the textures / f_i's.
    renderer.renderToTexture(buffers[0].texture[0]);
    renderer.clear(0.0f, 0.0f, 0.0f, 0.43777778f);

    renderer.renderToTexture(buffers[0].texture[1]);
    renderer.clear(0.10944444444444444, 0.14777777777777779,
                   0.0811111111111111, 0.10944444444444444);

    renderer.renderToTexture(buffers[0].texture[2]);
    renderer.clear(0.036944444444444446, 0.020277777777777777,
                   0.020277777777777777, 0.036944444444444446);


    renderer.renderToScreen();

    gl::checkErrors("CA init");
}

void LatticeBoltzmann::close() {
    for (Buffers& buff : buffers) {
        glDeleteTextures(3, buff.texture);
        glDeleteFramebuffers(1, &buff.fbo);
    }

    for (GLuint program : programs) {
        glDeleteProgram(program);
    }
}

void LatticeBoltzmann::update( GLRenderer& renderer, InputData& input,
                               int windowWidth, int windowHeight ) {

    static bool paused = false;
    if (input.keyMap[SDL_SCANCODE_P] == 2) {
        paused = !paused;
    }


    // Add the objects.
    if (input.keyMap[SDL_SCANCODE_O] == 2) {
        renderer.renderToTexture(buffers[0].texture[0]);
        renderer.updateViewport(width, height);
        renderer.setRenderColor(1.f, 0.f, 0.f, 0.f);
        renderer.renderRectangle(10.f, 0.f, width, height);
        for (int i = 1; i < 3; i++) {
            renderer.renderToTexture(buffers[0].texture[i]);
            renderer.setRenderColor(0.f, 0.f, 0.f, 0.f);
            renderer.renderRectangle(10.f, 0.f, width, height);
            // renderer.renderRectangle(100.f, 280.f, 300.f, 100.f);
        }

        renderer.renderToTexture(buffers[0].texture[0]);
        renderer.setRenderColor(0.f, 0.f, 0.f, 0.43777778f);
        renderer.renderRectangle(10.f, 200.f, width-800.f, 100.f);

        // renderer.setRenderColor(1.f, 0.f, 0.f, 0.0);
        // // renderer.setRenderColor(1.f, 0.f, 0.f, 0.43777778f);
        // renderer.renderRectangle(100.f, 280.f, 300.f, 100.f);
        // renderer.renderToTexture(buffers[0].texture[1]);
        // renderer.setRenderColor(0.f, 0.f, 0.f, 0.0);
        // //renderer.setRenderColor(0.10944444444444444, 0.14777777777777779,
        //             //   0.0811111111111111, 0.10944444444444444);
        // renderer.renderRectangle(100.f, 280.f, 300.f, 100.f);

        // renderer.renderToTexture(buffers[0].texture[2]);
        // // renderer.setRenderColor(0.036944444444444446, 0.020277777777777777,
        //             // 0.020277777777777777, 0.036944444444444446);
        // renderer.renderRectangle(100.f, 280.f, 300.f, 100.f);
    }


    if (!paused|| input.keyMap[SDL_SCANCODE_F] == 2) {

        renderer.useProgram(programs[0]);
        renderer.updateViewport(width, height);
        renderer.setModelMatrix(0.f, 0.f, width, height);

        for (unsigned i = 0; i < 20; ++i) {

            // // Some debug stuff, please ignore.
            // float img[1000*500*4];
            // glBindTexture(GL_TEXTURE_2D, buffers[frame % 2].texture[0]);
            // glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, (GLvoid*) img);
            // glBindTexture(GL_TEXTURE_2D, 0);

            // float min = 123456.f, max = 0.f;
            // for (size_t i = 0; i < 1000*500*4; i++) {
            //     min = img[i] < min ? img[i] : min;
            //     max = img[i] > max ? img[i] : max;
            //     if (img[i] <= 0.f) {
            //         int j = i - i % 4;
            //         //print(j, img[j], img[j+1], img[j+2], img[j+3]);
            //     }
            // }
            // print(img[0], img[1], img[2], img[3]);
            // if (min < 0.f) {
            //     print(min, max);
            //     paused = true;
            //     break;
            // }

            // Bind the textures from which we render, and bind to
            // framebuffer to which we rander.
            glBindTextures(0, 3, buffers[frame % 2].texture);
            glBindFramebuffer(GL_FRAMEBUFFER, buffers[(frame + 1) % 2].fbo);

            // Bind the color attachments.
            GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                    GL_COLOR_ATTACHMENT2};
            glDrawBuffers(3, drawBuffers);

            // Render the model.
            renderer.renderModel(renderer.getSquareModel());

            ++frame;

        }

        // Render the results.
        renderer.resetProgram();
    }



    renderer.renderToScreen();

    bool debugRender = true;
    if (debugRender) {
        float posX = 10.f, posY = 600.f;
        renderer.setRenderColor(1.f, 1.f, 1.f, 1.f);
        renderer.updateViewport(windowWidth, windowHeight);
        renderer.renderTexture(buffers[frame % 2].texture[0],
                            posX, posY, width, height);
        gl::checkErrors("CA ads");

        posX = 1100.f; posY = 10.f;
        renderer.setRenderColor(1.f, 1.f, 1.f, 1.f);
        renderer.renderTexture(buffers[frame % 2].texture[1],
                            posX, posY, width, height);

        posX = 1100.f; posY = 600.f;
        renderer.setRenderColor(1.f, 1.f, 1.f, 1.f);
        renderer.renderTexture(buffers[frame % 2].texture[2],
                            posX, posY, width, height);

    }

    if (true) {
        // Visualise the output
        renderer.useProgram(programs[1]);
        renderer.updateViewport(windowWidth, windowHeight);
        renderer.setRenderColor(1.f, 1.f, 1.f, 1.f);
        glBindTextures(0, 3, buffers[frame % 2].texture);
        renderer.setModelMatrix(10.f, 10.f, width, height);
        renderer.renderModel(renderer.getSquareModel());

        // renderer.setModelMatrix(10.f+width, 10.f, width, height);
        // renderer.renderModel(renderer.getSquareModel());
        // renderer.setModelMatrix(10.f, 10.f+height, width, height);
        // renderer.renderModel(renderer.getSquareModel());

        renderer.resetProgram();
    }
    gl::checkErrors("CA end");
}
