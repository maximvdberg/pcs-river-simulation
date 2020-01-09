#version 460

precision mediump float;

in vec2 v_tex_coords;

out vec4 o_color;

uniform vec4 u_color;
uniform sampler2D u_texture;

void main() {
    o_color = texture(u_texture, v_tex_coords) * u_color;
}