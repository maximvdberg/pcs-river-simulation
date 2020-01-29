/**
 * Model setup and OpenGL communication.
 * See lbm.hpp for details.
 *
 * @file lbm.cpp
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 15-01-2020
 */

#include "lbm.hpp"

#include "../print.hpp"

using namespace pcs;


// Flow constants.
static const double e_x[9] = {0., 1.,  0., -1., 0.,  1., -1., -1., 1.};
static const double e_y[9] = {0., 0., 1., 0., -1., 1., 1.,  -1., -1.};
static const double w[9] = {4./9., 1./9., 1./9., 1./9., 1./9.,
                            1./36., 1./36., 1./36., 1./36.};

// System parameters.
static const double delta_x = 1.0;  // Lattice spacing
static const double delta_t = 1.0;  // Time step
static const double c = delta_x / delta_t;

// Starting values and functions.
static const double rho0 = 1.0;  // The initial rho (density).
static const double u0_x = 0.0;  // The initial x velocity.
static const double u0_y = 0.0;  // The initial y velocity.

static double calc_feq( int i ) {
    const double udotu = u0_x * u0_x + u0_y * u0_y;
    double edotu_c = 3.0 * (e_x[i] * u0_x + e_y[i] * u0_y) / c;
    return w[i] * rho0 * (1 + edotu_c + edotu_c*edotu_c / 2.0 -
                          1.5 * udotu / (c * c));
}


LatticeBoltzmann::LatticeBoltzmann( GLRenderer& renderer, const std::string& riverFile ) {

    // Load the background texture file, which is the river configuration.
    backgroundTexture = gl::loadTexture(riverFile, &width, &height);

    // Set frame variables
    framestep = 10;      // Amount of simulation frames between rendering
    frame = 0;           // Frame counter
    paused = false;

    settings[0] = true;  // enable flow
    settings[1] = false; // enable corrosion
    settings[2] = false; // enable sedimentation
    settings[3] = false; // enable slope

    // Initialise the camera position.
    screenX = screenY = 0.f;
    screenScale = 1.f;
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

    programs[0] = gl::compileProgram(readFile("src/opengl/main.vert"),
                                     readFile("src/lbm/lbm.frag"));
    programs[1] = gl::compileProgram(readFile("src/opengl/main.vert"),
                                     readFile("src/lbm/visual.frag"));

    // These uniform locations are defined in the program using layout().
    for (size_t i = 0; i < textureCount; ++i) {
        u_textures[i] = i + 3;
    }
    u_settings = u_textures[textureCount - 1] + 1;

    // Program setup.
    for (GLuint program : programs) {
        renderer.useProgram(program);

        for (size_t i = 0; i < textureCount; ++i) {
            glUniform1i(u_textures[i], i);
        }

        renderer.setModelMatrix(0.f, 0.f, width, height);
        renderer.updateViewport(width, height);
    }

    // Rendering setup.
    renderer.resetProgram();
    renderer.updateViewport(width, height);

    // Clear the textures.
    for (Buffers& buff : buffers) {
        for (GLuint& tex : buff.texture)  {
            renderer.renderToTexture(tex);
            renderer.clear(0.f, 0.f, 0.f, 0.f);
        }
    }

    // Initialise the textures and the  f_i values.
    double f_eq[10] = { 0.0 }; // <-- filler
    for (int i = 0; i < 9; i++) {
        f_eq[i+1] = calc_feq(i);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, buffers[0].fbo);
    for (uint i = 0; i < 5; ++i) {
        glClearBufferuiv(GL_COLOR, i + 2, (GLuint*) &f_eq[i*2]);
    }

    renderer.renderToTexture(buffers[0].texture[0]);
    renderer.setRenderColor(1.0f, 1.0f, 1.0f, 0.0f);
    renderer.renderTexture(backgroundTexture, 0, 0, width, height);

    renderer.renderToScreen();

    gl::checkErrors("LBM initialise");
}

void LatticeBoltzmann::close() {

    for (Buffers& buff : buffers) {
        glDeleteTextures(textureCount, buff.texture);
        glDeleteFramebuffers(1, &buff.fbo);
    }

    for (GLuint program : programs) {
        glDeleteProgram(program);
    }
}

