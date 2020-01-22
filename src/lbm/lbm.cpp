
#include "lbm.hpp"
#include "../misc.hpp"

using namespace pcs;

LatticeBoltzmann::LatticeBoltzmann( GLRenderer& renderer ) {

    backgroundTexture = gl::loadTexture("assets/result3.bmp", &width, &height);

    // width = 1000;
    // height = 500;
    frame = 0;
    paused = false;
    cursorX = cursorY = -1;

    // Create the buffers, storing the flow parameters f_i.
    for (Buffers& buff : buffers) {
        glGenFramebuffers(1, &buff.fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, buff.fbo);
        GLenum drawBuffers[textureCount];

        // Generate and bind the textures.
        for (size_t i = 0; i < textureCount; ++i) {
            buff.texture[i] = gl::genUTexture(width, height);
            drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
            glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[i],
                                   GL_TEXTURE_2D, buff.texture[i], 0);
        }

        // Bind the color attachments.
        glDrawBuffers(textureCount, drawBuffers);
    }

    programs[0] = gl::compileProgram(loadFile("src/opengl/main.vert"),
                                     loadFile("src/lbm/lbm.frag"));
    programs[1] = gl::compileProgram(loadFile("src/opengl/main.vert"),
                                     loadFile("src/lbm/visual.frag"));

    // These uniform locations are defined in the program using layout().
    for (size_t i = 0; i < textureCount; ++i) {
        u_textures[i] = i + 3;
    }

    // Program setup.
    for (GLuint program : programs) {
        renderer.useProgram(program);
        for (size_t i = 0; i < textureCount; ++i) {
            glUniform1i(u_textures[i], i);
        }
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
            renderer.clear(0.f, 0.f, 0.f, 0.f);
        }
    }


    // Initialise the textures / f_i's.

    double f[10] = { 0.0, // <-- filler.
        0.43777778, 0.10944444444444444, 0.14777777777777779,
        0.0811111111111111, 0.10944444444444444, 0.036944444444444446,
        0.020277777777777777, 0.020277777777777777, 0.036944444444444446
    };


    glBindFramebuffer(GL_FRAMEBUFFER, buffers[0].fbo);
    glClearBufferuiv(GL_COLOR, 2, (GLuint*) &f[0]);
    glClearBufferuiv(GL_COLOR, 3, (GLuint*) &f[2]);
    glClearBufferuiv(GL_COLOR, 4, (GLuint*) &f[4]);
    glClearBufferuiv(GL_COLOR, 5, (GLuint*) &f[6]);
    glClearBufferuiv(GL_COLOR, 6, (GLuint*) &f[8]);

    renderer.renderToTexture(buffers[0].texture[0]);
    renderWall(renderer, backgroundTexture, 0, 0, width, height);

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

void LatticeBoltzmann::renderWall( GLRenderer& renderer, GLuint texture, int posX, int posY,
                                   int width, int height ) {

    renderer.renderToTexture(buffers[0].texture[0]);
    renderer.setRenderColor(1.f, 0.f, 0.f, 0.f);
    renderer.renderTexture(texture, posX, posY, width, height);
}

void LatticeBoltzmann::renderFlow( GLRenderer& renderer, GLuint texture, int posX, int posY,
                                   int width, int height ) {

    renderer.renderToTexture(buffers[0].texture[0]);
    renderer.setRenderColor(0.f, 1.f, 0.f, 0.f);
    renderer.renderTexture(texture, posX, posY, width, height);
}



