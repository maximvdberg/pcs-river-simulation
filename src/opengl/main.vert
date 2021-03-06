/**
 * The main vertex shader for rendering to the viewport.
 *
 * @file main.vert
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 15-01-2020
 */
#version 430

precision highp float;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_tex_coords;

out vec2 v_tex_coords;

layout(location = 0) uniform mat4 u_projection_matrix;
layout(location = 1) uniform mat4 u_model_matrix;

void main() {
    v_tex_coords = i_tex_coords;
    gl_Position = u_projection_matrix * u_model_matrix * vec4(i_position, 1.0);
}
