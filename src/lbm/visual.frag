#version 450

precision highp float;

in vec2 v_tex_coords;

layout(location = 0) out vec4 o_color;

layout(location = 2) uniform vec4 u_color;
layout(location = 3) uniform sampler2D u_textures[4];

void main() {

    vec4 color_0 = texture(u_textures[0], v_tex_coords);
    vec4 color_1 = texture(u_textures[1], v_tex_coords);
    vec4 color_2 = texture(u_textures[2], v_tex_coords);


    bool isWall = color_0.b != 0.0;

    if (isWall) {
        o_color = vec4(1.0, 1.0, 1.0, 1.0) * color_1.r;
    }
    else {
        float rho = color_1.r;
        vec2 u = color_1.gb;
        o_color = vec4(u_color.rgb * length(u) * 2.0, u_color.a);
        // o_color = vec4(color_2.g, color_2.g, color_2.g, u_color.a);

        // o_color = vec4(u_color.rgb * rho * 0.2, u_color.a);


    }
}