#version 450

precision highp float;

in vec2 v_tex_coords;

layout(location = 0) out vec4 o_color;

layout(location = 2) uniform vec4 u_color;
layout(location = 3) uniform sampler2D u_textures[3];


// Flow directions.
const ivec2 e[9] = {ivec2(0, 0),  ivec2(1, 0),   ivec2(0, 1),
                    ivec2(-1, 0), ivec2(0, -1),  ivec2(1, 1),
                    ivec2(-1, 1), ivec2(-1, -1), ivec2(1, -1)};

void main() {

    vec4 color_0 = texture(u_textures[0], v_tex_coords);
    vec4 color_1 = texture(u_textures[1], v_tex_coords);
    vec4 color_2 = texture(u_textures[2], v_tex_coords);

    bool isWall = color_0.r != 0.0;

    if (isWall) {
        o_color = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else {
        float rho = color_0.a + color_1.r + color_1.g +
                    color_1.b + color_1.a + color_2.r +
                    color_2.g + color_2.b + color_2.a;

        const float c = 1.0;

        vec2 u = (                 e[1]*color_1.r + e[2]*color_1.g +
                  e[3]*color_1.b + e[4]*color_1.a + e[5]*color_2.r +
                  e[6]*color_2.g + e[7]*color_2.b + e[8]*color_2.a) * c / rho;

        o_color = vec4(u_color.rgb * length(u) * 2.0, u_color.a);


        // o_color = vec4(u_color.rgb * rho * 0.2, u_color.a);


    }
}