void LatticeBoltzmann::handleInput( GLRenderer& renderer, InputData& input ) {

    float camSpeed = 10.f;
    if (input.keyMap[SDL_SCANCODE_LEFT]) screenX += camSpeed;
    if (input.keyMap[SDL_SCANCODE_RIGHT]) screenX -= camSpeed;
    if (input.keyMap[SDL_SCANCODE_DOWN]) screenY += camSpeed;
    if (input.keyMap[SDL_SCANCODE_UP]) screenY -= camSpeed;

    // Set the screen scale.
    for (int i = 0; i < 9; ++i) {
        if (input.keyMap[SDL_SCANCODE_1 + i]) {
            screenScale = (float) i + 1.f;
        }
    }

    // Reset the screen position and scale.
    if (input.keyMap[SDL_SCANCODE_ESCAPE] == 2) {
        screenX = screenY = 0.0f;
        screenScale = 1.f;
    }

    // Pause and unpause the flow.
    if (input.keyMap[SDL_SCANCODE_P] == 2) {
        paused = !paused;
    }

    // Frame advance if paused.
    runFrame = false;
    if (input.keyMap[SDL_SCANCODE_F] == 2) {
        runFrame = true;
    }

    // Rerender the background, to restore starting walls.
    if (input.keyMap[SDL_SCANCODE_O] == 2) {
        renderer.renderToTexture(buffers[0].texture[0]);
        renderer.setRenderColor(1.0f, 1.0f, 1.0f, 0.0f);
        renderer.renderTexture(backgroundTexture, 0, 0, width, height);
    }

    // Update flow settings
    int settingsCodes[4] = {SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R};
    for (int i = 0; i < 4; ++i) {
        if (input.keyMap[settingsCodes[i]] == 2)
            settings[i] = !settings[i];
    }

    // Enable both corrosion and sedimentation.
    if (input.keyMap[SDL_SCANCODE_T] == 2) {
        settings[1] = settings[2] = true;
    }
}

void LatticeBoltzmann::update( GLRenderer& renderer, InputData& input,
                               int windowWidth, int windowHeight ) {

    handleInput(renderer, input);

    if (!paused || runFrame) {

        renderer.useProgram(programs[0]);
        renderer.updateViewport(width, height);
        renderer.setModelMatrix(0.f, 0.f, width, height);

        glUniform4i(u_settings, settings[0], settings[1], settings[2], settings[3]);

        // Run for `framestep` amount of frames.
        for (unsigned i = 0; i < framestep; ++i) {

            // Bind the textures from which we render, and bind to
            // framebuffer to which we render.
            glBindTextures(0, 7, buffers[frame % 2].texture);
            glBindFramebuffer(GL_FRAMEBUFFER, buffers[(frame + 1) % 2].fbo);

            // Render the model.
            renderer.renderModel(renderer.getSquareModel());

            ++frame;
        }

        // Render the results.
        renderer.resetProgram();
    }

    readPixels(renderer, input);

    renderer.renderToScreen();
    renderer.updateViewport(windowWidth, windowHeight);

    // Render the main simulation viewport.
    renderer.useProgram(programs[1]);
    renderer.updateViewport(windowWidth, windowHeight);

    renderer.setRenderColor(1.f, 1.f, 1.f, 1.f);
    glBindTextures(0, 3, buffers[frame % 2].texture);
    renderer.setModelMatrix(screenX * screenScale, screenY * screenScale,
                            width * screenScale, height * screenScale);

    renderer.renderModel(renderer.getSquareModel());
    renderer.resetProgram();

    // Render the cursor.
    if (cursorX >= 0 && cursorX < width &&
        cursorY >= 0 && cursorY < height) {

        const int size = 8;
        renderer.setRenderColor(1.0, 0.0, 0.0);
        renderer.renderRectangle(screenX + cursorX * screenScale,
                                 screenY + cursorY * screenScale-size/2,
                                 1.f, size);
        renderer.renderRectangle(screenX + cursorX * screenScale - size/2,
                                 screenY + cursorY * screenScale,
                                 size, 1.f);
    }

    // Render the settings.
    constexpr int size = 10;
    renderer.setRenderColor(0.0, 0.0, 1.0);
    for (int i = 0; i < 4; i++) {
        if (settings[i]) {
            renderer.setRenderColor(i % 2, i % 3, (i+1) % 2);
            renderer.renderRectangle(size*i, 0, size, size);
        }
    }

    gl::checkErrors("LBM end of update");
}



