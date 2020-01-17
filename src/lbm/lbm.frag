#version 450

precision highp float;

in vec2 v_tex_coords;

layout(location = 0) out vec4 o_color[3];

layout(location = 3) uniform sampler2D u_textures[3];


// Some constants.
const float viscosity = 0.02;                    // viscosity
const float omega = 1 / (3*0.02 + 0.5);     // parameter for "relaxation"
const float u0 = 0.1;                            // initial and in-flow speed

// Flow directions.
// const ivec2 e[9] = {ivec2(0, 0),  ivec2(1, 0),   ivec2(0, 1),
//                     ivec2(-1, 0), ivec2(0, -1),  ivec2(1, 1),
//                     ivec2(-1, 1), ivec2(-1, -1), ivec2(1, -1)};

const vec2 e[9] = {vec2(0, 0),  vec2(1, 0),   vec2(0, 1),
                   vec2(-1, 0), vec2(0, -1),  vec2(1, 1),
                   vec2(-1, 1), vec2(-1, -1), vec2(1, -1)};


// Flow weights.
const float w[3] = {4.0 / 9.0, 1.0 / 9.0, 1.0 / 36.0};



ivec2 pmod( ivec2 i, ivec2 n ) {
    return ivec2(mod(mod(i, n) + n, n));
}

