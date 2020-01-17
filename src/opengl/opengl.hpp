/**
 * A renderer implementations using OpenGl.
 *
 * @file opengl.hpp
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 08-01-2020
 */

#pragma once

#include <string>

#define GL_GLEXT_PROTOTYPES yes please
#include <GL/gl.h>


namespace pcs {

    /**
     * A Model struct, which can be used to render objects / models.
     * This struct stores OpenGL buffers, the draw mode, and the vertex count,
     * which is everything needed to render an object. These OpenGL buffers
     * are:
     * Vertex Array Buffer (vao):
     *      This stores the binding of the VBO and IBO.
     * Vertex Buffer Object (vbo):
     *      This stores vertices of the model, which contain information such
     *      as position and texture coordinates (but can contain anything
     *      defined as input in the vertex shader).
     * Index Buffer Object / Element Array Buffer (ibo):
     *      This stores the indices of the vertices to be rendered, and as such
     *      specifies the ordering of the rendering of the vertices. Indices of
     *      vertices can be used more than once, which saves space.
     *
     * The draw mode (mode) specifies how OpenGL draws the vertices specified
     * using the buffers above. These modes are called primitives in OpenGL, and
     * can be things such as GL_TRIANGLES, or GL_LINES.
     */
    struct Model {
        GLuint vao, vbo, ibo;
        GLuint mode;
        GLuint vertexCount;
    };


    /**
     * An OpenGL renderer class.
     */
    class GLRenderer {

    public:
        /**
         * Construct the renderer, which sets up an OpenGL
         * program, and some buffers and textures.
         */
        GLRenderer();

        /**
         * Destruct the renderer, which just calls close()
         * @see GLRenderer::close()
         */
        inline ~GLRenderer() { close(); }

        /**
         * Close the renderer. Here the program and the
         * allocated buffers are deleted.
         */
        void close();


        /**
         * Update the viewport to a specific width and height, which must be
         * called after the window size has been changed.
         *
         * @param width The width of the viewport / screen.
         * @param width The height of the viewport / screen.
         */
        void updateViewport( int width, int height );


        /**
         * Clear the current rendering target to a specific background color.
         * The default clear color is black.
         *
         * @param r The red component in the range [0, 1.0]. Default is 0.f.
         * @param g The green component in the range [0, 1.0]. Default is 0.f.
         * @param b The blue component in the range [0, 1.0]. Default is 0.f.
         * @param a The alpha component in the range [0, 1.0]. Default is 1.f.
         */
        void clear( float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f );

        /**
         * Set the rendering color, which modifies the color of the to be
         * rendered texture. The default color is white.
         *
         * @param r The red component in the range [0, 1.0]. Default is 1.f.
         * @param g The green component in the range [0, 1.0]. Default is 1.f.
         * @param b The blue component in the range [0, 1.0]. Default is 1.f.
         * @param a The alpha component in the range [0, 1.0]. Default is 1.f.
         */
        void setRenderColor( float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);

        /**
         * Update the model matrix in the current program.
         *
         * @param matrix The model matrix.
         */
        void setModelMatrix( const float matrix[16]);

        /**
         * TODO
         */
        void setModelMatrix( float posX, float posY, float width, float height);

        /**
         * Render a texture to the current target. This function render the
         * texture specified by 'texture' to the position ('posX', 'posY')
         * with size ('width', 'height').
         *
         * @param texture The texture ID of the texture to be rendered.
         * @param posX The x position to render the texture to.
         * @param posY The y position to render the texture to.
         * @param width The width to render the texture.
         * @param height The height to render the texture.
         */
        void renderTexture( GLuint texture,
                            float posX, float posY,
                            float width, float height );

        /**
         * Render a rectangle to the current target. This function just calls
         * renderTexture() with a blank white texture.
         *
         * @see renderTexture()
         * @param posX The x position to render the texture to.
         * @param posY The y position to render the texture to.
         * @param width The width to render the texture.
         * @param height The height to render the texture.
         */
        inline void renderRectangle( float posX, float posY,
                                     float width, float height ) {
            renderTexture(getBlankTexture(), posX, posY, width, height);
        }

        /**
         * Render a model to the current target, positioned by the model
         * transformation matrix 'modelmatrix'.
         *
         * @param model The model to render.
         * @param modelMatrix The 4x4 matrix specifying the model matrix.
         */
        void renderModel( const Model& model );


        /**
         * Render to a texture instead of the window.
         *
         * @param textureTarget The texture to target, e.g. to render to.
         * @param attachment Which color attachment to use when rendering.
         *                   Defaults to color attachment 0.
         */
        void renderToTexture( GLuint textureTarget, int attachment = 0 );

        /**
         * Reset the rendering target back to the screen.
         */
        void renderToScreen();


        /**
         * Use the OpenGL program specified by 'program'.
         *
         * @param program The program to use.
         */
        void useProgram( GLuint program );

        /**
         * Set the OpenGL program back to the default rendering program.
         */
        void resetProgram();


        /**
         * Get a default blank texture.
         *
         * @return A texture ID for a blank texture.
         */
        inline GLuint getBlankTexture() {
            return blankTexture;
        }

        /**
         * Get a default square model.
         *
         * @return The a square model object.
         */
        inline Model& getSquareModel() {
            return squareModel;
        }


    private:

        // The main program ID.
        GLint program;

        // Frame buffer objects.
        GLuint windowFBO;
        GLuint multipurposeFBO;

        // Shader locations. i_* are input attributes and u_* are uniforms.
        GLint i_position;
        GLint i_tex_coords;
        GLint u_projection_matrix;
        GLint u_model_matrix;

        GLint u_color;
        GLint u_texture;

        // The size of the current view.
        int viewWidth, viewHeight;

        // A default blank texture and square model.
        GLuint blankTexture;
        Model squareModel;
    };


    namespace gl {

        /**
         * Send a glsl shader to OpenGL and compile it. Returns the shader ID,
         * or 0 if the compilation fails. Errors, if there are any, are
         * printed to stdout.
         *
         * @param source The source code of the shader.
         * @param type The GL shader type, which can be GL_VERTEX_SHADER
         *             or GL_FRAGMENT_SHADER.
         * @return The shader ID, or 0 if the compilation has failed.
         */
        GLuint compileShader( const std::string& source, GLenum type );

        /**
         * Compile an OpenGL program which consists of two glsl shader sources.
         * Returns the program ID, or 0 if there are compilation or linkage
         * errors. These errors, if there are any, are printed to stdout.
         *
         * @param vertexSource The source of the vertex shader.
         * @param fragmentSource The source of the fragment shader.
         * @return The program ID, or 0 if the compilation has failed.
         */
        GLuint compileProgram( const std::string& vertexSource,
                               const std::string& fragmentSource );

        /**
         * Generate an OpenGL texture of width 'width' and height 'height'.
         * Pixel data can be supplied by setting 'data', and must be formated
         * as an float per color in RGBA format, so 4 float per pixel.
         *
         * @param width The width of the texture.
         * @param height The height of the texture.
         * @param data A pointer to the first color of the
         *             pixel data, formatted RGBA.
         * @return The texture id, or 0 if the generation has failed.
         */
        GLuint genTexture( int width, int height, const float* data = nullptr );

        /**
         * Check and print any OpenGL errors that may have occured. An
         * identifier can be supplied to mark the printed output. This
         * function returns true if errors occured, false otherwise.
         *
         * @param identifier A string identifier.
         * @return True if there were OpenGL errors, false otherwise.
         */
        bool checkErrors( const std::string& identifier = "" );

    }
}
