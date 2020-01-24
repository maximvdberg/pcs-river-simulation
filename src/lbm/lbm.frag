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
const double viscosity = 0.005;              // Viscosity
const double delta_x = 1.0;                  // Lattice spacing
const double delta_t = 1.0;                  // Time step
double c = delta_x / delta_t;                // Lattice speed
double omega = 2 / (6 * viscosity * delta_t
                   / (delta_x * delta_x) + 1);  // Parameter for "relaxation"
const dvec2 u0 = dvec2(0.1,0.0);                // Initial in-flow speed
const double rho0 = 1.0;

const dvec2 u_slope = dvec2(0.0001,0.0);


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
const float cor_lim   = 0.08;     // Maximum probability
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


    // Make the wall.
    if (data.g != 0) {
        data.g = 0;
        data.a = 1;

        // Invert f_i's.
        // for (uint i = 0; i < 9; i++) {
        //     f[i] = -abs(f[i]);
        // }
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

        for (uint i = 0; i < 9; i++) {
            rho += f[i];
            u += e[i] * f[i];
        }
        double u_len = length(u);
        u += u_slope;
        u *= u_len / length(u);
        u *= c / rho;
    }


    if (!isWall && !isSource) {
        // Interpolate f with feq
        double udotu = dot(u,u);
        for (uint i = 0; i < 9; i++) {
            f[i] = (1 - omega) * f[i] + omega * calc_feq(i, rho, u, udotu);
        }

        // Sedimentation.
        if (u_settings[2] && sed(float(length(u))) > rand(vec2(u)*texture_loc) + 0.0001) {
            data.g = 1; // Add wall next step.
        }
    }
    else if (isWall && !isSource) {

        bool isIndestructible = data.r != 0;

        if (!isIndestructible) {
            double press_x = f[1]*double(f[1]>0.0) + f[5]*double(f[5]>0.0) + f[8]*double(f[8]>0.0) -
                             f[3]*double(f[3]>0.0) - f[6]*double(f[6]>0.0) - f[7]*double(f[7]>0.0);
                             // E + NE + SE - W - NW - SW
            double press_y = f[2]*double(f[2]>0.0) + f[5]*double(f[5]>0.0) + f[6]*double(f[6]>0.0) -
                             f[4]*double(f[4]>0.0) - f[8]*double(f[8]>0.0) - f[7]*double(f[7]>0.0);
                             // N + NE + NW - S - SE - SW

            double press = sqrt(press_x*press_x + press_y*press_y);
            rho = press;

            if (u_settings[3]) {
                rho = 1.0;
            }

            /* if (press > 0.28) { */
            if (u_settings[1] && (cor(float(press)) > rand(vec2(press*texture_loc)) + 0.0002)) {
                // Corrosion, remove the wall
                data.a = 0;
            }
        }

        // Bounce back
        double f2c = f[2]; // N
        double f3c = f[3]; // W
        double f6c = f[6]; // NW
        double f7c = f[7]; // SW

        const double modmod = 0.999;

        f[2] = -abs(f[4])*modmod; // N -> S
        f[3] = -abs(f[1])*modmod; // W -> E
        f[4] = -abs(f2c)*modmod;  // S -> N
        f[1] = -abs(f3c)*modmod;  // E -> W

        f[6] = -abs(f[8])*modmod; // NW -> SE
        f[7] = -abs(f[5])*modmod; // SW -> NE
        f[8] = -abs(f6c)*modmod;  // SE -> NW
        f[5] = -abs(f7c)*modmod;  // NE -> SW

    }
    else { // is Source
        // Flow to the right.
        u = dvec2(0.0); // dvec2(length(u),0);

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