void LatticeBoltzmann::readPixels( GLRenderer& renderer, InputData& input ) {

    bool posChanged = false;

    // Remove pointer.
    if (input.keyMap[SDL_BUTTON_RIGHT] == 2) {
        cursorX = cursorY = -1;
    }

    // Place pointer.
    if (input.keyMap[SDL_BUTTON_LEFT] == 1) {
        cursorX = (input.cursorX - screenX) / screenScale;
        cursorY = (input.cursorY - screenY)  / screenScale;
        posChanged = true;
    }

    // Gather the information at the pointer's position.
    if (cursorX >= 0 && cursorX < width &&
        cursorY >= 0 && cursorY < height &&
        (input.keyMap[SDL_SCANCODE_V] > 0 || posChanged)) {

        const Buffers& buf = buffers[frame % 2];
        glBindFramebuffer(GL_FRAMEBUFFER, buf.fbo);

        // Get the tile information (flags for source, walls, etc.).
        unsigned data[4];
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glReadPixels(cursorX, cursorY, 1, 1, GL_RGBA_INTEGER,
                     GL_UNSIGNED_INT, &data);

         // Get the values for u_x, u_y, rho and all f_i's
        double vals[12];
        for (int i = 0; i < textureCount - 1; ++i) {
            glReadBuffer(GL_COLOR_ATTACHMENT1 + i);
            glReadPixels(cursorX, cursorY, 1, 1, GL_RGBA_INTEGER,
                         GL_UNSIGNED_INT, &vals[i*2]);
        }

        int dw = 12;

        // Begin outputting the information to the terminal.
        std::cout << "------- Sherlock Data --------" << std::endl;
        std::cout << "location: " << std::setw(5) << toString(cursorX)
                  << ", " << std::setw(5) << toString(cursorY) << std::endl;

        // Output tile data.
        std::cout << "data: ";
        for (unsigned u : data) std::cout << std::setw(dw) << toString(u);
        std::cout << std::endl;

        // Output u and rho.
        std::cout << "u   = (" << std::setw(dw) << toString(vals[0])
                  << ", " << std::setw(dw) << toString(vals[1])
                  << ")" << std::endl;
        std::cout << "|u| =  " << std::setw(dw)
                  << toString(std::sqrt(vals[0]*vals[0] +
                                        vals[1]*vals[1])) << std::endl;
        std::cout << "rho =  " << std::setw(dw) << toString(vals[2])
                  << std::endl;

        // Output the f values.
        std::cout << "f values:";
        int j = 0;
        for (int i : {6,2,5,3,0,1,7,4,8}) {
            if (j++ % 3 == 0) std::cout << std::endl;
            std::cout << std::setw(dw) << toString(vals[i + 3]);
        }

        std::cout << std::endl << std::endl << std::flush;

        renderer.renderToScreen();
    }


    // Display the rightward momentum of all non-wall
    // tiles in the vertical line through the cursor.
    if (input.keyMap[SDL_SCANCODE_X] == 2) {
        const Buffers& buf = buffers[frame % 2];
        glBindFramebuffer(GL_FRAMEBUFFER, buf.fbo);

        unsigned data[4];
        double u[2];

        std::cout << "[";
        for (int y = 0; y < height; y++) {

            // Get the tile data (to find walls).
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glReadPixels(cursorX, y, 1, 1, GL_RGBA_INTEGER,
                         GL_UNSIGNED_INT, &data);

            // Get the data for u.
            glReadBuffer(GL_COLOR_ATTACHMENT1);
            glReadPixels(cursorX, y, 1, 1, GL_RGBA_INTEGER,
                         GL_UNSIGNED_INT, &u[0]);

            // Display the x component of u.
            if (data[3] == 0) {
                std::cout << toString(u[0]) << ", ";
            }
        }

        std::cout << "]" << std::endl << std::endl;

        // Bind back to the screen framebuffer.
        renderer.renderToScreen();
    }

    // Display the upward momentum of all non-wall
    // tiles in the horizontal line through the cursor.
    if (input.keyMap[SDL_SCANCODE_Y] == 2) {
        const Buffers& buf = buffers[frame % 2];
        glBindFramebuffer(GL_FRAMEBUFFER, buf.fbo);

        unsigned data[4];
        double u[2];

        std::cout << "[";
        for (int x = 0; x < width; x++) {

            // Get the tile data (to find walls).
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glReadPixels(x, cursorY, 1, 1, GL_RGBA_INTEGER,
                         GL_UNSIGNED_INT, &data);

            // Get the data for u.
            glReadBuffer(GL_COLOR_ATTACHMENT1);
            glReadPixels(x, cursorY, 1, 1, GL_RGBA_INTEGER,
                         GL_UNSIGNED_INT, &u[0]);

            // Display the y component of u.
            if (data[3] == 0) {
                std::cout << toString(u[1]) << ", ";
            }
        }

        std::cout << "]" << std::endl << std::endl;

        // Bind back to the screen framebuffer.
        renderer.renderToScreen();
    }
}
