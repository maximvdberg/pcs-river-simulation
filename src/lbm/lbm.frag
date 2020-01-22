#version 430

precision highp float;

in vec2 v_tex_coords;

layout(location = 0) out vec4 o_color[4];

layout(location = 3) uniform sampler2D u_textures[4];


// Some constants.
const float viscosity = 0.2;                    // viscosity
const float omega = 1 / (3*0.1 + 0.5);     // parameter for "relaxation"
const float u0 = 0.1;                            // initial and in-flow speed
const float c = 1.0;

// Constants for corrosion activation curve
const float cor_act = 10.0;      // Centre of the curve
const float cor_lim = 0.04;      // Maximum probability
const float cor_slope = 0.5;     // Slope of the curve

// Constants for sedimentation activation curve
const float sed_act = 0.2;      // Centre of the curve
const float sed_lim = 0.7;      // Maximum probability
const float sed_slope = 20;     // Slope of the curve


float sigma(float x) {
    return 1 / (1 + exp(-x));
}

// Corrosion activation curve
float sigma_a = sigma(-cor_slope * cor_act);
float cor_scaling = cor_lim / (1 - sigma_a);
float cor( float x ) {
    return (sigma(cor_slope * (x - cor_act)) - sigma_a) * cor_scaling;
}

// Sedimentation activation curve
float sigma_b = sigma(-sed_slope * sed_act);
float sed_scaling = sed_lim / (1 - sigma_b);
float sed( float x ) {
    return (sigma(sed_slope * (x - sed_act)) - sigma_b) * sed_scaling;
}


// Flow directions.
// const ivec2 e[9] = {ivec2(0, 0),  ivec2(1, 0),   ivec2(0, 1),
//                     ivec2(-1, 0), ivec2(0, -1),  ivec2(1, 1),
//                     ivec2(-1, 1), ivec2(-1, -1), ivec2(1, -1)};

const vec2 e[9] = {vec2(0, 0),  vec2(1, 0),   vec2(0, 1),
                   vec2(-1, 0), vec2(0, -1),  vec2(1, 1),
                   vec2(-1, 1), vec2(-1, -1), vec2(1, -1)};


// Flow weights.
const float w[9] = {4.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
                    1.0 / 9.0, 1.0 / 9.0, 1.0 / 36.0,
                    1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0};



