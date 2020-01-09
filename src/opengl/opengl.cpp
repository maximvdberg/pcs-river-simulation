/**
 * Renderer function implementations using OpenGl. Function
 * documentation can be found in opengl.hpp.
 *
 * @file opengl.cpp
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 08-01-2020
 */

#include "opengl.hpp"

#include <string>
#include <fstream>
#include "../print.hpp"

using namespace pcs;


std::string loadFile( const std::string& path ) {
    std::ifstream file(path);
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    else {
        print(INFO_, "file", path, "does not exists!");
        return "";
    }
}

GLRenderer::GLRenderer() {

    // Compile the rendering program.
    program = gl::compileProgram(loadFile("src/shaders/vertex.shader"),
                                 loadFile("src/shaders/main_fragment.shader"));

    // Bind the attribute and uniform locations of the program, so
    // that we can edit them from C++.
    i_position = glGetAttribLocation(program, "i_position");
    i_tex_coords = glGetAttribLocation(program, "i_tex_coords");
    u_projection_matrix = glGetUniformLocation(program, "u_projection_matrix");
    u_model_matrix = glGetUniformLocation(program, "u_model_matrix");
    u_color = glGetUniformLocation(program, "u_color");
    u_texture = glGetUniformLocation(program, "u_texture");

    // Get the window frame buffer object, used for rendering to the window.
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*) &windowFBO);

    // Create a multipurpose fbo, used for rendering to textures.
    glGenFramebuffers(1, &multipurposeFBO);

    // Generate a 1x1 blank texture, used for rendering texturesless rects.
    blankTexture = gl::genTexture(1, 1, (uint8_t[]) {0xFF, 0xFF, 0xFF, 0xFF});

    // Create a square model used for rendering simple textures.
    glGenVertexArrays(1, &squareModel.vao);
    glBindVertexArray(squareModel.vao);
    constexpr size_t fSize = sizeof (GLfloat);
    GLfloat vertices[20] = {0.f, 0.f, 0.0f, 0.f, 0.f,
                            1.f, 0.f, 0.0f, 1.f, 0.f,
                            1.f, 1.f, 0.0f, 1.f, 1.f,
                            0.f, 1.f, 0.0f, 0.f, 1.f};
    glGenBuffers(1, &squareModel.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, squareModel.vbo);
    glBufferData(GL_ARRAY_BUFFER, 20 * fSize, vertices, GL_STATIC_DRAW);

    // Enable the attributes and map their locations to the array above.
    glEnableVertexAttribArray(i_position);
    glEnableVertexAttribArray(i_tex_coords);
    glVertexAttribPointer(i_position, 3, GL_FLOAT, GL_FALSE,
                          (GLsizei) (5 * fSize), 0);
    glVertexAttribPointer(i_tex_coords, 2, GL_FLOAT, GL_FALSE,
                          (GLsizei) (5 * fSize),
                          (void*) (3 * fSize));

    // Generate the index buffer object, which specifies
    // which vertices to draw.
    glGenBuffers(1, &squareModel.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareModel.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof (GLint),
                 (int[]) {0, 1, 2, 0, 2, 3}, GL_STATIC_DRAW);

    // Set the draw mode and the vertex count, and we can unbind the vao.
    squareModel.mode = GL_TRIANGLES;
    squareModel.vertexCount = 6;
    glBindVertexArray(0);

    // Start using the program, and set the texture binding
    // of the first texture to 0.
    glUseProgram(program);
    glUniform1i(u_texture, 0);

    gl::checkErrors("GL initialisation errors");
}

void GLRenderer::close() {

    // Delete the square model.
    glDeleteVertexArrays(1, &squareModel.vao);
    glDeleteBuffers(1, (GLuint[]) {squareModel.vbo, squareModel.ibo});

    glDeleteTextures(1, &blankTexture);
    glDeleteFramebuffers(1, &multipurposeFBO);
    glDeleteProgram(program);
}


void GLRenderer::updateViewport( int width, int height ) {

    viewWidth = width;
    viewHeight = height;

    // Set the projection matrix.
    const float ortho[16] = {
        2.f / (float) viewWidth, 0.f, 0.f, -1.f,
        0.f, 2.f / (float) viewHeight, 0.f, -1.f,
        0.f, 0.f, -0.001f, 0.f,
        0.f, 0.f, 0.f, 1.f};

    glViewport(0, 0, viewWidth, viewHeight);
    glUniformMatrix4fv(u_projection_matrix, 1, true, ortho);
}


void GLRenderer::clear( float r, float g, float b, float a ) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLRenderer::setRenderColor( float r, float g, float b, float a ) {
    glUniform4f(u_color, r, g, b, a);
}


