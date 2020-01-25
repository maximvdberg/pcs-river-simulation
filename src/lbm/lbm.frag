#version 430

precision highp float;

in vec2 v_tex_coords;

layout(location = 0) out uvec4 o_color[7];

layout(location = 3) uniform usampler2D u_textures[7];

layout(location = 10) uniform bvec4 u_settings;

/*
 * How the textures are mapped:
 * +----------+---------------+---------------+
 * | Texture  | Mapped to     | input/output  |
 * +----------+---------------+---------------+
 * | 0.r        Walls temp      In            |
 * | 0.g        Indestructible  In/out        |
 * | 0.b        Flow source     In/out        |
 * | 0.a        Walls           In/out        |
 * | 1.rg       u flow x        Out           |
 * | 1.ba       u flow y        Out           |
 * | 2.rg       rho             Out           |
 * | 2.ba       f0 (0)          In/out        |
 * | 3.rg       f1 (E)          In/out        |
 * | 3.ba       f2 (N)          In/out        |
 * | 4.rg       f3 (W)          In/out        |
 * | 4.ba       f4 (S)          In/out        |
 * | 5.rg       f5 (NE)         In/out        |
 * | 5.ba       f6 (NW)         In/out        |
 * | 6.rg       f7 (SW)         In/out        |
 * | 6.ba       f8 (SE)         In/out        |
 * +------------------------------------------+
 *
 */



// Some constants.
const double viscosity = 0.0005;              // Viscosity
const double delta_x = 1.0;                  // Lattice spacing
const double delta_t = 1.0;                  // Time step
double c = delta_x / delta_t;                // Lattice speed
double omega = 2 / (6 * viscosity * delta_t
                   / (delta_x * delta_x) + 1);  // Parameter for "relaxation"
const dvec2 u0 = dvec2(0.10,0.0);                // Initial in-flow speed
const double rho0 = 1.0;

const dvec2 u_slope = dvec2(0.000,0.0);


const dvec2 e[9] = {dvec2(0., 0.),  dvec2(1., 0.),   dvec2(0., 1.),
                    dvec2(-1., 0.), dvec2(0., -1.),  dvec2(1., 1.),
                    dvec2(-1., 1.), dvec2(-1., -1.), dvec2(1., -1.)};

const vec2 ef[9] = {vec2(0., 0.),  vec2(1., 0.),   vec2(0., 1.),
                    vec2(-1., 0.), vec2(0., -1.),  vec2(1., 1.),
                    vec2(-1., 1.), vec2(-1., -1.), vec2(1., -1.)};

// Flow weights.
const double w[9] = {4. /  9., 1. /  9., 1. /  9.,
                     1. /  9., 1. /  9., 1. / 36.,
                     1. / 36., 1. / 36., 1. / 36.};


// Constants for corrosion activation curve
const float cor_act   = 1.0;      // Centre of the curve
const float cor_lim   = 0.12;     // Maximum probability
const float cor_slope = 6.0;      // Slope of the curve

// Constants for sedimentation activation curve
const float sed_act   = 0.001;      // Centre of the curve
const float sed_lim   = 0.0005;      // Maximum probability
const float sed_slope = 160.0;     // Slope of the curve


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
    return sed_lim - (sigma(sed_slope * (x - sed_act)) - sigma_b) * sed_scaling;
}




ivec2 pmod( in ivec2 i, in ivec2 n ) {
    return ivec2(mod(mod(i, n) + n, n));
}