ivec2 pmod( ivec2 i, ivec2 n ) {
    return ivec2(mod(mod(i, n) + n, n));
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


void main() {

    const ivec2 texture_size = textureSize(u_textures[0], 0);
    const ivec2 texture_loc = ivec2(v_tex_coords * vec2(texture_size - ivec2(1)) + vec2(0.5));
    const vec2 pixel_size = 1.0 / texture_size;

    float f[9];
    // vec4 texture0 = texture(u_textures[0], v_tex_coords);

    // Copy the data like walls and such.
    o_color[0] = texture(u_textures[0], v_tex_coords);


    // Stream.
    f[0] = texture(u_textures[1], v_tex_coords).a;                                      // f0 + rest
    f[1] = texture(u_textures[2], v_tex_coords - pixel_size*e[1]).r; // f1
    f[2] = texture(u_textures[2], v_tex_coords - pixel_size*e[2]).g; // f2
    f[3] = texture(u_textures[2], v_tex_coords - pixel_size*e[3]).b; // f3
    f[4] = texture(u_textures[2], v_tex_coords - pixel_size*e[4]).a; // f4
    f[5] = texture(u_textures[3], v_tex_coords - pixel_size*e[5]).r; // f5
    f[6] = texture(u_textures[3], v_tex_coords - pixel_size*e[6]).g; // f6
    f[7] = texture(u_textures[3], v_tex_coords - pixel_size*e[7]).b; // f7
    f[8] = texture(u_textures[3], v_tex_coords - pixel_size*e[8]).a; // f8

    if (o_color[0].r != 0.0) {
        o_color[0].b = 1.0;
        o_color[0].r = 0.0;
        for (int i = 0; i < 9; i++) {
            f[i] = -abs(f[i]);
        }
    }
    bool isWall = o_color[0].b != 0.0;


    float rho = 0.0;
    vec2 u = vec2(0.0);

    if (!isWall) {
        for (int i = 0; i < 9; i++) {
            f[i] = abs(f[i]);
            if (f[i] < 0) {
            }
        }

        // Collide:
        for (int i = 0; i < 9; i++) {
            rho += f[i];
        }

        for (int i = 1; i < 9; i++) {
            u += e[i] * f[i];
        }
        u *= c / rho;


        float udotu = 1.5 * dot(u, u) / (c * c);

        for (int i = 0; i < 9; i++) {
            float edotu_c = 3.0*dot(e[i], u) / c;
            float s = w[i] * (edotu_c + pow(edotu_c, 2)/2 - udotu);

            float feq = w[i] * rho + rho * s;

            f[i] -= omega * (f[i] - feq);
            // if (f[i] < 0.0) {
            //     f[i] = 0.0;
            // }
            // if (f[i] > 1.0) {
            //     f[i] = 1.0;
            // }
        }
    }


    // Boundary:
    rho = 0.0;

    if (isWall) { // isWall

        float press_x = f[1]*float(f[1]>0.0) + f[3]*float(f[3]>0.0) + f[5]*float(f[5]>0.0) +
                        f[6]*float(f[6]>0.0) + f[7]*float(f[7]>0.0) + f[8]*float(f[8]>0.0);
                        // E + NE + SE - W - NW - SW
        float press_y = f[2]*float(f[2]>0.0) + f[4]*float(f[4]>0.0) + f[5]*float(f[5]>0.0) +
                        f[6]*float(f[6]>0.0) + f[7]*float(f[7]>0.0) + f[8]*float(f[8]>0.0);
                        // N + NE + NW - S - SE - SW

        float press = sqrt(pow(press_x, 2) + pow(press_y, 2));

        rho = press;

        if (press > 0.28) {
        // if (cor(press) > rand(press*texture_loc) + 0.0001) {
        // if (f[5] > 0.01 && false) {
            // Corrosion
            o_color[0].b = 0.0;

            // Set water density
            //f[1] = (f[1] + f[3]); // E + W
            //f[3] = f[1];
            //f[2] = (f[2] + f[4]); // N + S
            //f[4] = f[2];
            //f[5] = (f[5] + f[7]); // NE + SW
            //f[7] = f[5];
            //f[6] = (f[6] + f[8]); // NW + SE
            //f[8] = f[6];

            //f[0] = 0;
            //for (int i = 0; i < 9; i++)
            //    f[0] = max(f[0],texture(u_textures[0], v_tex_coords - pixel_size * e[i]).a);
        } else {

        }

        // Bounce back
        float f2c = f[2]; // N
        float f3c = f[3]; // W
        float f6c = f[6]; // NW
        float f7c = f[7]; // SW

        f[2] = -abs(f[4]); // N -> S
        f[3] = -abs(f[1]); // W -> E
        f[4] = -abs(f2c);  // S -> N
        f[1] = -abs(f3c);  // E -> W

        f[6] = -abs(f[8]); // NW -> SE
        f[7] = -abs(f[5]); // SW -> NE
        f[8] = -abs(f6c);  // SE -> NW
        f[5] = -abs(f7c);  // NE -> SW
    }
    // else {
    //     // Sedimentation
    //     if (length(u) < 0.001) {
    //         if (rand(u) < 0.1) {
    //             texture0.r = 1.0;
    //         }
    //     }

    // }

    // Flow from the right.
    if (!isWall && o_color[0].g != 0.0 /*&& 200 < texture_loc.y && texture_loc.y < 300*/) {
       float u02 = pow(u0, 2);
       //f[0] = 0.0; // 0
       f[1] = w[1]*(1 + 3*u0 + 4.5*u02 - 1.5*u02); // E
       //f[2] = 0.0; // N
       f[3] = w[3]*(1 - 3*u0 + 4.5*u02 - 1.5*u02); // W
       f[4] = w[4]*(1 - 3*u0 + 4.5*u02 - 1.5*u02); // S
       f[5] = w[5]*(1 + 3*u0 + 4.5*u02 - 1.5*u02); // NE
       f[6] = w[6]*(1 - 3*u0 + 4.5*u02 - 1.5*u02); // NW
       f[7] = w[7]*(1 - 3*u0 + 4.5*u02 - 1.5*u02); // SW
       f[8] = w[8]*(1 + 3*u0 + 4.5*u02 - 1.5*u02); // SE
    }


    // Output the colors.
    o_color[1] = vec4(rho, u.x, u.y, f[0]);
    o_color[2] = vec4(f[1], f[2], f[3], f[4]);
    o_color[3] = vec4(f[5], f[6], f[7], f[8]);
}
