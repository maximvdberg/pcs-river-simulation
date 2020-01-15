#version 450

precision highp float;

in vec2 v_tex_coords;

layout(location = 0) out vec4 o_color_0;
layout(location = 1) out vec4 o_color_1;
layout(location = 2) out vec4 o_color_2;

layout(location = 3) uniform sampler2D u_textures[3];


// Some constants.
const float viscosity = 0.02;                    // viscosity
const float omega = 1 / (3*viscosity + 0.5);     // parameter for "relaxation"
const float u0 = 0.1;                            // initial and in-flow speed

// Flow directions.
const ivec2 e[9] = {ivec2(0, 0),  ivec2(1, 0),   ivec2(0, 1),
                    ivec2(-1, 0), ivec2(0, -1),  ivec2(1, 1),
                    ivec2(-1, 1), ivec2(-1, -1), ivec2(1, -1)};

// Flow weights.
const float weights[9] = {4./9.,  1./9.,  1./9.,
                          1./9.,  1./9.,  1./36.,
                          1./36., 1./36., 1./36.};


void main() {

    ivec2 tex_size = textureSize(u_textures[0], 0);

    ivec2 tex_location = ivec2(v_tex_coords * tex_size);

    vec4 f0123 = texture(u_textures[0], v_tex_coords);
    vec4 f4567 = texture(u_textures[1], v_tex_coords);
    vec4 f8 = texture(u_textures[2], v_tex_coords);

    vec4 c = texelFetch(u_textures[0], tex_location, 0);
    vec4 c_up = texelFetch(u_textures[0], (tex_location + ivec2(0, 1)) % tex_size, 0);
    vec4 c_down = texelFetch(u_textures[0], (tex_location + ivec2(0, -1)) % tex_size, 0);
    vec4 c_left = texelFetch(u_textures[0], (tex_location + ivec2(-1, 0)) % tex_size, 0);
    vec4 c_right = texelFetch(u_textures[0], (tex_location + ivec2(1, 0)) % tex_size, 0);


    float f = c.r;
    if (c_up.r > c.r) f += 0.1;
    else if (c_up.r < c.r) f -= 0.1;

    if (c_down.r > c.r) f += 0.1;
    else if (c_down.r < c.r) f -= 0.1;

    if (c_left.r > c.r) f += 0.1;
    else if (c_left.r < c.r) f -= 0.1;

    if (c_right.r > c.r) f += 0.1;
    else if (c_right.r < c.r) f -= 0.1;

    if (f < 0.) f = 0.;
    if (f > 1.) f = 1.;

    c.r = f;

    o_color_0 = c;
    o_color_1 = c.grba;
    o_color_2 = c.bgra;
}