#version 430

precision highp float;

in vec2 v_tex_coords;

layout(location = 0) out uvec4 o_color[7];

layout(location = 3) uniform usampler2D u_textures[7];

/*
 * How the textures are mapped:
 * -----------+---------------+----------------
 * | Texture  | Mapped to     | input/output
 * -----------+---------------+-----------------
 *   0.r        Walls           In/out
 *   0.g        Flow            In
 *   0.b        Walls temp      In
 *   0.a        empty           --
 *   1.rg       u flow x        Out
 *   1.ba       u flow y        Out
 *   2.rg       rho             Out
 *   2.ba       f0 (0)          In/out
 *   3.rg       f1 (E)          In/out
 *   3.ba       f2 (N)          In/out
 *   4.rg       f3 (W)          In/out
 *   4.ba       f4 (S)          In/out
 *   5.rg       f5 (NE)         In/out
 *   5.ba       f6 (NW)         In/out
 *   6.rg       f7 (SW)         In/out
 *   6.ba       f8 (SE)         In/out
 */



// Some constants.
const double viscosity = 0.001;              // Viscosity
const double delta_x = 1.0;                  // Lattice spacing
const double delta_t = 1.0;                  // Time step
double c = delta_x / delta_t;                // Lattice speed
double omega = 2 / (6 * viscosity * delta_t
                   / (delta_x * delta_x) + 1);   // Parameter for "relaxation"
const double u0 = 0.3;                       // Initial and in-flow speed


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
const float cor_act   = 0.30;      // Centre of the curve
const float cor_lim   = 0.05;      // Maximum probability
const float cor_slope = 30.0;      // Slope of the curve

// Constants for sedimentation activation curve
const float sed_act   = 0.05;      // Centre of the curve
const float sed_lim   = 0.05;      // Maximum probability
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

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

double get1f( in usampler2D textr, in vec2 pos ) {
    return packDouble2x32(texture(textr, pos).rg);
}

double get2f( in usampler2D textr, in vec2 pos ) {
    return packDouble2x32(texture(textr, pos).ba);
}


void main() {

    const ivec2 texture_size = textureSize(u_textures[0], 0);
    const ivec2 texture_loc = ivec2(v_tex_coords * vec2(texture_size - ivec2(1)) + vec2(0.5));
    const vec2 pixel_size = 1.0 / texture_size;

    // Copy the data like walls and such.
    uvec4 data = texture(u_textures[0], v_tex_coords);


    // Get the f values and stream.
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
    if (data.r != 0) {
        data.b = 1;
        data.r = 0;
        for (uint i = 0; i < 9; i++) {
            f[i] = -abs(f[i]);
        }
    }

    bool isWall = data.b != 0;


    double rho = 0.0;
    dvec2 u = vec2(0.0);

    if (!isWall) {
        for (uint i = 0; i < 9; i++) {
            f[i] = abs(f[i]);
            if (f[i] < 0) {
            }
        }

        // Collide:
        for (uint i = 0; i < 9; i++) {
            rho += f[i];
        }

        for (uint i = 1; i < 9; i++) {
            u += e[i] * f[i];
        }
        u *= c / rho;


        double udotu = 1.5 * dot(u, u) / (c * c);

        for (uint i = 0; i < 9; i++) {
            double edotu_c = 3.0*dot(e[i], u) / c;
            double s = w[i] * (edotu_c + edotu_c*edotu_c / 2.0 - udotu);

            double feq = w[i] * rho + rho * s;

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
    // rho = 0.0;

    if (isWall) { // isWall

        double press_x = f[1]*double(f[1]>0.0) + f[3]*double(f[3]>0.0) + f[5]*double(f[5]>0.0) +
                         f[6]*double(f[6]>0.0) + f[7]*double(f[7]>0.0) + f[8]*double(f[8]>0.0);
                        // E + NE + SE - W - NW - SW
        double press_y = f[2]*double(f[2]>0.0) + f[4]*double(f[4]>0.0) + f[5]*double(f[5]>0.0) +
                         f[6]*double(f[6]>0.0) + f[7]*double(f[7]>0.0) + f[8]*double(f[8]>0.0);
                        // N + NE + NW - S - SE - SW

        double press = sqrt(press_x*press_x + press_y*press_y);
        rho = press;

        /* if (press > 0.28) { */
        if (cor(float(press)) > rand(vec2(press*texture_loc)) + 0.02) {
            // Corrosion
            data.b = 0;


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
        }

        // Bounce back
        double f2c = f[2]; // N
        double f3c = f[3]; // W
        double f6c = f[6]; // NW
        double f7c = f[7]; // SW

        f[2] = -abs(f[4]); // N -> S
        f[3] = -abs(f[1]); // W -> E
        f[4] = -abs(f2c);  // S -> N
        f[1] = -abs(f3c);  // E -> W

        f[6] = -abs(f[8]); // NW -> SE
        f[7] = -abs(f[5]); // SW -> NE
        f[8] = -abs(f6c);  // SE -> NW
        f[5] = -abs(f7c);  // NE -> SW
    }
    else {
        // Sedimentation
        if (sed(float(length(u))) > rand(vec2(u)) + 0.03) {
            data.r = 1;
        }

    }

    // Flow from the right.
    if (!isWall && data.g != 0) {

       const double u02 = u0*u0;

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


    // Ouput to the textures.
    o_color[0] = data;
    o_color[1] = uvec4(unpackDouble2x32(u.x),  unpackDouble2x32(u.y));
    o_color[2] = uvec4(unpackDouble2x32(rho),  unpackDouble2x32(f[0]));
    o_color[3] = uvec4(unpackDouble2x32(f[1]), unpackDouble2x32(f[2]));
    o_color[4] = uvec4(unpackDouble2x32(f[3]), unpackDouble2x32(f[4]));
    o_color[5] = uvec4(unpackDouble2x32(f[5]), unpackDouble2x32(f[6]));
    o_color[6] = uvec4(unpackDouble2x32(f[7]), unpackDouble2x32(f[8]));
}
