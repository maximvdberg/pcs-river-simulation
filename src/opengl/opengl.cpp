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

#include "../print.hpp"

#include <sstream>
#include <fstream>

using namespace pcs;


GLRenderer::GLRenderer() {

    // Compile the rendering program.
    program = gl::compileProgram(readFile("src/opengl/main.vert"),
                                 readFile("src/opengl/main.frag"));

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
    blankTexture = gl::genTexture(1, 1, (float[]) {1.0f, 1.0f, 1.0f, 1.0f});

    // Create a square model used for rendering simple textured squares.
    glGenVertexArrays(1, &squareModel.vao);
    glBindVertexArray(squareModel.vao);
    constexpr size_t fSize = sizeof (GLfloat); // Size of float shortcut
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


void GLRenderer::setModelMatrix( const float matrix[16] ) {
    glUniformMatrix4fv(u_model_matrix, 1, true, matrix);
}

void GLRenderer::setModelMatrix( float posX, float posY, float width, float height ) {
    const float modelMatrix[16] = {
       (float) width, 0.f, 0.f, posX,
       0.f, (float) height, 0.f, posY,
       0.f, 0.f, 1.f, 0.f,
       0.f, 0.f, 0.f, 1.f,
    };
    setModelMatrix(modelMatrix);
}


void GLRenderer::renderTexture( GLuint texture,
                                float posX, float posY,
                                float width, float height ) {

    // Move and scale the model.
    setModelMatrix(posX, posY, width, height);

    // Bind the texture to binding point 0.
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Render the model.
    renderModel(squareModel);
}

void GLRenderer::renderModel( const Model& model ) {

    // Bind the VAO (and the VBO and IBO with it).
    glBindVertexArray(model.vao);

    // Draw the elements.
    glDrawElements(model.mode, model.vertexCount, GL_UNSIGNED_INT, 0);
}


void GLRenderer::renderToTexture( GLuint textureTarget ) {

    // Bind the multipurpose framebuffer and attach the texture.
    glBindFramebuffer(GL_FRAMEBUFFER, multipurposeFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, textureTarget, 0);
}

void GLRenderer::renderToScreen() {
    // Bind back to the window frambuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, windowFBO);
}


void GLRenderer::useProgram( GLuint program_ ) {
    glUseProgram(program_);
}

void GLRenderer::resetProgram() {
    glUseProgram(program);
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


GLuint gl::genTexture( int width, int height, const float* data ) {

    // Generate the texture and bind it.
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Set the textures to repeat (e.g. periodic boundary conditions).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Disable pixel interpolation.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Send the image to OpenGL.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_FLOAT, data);

    // Unbind the texture.
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

GLuint gl::genUTexture( int width, int height, const uint32_t* data ) {

    // Generate the texture and bind it.
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Set the textures to repeat (e.g. periodic boundary conditions).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Disable pixel interpolation.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Send the image to OpenGL.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, width, height, 0,
                 GL_RGBA_INTEGER, GL_UNSIGNED_INT, data);

    // Unbind the texture.
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}


GLuint gl::loadTexture( const std::string& filePath,
                        int* widthPtr, int* heightPtr ) {



    // Load the file.
    std::ifstream file(filePath, std::ios::binary);

    if (!file) {
        print(INFO_, "Failed to load the texture at", "["+filePath+"]",
              "! Does it exists?");
        return 0;
    }

    // Read the header of the bitmap.
    constexpr size_t bmp_header_size = 54;
    char header[bmp_header_size];
    file.read(header, bmp_header_size);

    const int offset = *(int*) &header[10];
    const int hsize  = *(int*) &header[14];  // Header size.
    const int width  = *(int*) &header[18];
    const int height = *(int*) &header[22];
    const int bbp    = *(int*) &header[28];  // Bits per pixel.
    const int comprm = *(int*) &header[30];  // Compression method.

    // Give up if the following conditions aren't met.
    if (header[0] != 'B' || header[1] != 'M' ||              // Bitmap type.
        hsize == 12 || hsize == 16 || hsize == 64 ||        // Header sizes.
        comprm != 0 ||                     // Compression Method (= BI_RGB).
        bbp != 24 ||                 // We only support 24 bit bitmaps here.
        height < 0) {                             // No upside down bitmaps.

        print(INFO_, "Bitmap type from", "["+filePath+"]", "is not supported!");
        return 0;
    }

    // Find the row and image size of the bitmap.
    const int rowSize = ((bbp * width + 31) / 32) * 4; // In bytes
    const int imageSize = rowSize * height;

    // Move to the right offset and read the pixel data.
    file.seekg(offset);
    uint8_t* const source = new uint8_t[imageSize];
    file.read((char*) source, imageSize);
    file.close();

    // Convert the bitmap data to pixel data.
    float* const pixels = new float[width * height * 4];
    float* ptr = pixels;

    for (int y = 0; y < std::abs(height); ++y) {
        for (int x = 0; x < width; ++x) {
            int i = 3 * x + y * rowSize;
            *(ptr++) = (float) source[i+2] / 255.f;
            *(ptr++) = (float) source[i+1] / 255.f;
            *(ptr++) = (float) source[i] / 255.f;
            *(ptr++) = 1.f;
        }
    }
    delete[] source;

    // Create the texture using the acquired pixel data.
    GLuint texture = gl::genTexture(width, height, pixels);
    delete[] pixels;

    // 'Return' the width and height of the texture.
    if (widthPtr != nullptr)
        *widthPtr = width;
    if (heightPtr != nullptr)
        *heightPtr = height;

    return texture;
}


bool gl::checkErrors( const std::string& identifier ) {

    bool errorOccured = false;

    // Poll and print all the errors that may have occured.
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string errorType = "[unknown]";

        // Get the error type.
        switch (errorCode) {
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
        print("GL ERROR ", identifier, "of type", errorType, "(id:", errorCode, ")");
        errorOccured = true;
    }

    return errorOccured;
}


std::string pcs::readFile( const std::string& path ) {

    // Open the file and create a buffer to read it.
    std::ifstream file(path);
    std::stringstream buffer;

    // Read the file if it exists, and print an warning otherwise.
    if (file) {
        buffer << file.rdbuf();
        return buffer.str();
    }
    else {
        print(INFO_, "file", path, "does not exists!");
        return "";
    }
}