void GLRenderer::renderTexture( GLuint texture,
                                float posX, float posY,
                                float width, float height ) {

    // Move and scale the model.
    const float modelMatrix[16] = {
       width, 0.f, 0.f, posX,
       0.f, height, 0.f, posY,
       0.f, 0.f, 1.f, 0.f,
       0.f, 0.f, 0.f, 1.f,
    };

    // Bind the texture to binding point 0.
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Render the model.
    renderModel(squareModel, modelMatrix);
}

void GLRenderer::renderModel( const Model& model, const float modelmatrix[16] ) {

    // Update the model matrix.
    glUniformMatrix4fv(u_model_matrix, 1, true, modelmatrix);

    // Bind the VAO (and the VBO and IBO with it).
    glBindVertexArray(model.vao);

    // Draw the elements.
    glDrawElements(model.mode, model.vertexCount, GL_UNSIGNED_INT, 0);
}


void GLRenderer::renderToTexture( GLuint textureTarget, int attachment ) {
    // Bind the multipurpose framebuffer and attach the texture.
    glBindFramebuffer(GL_FRAMEBUFFER, multipurposeFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment,
                            GL_TEXTURE_2D, textureTarget, 0);
}

void GLRenderer::renderToScreen() {
    // Bind back to the window frambuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, windowFBO);
}


GLuint gl::compileShader( const std::string& source, GLenum type ) {
    GLuint shaderId = glCreateShader(type);

    // Upload and compile the shader.
    const char* cstr = source.c_str();
    int length = source.length();
    glShaderSource(shaderId, 1, &cstr, &length);
    glCompileShader(shaderId);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);

    if (compiled == GL_FALSE) {
        // The shader failed to compile, we'll print an error:

        std::string shaderName = "unknown";
        switch (type) {
        case GL_VERTEX_SHADER: shaderName = "vertex"; break;
        case GL_FRAGMENT_SHADER: shaderName = "fragment"; break;
        default: break;
        }

        // Get and print the shader compilation log, and delete the shader.
        int maxLength = 0, actualLength = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);
        std::string infoLog(maxLength, 0);
        glGetShaderInfoLog(shaderId, maxLength, &actualLength,
                           (char*) infoLog.c_str());
        glDeleteShader(shaderId);

        print(INFO_, "Failed to compile a", shaderName, "shader!");
        print(INFO_, "GL", type, "shader info: ", infoLog.substr(0, actualLength));
        return 0;
    }
    return shaderId;
}

GLuint gl::compileProgram( const std::string& vertexSource,
                           const std::string& fragmentSource ) {

    // Create a new GL program and compile vertex and fragment shaders.
    GLint programId = glCreateProgram();
    GLuint vertexId = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentId = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    // Attach the shaders to the program, and then link them.
    glAttachShader(programId, vertexId);
    glAttachShader(programId, fragmentId);
    glLinkProgram(programId);

    // The shaders are not needed anymore after linkage.
    glDetachShader(programId, vertexId);
    glDetachShader(programId, fragmentId);
    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);

    // Check if the compilation was succesful.
    GLint succes = GL_FALSE;
    glGetProgramiv(programId, GL_LINK_STATUS, &succes);

    if (succes == GL_FALSE) {
        // The program compilation has failed, we'll print an error!
        // Get the program compilation/link log, and then delete the program.
        int maxLength = 0, actualLength = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);
        std::string infoLog(maxLength, 0);
        glGetProgramInfoLog(programId, maxLength, &actualLength,
                            (char*) infoLog.c_str());
        glDeleteProgram(programId);

        print(INFO_, "Failed to create an OpenGL program!");
        print(INFO_, "GL program info: ", infoLog.substr(0, actualLength));
        return 0;
    }

    return programId;
}


GLuint gl::genTexture( int width, int height, const uint8_t* data ) {

    // Generate the texture and bind it.
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Set some texture parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Send the image to OpenGL.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Unbind the texture.
    glBindTexture(GL_TEXTURE_2D, 0);

    if (glGetError() == GL_OUT_OF_MEMORY)  {
        print(INFO_, "Out of texture memory!");
        glDeleteTextures(1, &textureId);
        return 0;
    }
    return textureId;
}


bool gl::checkErrors( const std::string& identifier ) {

    bool errorOccured = false;

    // Poll and print all the errors that may have occured.
    GLenum glerror;
    while ((glerror = glGetError()) != GL_NO_ERROR) {
        std::string errorType = "[unknown]";

        // Get the error type.
        switch (glerror) {
        case GL_INVALID_ENUM:
            errorType = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errorType = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errorType = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            errorType = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            errorType = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            errorType = "OUT_OF_MEMORY!";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION :
            errorType = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_CONTEXT_LOST:
            errorType = "CONTEXT_LOST";
            break;
        default:
            break;
        }

        // Print the error.
        print("GL ERROR ", identifier, "of type", errorType, "(id:", glerror, ")");
        errorOccured = true;
    }

    return errorOccured;
}