void main() {

    ivec2 texture_size = textureSize(u_textures[0], 0);
    ivec2 texture_loc = ivec2(v_tex_coords * vec2(texture_size - ivec2(1)) + vec2(0.5));
    vec2 pixel_size = 1.0 / texture_size;

    o_color[0] = texture(u_textures[0], v_tex_coords);
    o_color[1] = texture(u_textures[1], v_tex_coords);
    o_color[2] = texture(u_textures[2], v_tex_coords);

    // Stream:
    o_color[0]   = texture(u_textures[0], v_tex_coords);              // f0 + rest
    o_color[1].r = texture(u_textures[1], v_tex_coords - pixel_size*e[1]).r; // f1
    o_color[1].g = texture(u_textures[1], v_tex_coords - pixel_size*e[2]).g; // f2
    o_color[1].b = texture(u_textures[1], v_tex_coords - pixel_size*e[3]).b; // f3
    o_color[1].a = texture(u_textures[1], v_tex_coords - pixel_size*e[4]).a; // f4
    o_color[2].r = texture(u_textures[2], v_tex_coords - pixel_size*e[5]).r; // f5
    o_color[2].g = texture(u_textures[2], v_tex_coords - pixel_size*e[6]).g; // f6
    o_color[2].b = texture(u_textures[2], v_tex_coords - pixel_size*e[7]).b; // f7
    o_color[2].a = texture(u_textures[2], v_tex_coords - pixel_size*e[8]).a; // f8

    // // Stream:
    // o_color[0] = texture(u_textures[0], v_tex_coords); // f0 + rest
    // o_color[1].r = texelFetch(u_textures[1], pmod(texture_loc - e[1], texture_size), 0).r; // f1
    // o_color[1].g = texelFetch(u_textures[1], pmod(texture_loc - e[2], texture_size), 0).g; // f2
    // o_color[1].b = texelFetch(u_textures[1], pmod(texture_loc - e[3], texture_size), 0).b; // f3
    // o_color[1].a = texelFetch(u_textures[1], pmod(texture_loc - e[4], texture_size), 0).a; // f4
    // o_color[2].r = texelFetch(u_textures[2], pmod(texture_loc - e[5], texture_size), 0).r; // f5
    // o_color[2].g = texelFetch(u_textures[2], pmod(texture_loc - e[6], texture_size), 0).g; // f6
    // o_color[2].b = texelFetch(u_textures[2], pmod(texture_loc - e[7], texture_size), 0).b; // f7
    // o_color[2].a = texelFetch(u_textures[2], pmod(texture_loc - e[8], texture_size), 0).a; // f8


    // Collide:
    float rho = o_color[0].a + o_color[1].r + o_color[1].g +
                o_color[1].b + o_color[1].a + o_color[2].r +
                o_color[2].g + o_color[2].b + o_color[2].a;

    const float c = 1.f;

    vec2 u =  c / rho * (         e[1]*o_color[1].r + e[2]*o_color[1].g +
              e[3]*o_color[1].b + e[4]*o_color[1].a + e[5]*o_color[2].r +
              e[6]*o_color[2].g + e[7]*o_color[2].b + e[8]*o_color[2].a);

    vec2 u2 = u * u;
    float omu215 = 1 - 1.5 * (u2.x + u2.y); // one minus u2 times 1.5
    float omg = 1 - omega;

    o_color[0].a = (1-omega)*o_color[0].a + omega*w[0]*rho*(omu215); // f0
    o_color[1].r = (1-omega)*o_color[1].r + omega*w[1]*rho*(omu215 + 3.*dot(e[1], u) + 4.5*pow(dot(e[1], u), 2)); // f1
    o_color[1].g = (1-omega)*o_color[1].g + omega*w[1]*rho*(omu215 + 3.*dot(e[2], u) + 4.5*pow(dot(e[2], u), 2)); // f2
    o_color[1].b = (1-omega)*o_color[1].b + omega*w[1]*rho*(omu215 + 3.*dot(e[3], u) + 4.5*pow(dot(e[3], u), 2)); // f3
    o_color[1].a = (1-omega)*o_color[1].a + omega*w[1]*rho*(omu215 + 3.*dot(e[4], u) + 4.5*pow(dot(e[4], u), 2)); // f8
    o_color[2].r = (1-omega)*o_color[2].r + omega*w[2]*rho*(omu215 + 3.*dot(e[5], u) + 4.5*pow(dot(e[5], u), 2)); // f4
    o_color[2].g = (1-omega)*o_color[2].g + omega*w[2]*rho*(omu215 + 3.*dot(e[6], u) + 4.5*pow(dot(e[6], u), 2)); // f5
    o_color[2].b = (1-omega)*o_color[2].b + omega*w[2]*rho*(omu215 + 3.*dot(e[7], u) + 4.5*pow(dot(e[7], u), 2)); // f6
    o_color[2].a = (1-omega)*o_color[2].a + omega*w[2]*rho*(omu215 + 3.*dot(e[8], u) + 4.5*pow(dot(e[8], u), 2)); // f7


    // Boundary:
    bool isWall = o_color[0].r != 0.0;

    // Bounce-back.
    if (isWall) {
        float f1 = o_color[1].r;
        float f2 = o_color[1].g;
        float f5 = o_color[2].r;
        float f6 = o_color[2].g;

        o_color[1].r = o_color[1].b;
        o_color[1].g = o_color[1].a;
        o_color[1].b = f1;
        o_color[1].a = f2;

        o_color[2].r = o_color[2].b;
        o_color[2].g = o_color[2].a;
        o_color[2].b = f5;
        o_color[2].a = f6;
    }

    // Flow from the right.
    if (texture_loc.x == 1) {
       float u02 = pow(u0, 2);
       // o_color[0].a = 0.0; // 0
       o_color[1].r = w[1]*(1 + 3*u0 + 4.5*u02 - 1.5*u02); // E
       // o_color[1].g = 0.0; // N
       o_color[1].b = w[1]*(1 - 3*u0 + 4.5*u02 - 1.5*u02); // W
       o_color[1].a = w[1]*(1 - 3*u0 + 4.5*u02 - 1.5*u02); // S
       o_color[2].r = w[2]*(1 + 3*u0 + 4.5*u02 - 1.5*u02); // NE
       o_color[2].g = w[2]*(1 - 3*u0 + 4.5*u02 - 1.5*u02); // NW
       o_color[2].b = w[2]*(1 - 3*u0 + 4.5*u02 - 1.5*u02); // SW
       o_color[2].a = w[2]*(1 + 3*u0 + 4.5*u02 - 1.5*u02); // SE
    }

    //o_color[1] = vec4(2.0, 0.5, 0.6, 1.2);
}
