
#include "lbm.hpp"
#include "../misc.hpp"

using namespace pcs;

LatticeBoltzmann::LatticeBoltzmann( GLRenderer& renderer ) {
    width = 500;
    height = 500;
    frame = 0;

    programId = gl::compileProgram(loadFile("src/shaders/main.vert"),
                                   loadFile("src/lbm/stream.frag"));

    u_textures[0] = glGetUniformLocation(programId, "u_textures");
    u_textures[1] = u_textures[0] + 1;
    u_textures[2] = u_textures[0] + 2;

    // Bind the program.
    renderer.useProgram(programId);

    glUniform1i(u_textures[0], 0);
    glUniform1i(u_textures[1], 1);
    glUniform1i(u_textures[2], 2);

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

    const float modelMatrix[16] = {
        (float) width, 0.f, 0.f, 0.f,
        0.f, (float) height, 0.f, 0,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f,
    };
    renderer.setModelMatrix(modelMatrix);
    renderer.updateViewport(width, height);

    // Render some base.
    renderer.resetProgram();
    renderer.updateViewport(width, height);

    renderer.renderToTexture(buffers[0].texture[0]);
    renderer.clear(0.f, 0.f, 0.f, 1.f);
    renderer.setRenderColor(1.f, 0.f, 0.f, 1.f);
    renderer.renderRectangle(100.f, 100.f, 300.f, 300.f);

    gl::checkErrors("CA init");

    renderer.renderToScreen();

}

void LatticeBoltzmann::close() {
    for (Buffers& buff : buffers) {
        glDeleteTextures(3, buff.texture);
        glDeleteFramebuffers(1, &buff.fbo);
    }

    glDeleteProgram(programId);
    programId = 0;
}

void LatticeBoltzmann::update( GLRenderer& renderer, InputData& input,
                               int windowWidth, int windowHeight ) {

    renderer.useProgram(programId);
    renderer.updateViewport(width, height);

    for (int i = 0; i < 1; ++i) {

        glBindTextures(0, 3, buffers[frame % 2].texture);
        glBindFramebuffer(GL_FRAMEBUFFER, buffers[(frame + 1) % 2].fbo);

        // Bind the textures.
        GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, drawBuffers);

        // Render the model.
        renderer.renderModel(renderer.getSquareModel());

        ++frame;
    }

    renderer.resetProgram();
    renderer.renderToScreen();

    float posX = 10.f, posY = 10.f;
    renderer.setRenderColor(1.f, 1.f, 1.f, 1.f);
    renderer.updateViewport(windowWidth, windowHeight);
    renderer.renderTexture(buffers[frame % 2].texture[0],
                           posX, posY, width, height);
    gl::checkErrors("CA ads");

    posX = 600.f; posY = 600.f;
    renderer.setRenderColor(1.f, 1.f, 1.f, 1.f);
    renderer.renderTexture(buffers[frame % 2].texture[1],
                           posX, posY, width, height);

    posX = 600.f; posY = 10.f;
    renderer.setRenderColor(1.f, 1.f, 1.f, 1.f);
    renderer.renderTexture(buffers[frame % 2].texture[2],
                           posX, posY, width, height);

}