float rand( in vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

double get1f( in usampler2D textr, in vec2 pos ) {
    return packDouble2x32(texture(textr, pos).rg);
}

double get2f( in usampler2D textr, in vec2 pos ) {
    return packDouble2x32(texture(textr, pos).ba);
}

// Get the index of the opposite direction
uint inverti( in uint i ) {
    return ((i + 1)%4 + 1) * uint(0 < i && i < 5) + ((i - 3)%4 + 5) * uint(i > 4);
}


double calc_feq( in uint i , in double rho, in dvec2 u, in double udotu ) {
    double edotu_c = 3.0*dot(e[i], u) / c;
    return w[i] * rho * (1 + edotu_c + edotu_c*edotu_c / 2.0 - 1.5 * udotu / (c * c));
}


void main() {

    const ivec2 texture_size = textureSize(u_textures[0], 0);
    const ivec2 texture_loc = ivec2(v_tex_coords * vec2(texture_size - ivec2(1)) + vec2(0.5));
    const vec2 pixel_size = 1.0 / texture_size;

    // Copy the data like walls and such.
    uvec4 data = texture(u_textures[0], v_tex_coords);

    // Get the f values and stream at the same time.
    double f[9] = {
        get2f(u_textures[2], v_tex_coords),                    // f0
        get1f(u_textures[3], v_tex_coords - pixel_size*ef[1]), // f1
        get2f(u_textures[3], v_tex_coords - pixel_size*ef[2]), // f2
        get1f(u_textures[4], v_tex_coords - pixel_size*ef[3]), // f3
        get2f(u_textures[4], v_tex_coords - pixel_size*ef[4]), // f4
        get1f(u_textures[5], v_tex_coords - pixel_size*ef[5]), // f5
        get2f(u_textures[5], v_tex_coords - pixel_size*ef[6]), // f6
        get1f(u_textures[6], v_tex_coords - pixel_size*ef[7]), // f7
        get2f(u_textures[6], v_tex_coords - pixel_size*ef[8])  // f8
    };

    // Memory for force and bounce-back calculations.
    double phi[9] = { f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8] };


    // Make the wall.
    if (data.g != 0) {
        data.g = 0;
        data.a = 1;

        // Invert f_i's.
        /* for (uint i = 1; i < 9; i++) { */
        /*     f[i] = -abs(f[i]); */
        /* } */
    }

    bool isWall = data.a != 0;
    bool isSource = data.b != 0;


    double rho = 0.0;
    dvec2 u = vec2(0.0);
    double feq[9];

    // Collide
    if (!isWall) {
        for (uint i = 0; i < 9; i++) {
            f[i] = abs(f[i]);
        }
    }

    for (uint i = 0; i < 9; i++) {
        rho += f[i] * double(f[i] > 0);
        u += e[i] * f[i] * double(f[i] > 0);
    }
    double u_len = length(u);
    if (u_len > 0) {
        u += u_slope;
        u *= u_len / length(u);
        u *= c / rho;
    }



    if (!isWall && !isSource) {
        // Interpolate f with feq
        double udotu = dot(u,u);
        for (uint i = 0; i < 9; i++) {
            f[i] = max(0,(1 - omega) * f[i] + omega * calc_feq(i, rho, u, udotu));
        }

        // Sedimentation.
        if (u_settings[2] && sed(float(length(u))) > rand(vec2(u)*texture_loc) + 0.0001) {
            data.g = 1; // Add wall next step.
        }
    }
    else if (isWall && !isSource) {
        // Bounce back
        const double modmod = 0.99;

        f[2] = -abs(phi[4]) * modmod; // N -> S
        f[3] = -abs(phi[1]) * modmod; // W -> E
        f[4] = -abs(phi[2]) * modmod; // S -> N
        f[1] = -abs(phi[3]) * modmod; // E -> W

        f[6] = -abs(phi[8]) * modmod; // NW -> SE
        f[7] = -abs(phi[5]) * modmod; // SW -> NE
        f[8] = -abs(phi[6]) * modmod; // SE -> NW
        f[5] = -abs(phi[7]) * modmod; // NE -> SW

        bool isIndestructible = data.r != 0;
        if (!isIndestructible) {
            // Calculate the momentum exchange
            dvec2 F = e[1] * (phi[1] + f[1]) * double(phi[1] > 0) + // E  <-> W
                      e[2] * (phi[2] + f[2]) * double(phi[2] > 0) + // N  <-> S
                      e[3] * (phi[3] + f[3]) * double(phi[3] > 0) + // W  <-> E
                      e[4] * (phi[4] + f[4]) * double(phi[4] > 0) + // S  <-> N
                      e[5] * (phi[5] + f[5]) * double(phi[5] > 0) + // NE <-> SW
                      e[6] * (phi[6] + f[6]) * double(phi[6] > 0) + // NW <-> SE
                      e[7] * (phi[7] + f[7]) * double(phi[7] > 0) + // SW <-> NE
                      e[8] * (phi[8] + f[8]) * double(phi[8] > 0);  // SE <-> NW

            double press = length(F) * 1;

            if (u_settings[1] && (cor(float(press) - 0.001) > rand(vec2(press*texture_loc)))) {
                // Corrosion, remove the wall
                data.a = 0;

                // Adjust for the momentum of the 'moving' wall.
                dvec2 mom = dvec2(double(u.x >= 0) + double(u.x < 0),double(u.x >= 0) + double(u.y < 0));
                f[2] = min(0, f[2] - 2 / (c*c) * w[4] * dot(e[4],mom));                //
                f[3] = min(0, f[3] - 2 / (c*c) * w[1] * dot(e[1],mom));                //
                f[4] = min(0, f[4] - 2 / (c*c) * w[2] * dot(e[2],mom));                //
                f[1] = min(0, f[1] - 2 / (c*c) * w[3] * dot(e[3],mom));                //

                f[6] = min(0, f[6] - 2 / (c*c) * w[8] * dot(e[8],mom));
                f[7] = min(0, f[7] - 2 / (c*c) * w[5] * dot(e[5],mom));
                f[8] = min(0, f[8] - 2 / (c*c) * w[6] * dot(e[6],mom));
                f[5] = min(0, f[5] - 2 / (c*c) * w[7] * dot(e[7],mom));
            } else {
                rho = press;
                if (u_settings[3]) {
                    rho = 1.0;
                }
            }
        }

    }
    else { // is Source
        // Flow to the right.
        u = dvec2(0.0);//dvec2(length(u),0);

        if (u_settings[0] && data.b != 0) {
            u += u0;

            double udotu = dot(u, u);

            rho = rho0;
            for (uint i = 0; i < 9; i++) {
                f[i] = calc_feq(i, rho0, u, udotu);
            }

        }

    }

    // Ouput to the textures.
    o_color[0] = data;
    o_color[1] = uvec4(unpackDouble2x32(u.x),  unpackDouble2x32(u.y));
    o_color[2] = uvec4(unpackDouble2x32(rho),  unpackDouble2x32(f[0]));
    o_color[3] = uvec4(unpackDouble2x32(f[1]), unpackDouble2x32(f[2]));
    o_color[4] = uvec4(unpackDouble2x32(f[3]), unpackDouble2x32(f[4]));
    o_color[5] = uvec4(unpackDouble2x32(f[5]), unpackDouble2x32(f[6]));
    o_color[6] = uvec4(unpackDouble2x32(f[7]), unpackDouble2x32(f[8]));
}