void LatticeBoltzmann::update( GLRenderer& renderer, InputData& input,
                               int windowWidth, int windowHeight ) {

    if (input.keyMap[SDL_SCANCODE_P] == 2) {
        paused = !paused;
    }

    if (input.keyMap[SDL_SCANCODE_S] == 2) {
        renderFlow(renderer, renderer.getBlankTexture(), 1, height/2-100, 1, 200);
    }

    // Add the objects.
    if (input.keyMap[SDL_SCANCODE_O] == 2) {
        // renderer.renderToTexture(buffers[0].texture[0]);
        // renderer.updateViewport(width, height);
        // renderer.setRenderColor(1.f, 0.f, 0.f, 0.f);
        // renderer.renderRectangle(10.f, 0.f, width, height);
        // for (int i = 1; i < 3; i++) {
        //     renderer.renderToTexture(buffers[0].texture[i]);
        //     renderer.setRenderColor(0.f, 0.f, 0.f, 0.f);
        //     renderer.renderRectangle(10.f, 0.f, width, height);
        //     // renderer.renderRectangle(100.f, 280.f, 300.f, 100.f);
        // }

        // renderer.renderToTexture(buffers[0].texture[0]);
        // renderer.setRenderColor(0.f, 0.f, 0.f, 0.43777778f);
        // renderer.renderRectangle(10.f, 200.f, width-800.f, 100.f);

        renderer.setRenderColor(1.0f, 0.0f, 0.0f, 0.43777778f);
        renderWall(renderer, backgroundTexture, 0, 0, width, height);
        // renderWall(renderer, 100, 280, 300, 100);
    }


    if (!paused|| input.keyMap[SDL_SCANCODE_F] == 2) {

        renderer.useProgram(programs[0]);
        renderer.updateViewport(width, height);
        renderer.setModelMatrix(0.f, 0.f, width, height);

        for (unsigned i = 0; i < 10; ++i) {

            readPixels(renderer, input);

            // Bind the textures from which we render, and bind to
            // framebuffer to which we rander.
            glBindTextures(0, 7, buffers[frame % 2].texture);
            glBindFramebuffer(GL_FRAMEBUFFER, buffers[(frame + 1) % 2].fbo);

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
        renderer.setModelMatrix(0.f, 0.f, width, height);
        renderer.renderModel(renderer.getSquareModel());

        // renderer.setModelMatrix(width, 0.f, width, height);
        // renderer.renderModel(renderer.getSquareModel());
        // renderer.setModelMatrix(0.f, height, width, height);
        // renderer.renderModel(renderer.getSquareModel());

        renderer.resetProgram();
    }
    gl::checkErrors("CA end");
}


void LatticeBoltzmann::readPixels( GLRenderer& renderer, InputData& input ) {
    // Some debug stuff, please ignore.

    if (input.keyMap[SDL_BUTTON_RIGHT] == 2) {
        cursorX = cursorY = -1;
    }

    if (input.keyMap[SDL_BUTTON_LEFT] == 1) {
        cursorX = input.cursorX;
        cursorY = input.cursorY;
    }

    // print(cursorX, cursorY, input.cursorX, input.cursorY);

    // if (cursorX >= 0 && cursorX < width &&
    //     cursorY >= 0 && cursorY < height) {

    //     const Buffers& buf = buffers[frame % 2];

    //     glBindTexture(GL_TEXTURE_2D, buf.texture[1]);
    //     float pixel[4];
    //     glReadPixels(cursorX, cursorY, 1, 1, GL_RGBA, GL_FLOAT, &pixel);
    //     print(pixel[0], pixel[1], pixel[2], pixel[3]);
    // }

    // glBindTexture(GL_TEXTURE_2D, 0);

    //static float* img = new float[width*height*4];
    //glBindTexture(GL_TEXTURE_2D, buffers[frame % 2].texture[1]);
    //glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, (GLvoid*) img);
    //glBindTexture(GL_TEXTURE_2D, 0);

    // float min = 123456.f, max = 0.f;
    // for (size_t i = 0; i < 1000*500*4; i += 4) {
    //     min = img[i] < min ? img[i] : min;
    //     max = img[i] > max ? img[i] : max;
    //     if (img[i] <= 0.f) {
    //         int j = i - i % 4;
    //         //print(j, img[j], img[j+1], img[j+2], img[j+3]);
    //     }
    // }
    // print(min, max);
    // if (min < 0.f) {
    //     paused = true;
    //     break;
    // }
}