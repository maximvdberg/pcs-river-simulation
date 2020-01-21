#version 430

precision highp float;

in vec2 v_tex_coords;

out vec4 o_color;

layout(location = 2) uniform vec4 u_color;
layout(location = 3) uniform sampler2D u_texture;

void main() {
    o_color = texture(u_texture, v_tex_coords) * u_color;
}