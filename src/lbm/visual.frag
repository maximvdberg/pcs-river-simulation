#version 450

precision highp float;

in vec2 v_tex_coords;

layout(location = 0) out vec4 o_color;

layout(location = 2) uniform vec4 u_color;
layout(location = 3) uniform usampler2D u_textures[7];


void main() {

    // Copy the data like walls and such.
    uvec4 color_0 = texture(u_textures[0], v_tex_coords);
    uvec4 color_1 = texture(u_textures[1], v_tex_coords);
    uvec4 color_2 = texture(u_textures[2], v_tex_coords);

    bool isWall = color_0.a != 0;

    float rho = float(packDouble2x32(color_2.rg));

    if (isWall) {
        o_color = vec4(1.0, 1.0, 1.0, 1.0) * float(color_0.b);
        o_color = vec4(1.0, 1.0, 1.0, 1.0) * float(rho);
    }
    else {
        vec2 u = vec2(float(packDouble2x32(color_1.rg)),
                      float(packDouble2x32(color_1.ba)));

        o_color = vec4(u_color.rgb * length(u), u_color.a);
        // o_color = vec4(color_2.g, color_2.g, color_2.g, u_color.a);

        // o_color = vec4(u_color.rgb * rho * 0.2, u_color.a);
    }
}
