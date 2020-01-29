/**
 * The implementation of the modified LBM model.
 *
 * @file lbm.frag
 * @author Jurriaan van den Berg
 * @author Maxim van den Berg
 * @author Melvin Seitner
 * @date 15-01-2020
 */

#version 430

precision highp float;

in vec2 v_tex_coords;

layout(location = 0) out uvec4 o_color[7];

layout(location = 3) uniform usampler2D u_textures[7];

layout(location = 10) uniform bvec4 u_settings;


 #define ENABLE_FLOW
 #define ENABLE_SEDIMENTATION
 #define ENABLE_EROSION
 #define ENABLE_SLOPE


// Some constants.
const double viscosity = 0.005;             // Viscosity
const double delta_x = 1.0;                  // Lattice spacing
const double delta_t = 1.0;               // Time step
double c = delta_x / delta_t;                // Lattice speed
double omega =  2 / (6 * viscosity * delta_t
                     / (delta_x * delta_x) + 1);  // Parameter for "relaxation"
const dvec2 u0 = dvec2(0.1, 0.0);                // Initial in-flow speed
const double rho0 = 1.0;


#ifdef ENABLE_SLOPE
const dvec2 u_slope = dvec2(0.1, 0.0);
#endif


// f_i directions.
const dvec2 e[9] = {dvec2(0., 0.),  dvec2(1., 0.),   dvec2(0., 1.),
                    dvec2(-1., 0.), dvec2(0., -1.),  dvec2(1., 1.),
                    dvec2(-1., 1.), dvec2(-1., -1.), dvec2(1., -1.)};

// Float f_i directions to avoid having to convert from doubles to floats.
const vec2 ef[9] = {vec2(0., 0.),  vec2(1., 0.),   vec2(0., 1.),
                    vec2(-1., 0.), vec2(0., -1.),  vec2(1., 1.),
                    vec2(-1., 1.), vec2(-1., -1.), vec2(1., -1.)};

// Flow weights for each f_i.
const double w[9] = {4. /  9., 1. /  9., 1. /  9.,
                     1. /  9., 1. /  9., 1. / 36.,
                     1. / 36., 1. / 36., 1. / 36.};


// The activation probability function.
float sigma( float x ) {
    return 1 / (1 + exp(-x));
}

// Constants for erosion activation curve
#ifdef ENABLE_EROSION
const float ero_act   = 0.00;      // Centre of the curve
const float ero_lim   = 1.0;     // Maximum probability
const float ero_slope = 1000.0;      // Slope of the curve

// Erosion activation curve
float sigma_a = sigma(-ero_slope * ero_act);
float ero_scaling = ero_lim / (1 - sigma_a);
float ero( float x ) {
    return (sigma(ero_slope * (x - ero_act)) - sigma_a) * ero_scaling;
}
#endif

// Constants for sedimentation activation curve
#ifdef ENABLE_SEDIMENTATION
const float sed_act   = 0.00;      // Centre of the curve
const float sed_lim   = 0.005;      // Maximum probability
const float sed_slope = 100.0;     // Slope of the curve

// Sedimentation activation curve
float sigma_b = sigma(-sed_slope * sed_act);
float sed_scaling = sed_lim / (1 - sigma_b);
float sed( float x ) {
    return sed_lim - (sigma(sed_slope * (x - sed_act)) - sigma_b) * sed_scaling;
}
#endif



float rand( in vec2 co ) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// Get the first double from a texture.
double get1f( in usampler2D textr, in vec2 pos ) {
    return packDouble2x32(texture(textr, pos).rg);
}

// Get the second double from a texture.
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
    uvec4 gridData = texture(u_textures[0], v_tex_coords);
    bool isIndestructible = gridData.r != 0;
    bool addWall = gridData.g != 0;
    bool isSource = gridData.b != 0;
    bool isWall = gridData.a != 0;

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
    if (addWall) {
        isWall = true;
        addWall = false;

        // Invert f_i's.
        for (uint i = 1; i < 9; i++) {
            f[i] = -abs(f[i]);
        }
    }


    #ifdef ENABLE_EROSION
    if (u_settings[1] && isWall && !isSource) {

        // Memory for force and bounce-back calculations.
        double phi[9] = {
            get2f(u_textures[2], v_tex_coords), // f0
            get1f(u_textures[3], v_tex_coords), // f1
            get2f(u_textures[3], v_tex_coords), // f2
            get1f(u_textures[4], v_tex_coords), // f3
            get2f(u_textures[4], v_tex_coords), // f4
            get1f(u_textures[5], v_tex_coords), // f5
            get2f(u_textures[5], v_tex_coords), // f6
            get1f(u_textures[6], v_tex_coords), // f7
            get2f(u_textures[6], v_tex_coords)  // f8
        };

        if (!isIndestructible) {

            // Calculate the momentum exchange

            // // Momentum exchange.
            dvec2 F = (e[1] * (abs(phi[1]) + f[3]) * double(f[1] > 0) + // E  <-> W
                       e[2] * (abs(phi[2]) + f[4]) * double(f[2] > 0) + // N  <-> S
                       e[3] * (abs(phi[3]) + f[1]) * double(f[3] > 0) + // W  <-> E
                       e[4] * (abs(phi[4]) + f[2]) * double(f[4] > 0) + // S  <-> N
                       e[5] * (abs(phi[5]) + f[7]) * double(f[5] > 0) + // NE <-> SW
                       e[6] * (abs(phi[6]) + f[8]) * double(f[6] > 0) + // NW <-> SE
                       e[7] * (abs(phi[7]) + f[5]) * double(f[7] > 0) + // SW <-> NE
                       e[8] * (abs(phi[8]) + f[6]) * double(f[8] > 0))
                       * c * delta_x; //  // SE <-> NW

            // dvec2 F = dvec2(0.0);
            // for (uint i = 0; i < 9; i++) {
            //     F += e[i] * (f2[i] - f[i]);
            // }

            double press = length(F) * 1;
            // double press = length(u);

            if ((ero(float(press) - 0.01) >
                                  rand(vec2(press*texture_loc)))) {
                // Erosion, remove the wall
                isWall = false;
            }
        }
    }
    #endif


    // Calculate rho (the density) and u (the velocity vector).
    double rho = 0.0;
    dvec2 u = vec2(0.0);
    for (uint i = 0; i < 9; i++) {
        rho += abs(f[i]);// * double(f[i] > 0);
        u += e[i] * abs(f[i]);// * double(f[i] > 0);
    }

    #ifdef ENABLE_SLOPE
    if (u_settings[3] && !isWall && !isSource) {
        double u_len = length(u);
        u += u_slope;

        // Normalise the flow.
        if (length(u) != 0.0) {
            u *= u_len / length(u);
        }
    }
    #endif
    u *= c / rho;



    // Collision step: Interpolate f with feq
    double udotu = dot(u, u);
    for (uint i = 0; i < 9; i++) {
        f[i] = max(0.0, (1 - omega) * abs(f[i]) + omega * calc_feq(i, rho, u, udotu));
    }


    // Sedimentation.
    #ifdef ENABLE_SEDIMENTATION
    if (u_settings[2] && !isSource && !isWall &&
        sed(float(length(u))) > rand(vec2(u)*texture_loc) + 0.003) {
        addWall = true; // Add wall next step.
    }
    #endif

    // Wall bounce back.
    if (isWall) {

        // Memory for force and bounce-back calculations.
        double f2[9] = {abs(f[0]), abs(f[1]), abs(f[2]),
                        abs(f[3]), abs(f[4]), abs(f[5]),
                        abs(f[6]), abs(f[7]), abs(f[8])};

        const double momentum_mod = 1.00;

        // Bounce back
        f[2] = -f2[4] * momentum_mod; // N -> S
        f[3] = -f2[1] * momentum_mod; // W -> E
        f[4] = -f2[2] * momentum_mod; // S -> N
        f[1] = -f2[3] * momentum_mod; // E ->f2
        f[6] = -f2[8] * momentum_mod; // NW -> SE
        f[7] = -f2[5] * momentum_mod; // SW -> NE
        f[8] = -f2[6] * momentum_mod; // SE -> NW
        f[5] = -f2[7] * momentum_mod; // NE -> SW


        // Bounce back (old version)
        // double f2c = f[2]; // N
        // double f3c = f[3]; // W
        // double f6c = f[6]; // NW
        // double f7c = f[7]; // SW

        // f[2] = -abs(f[4]); // N -> S
        // f[3] = -abs(f[1]); // W -> E
        // f[4] = -abs(f2c);  // S -> N
        // f[1] = -abs(f3c);  // E -> W
        // f[6] = -abs(f[8]); // NW -> SE
        // f[7] = -abs(f[5]); // SW -> NE
        // f[8] = -abs(f6c);  // SE -> NW
        // f[5] = -abs(f7c);  // NE -> SW

        // const double p = 0.0;
        // const double q = 1 - p;
        // f[2] = -(f2[2]*p + f2[4]*q); // N -> S
        // f[3] = -(f2[1]*p + f2[3]*q); // W -> E
        // f[4] = -(f2[4]*p + f2[2]*q); // S -> N
        // f[1] = -(f2[1]*p + f2[3]*q); // E ->f2
        // f[6] = -(f2[6]*p + f2[8]*q); // NW -> SE
        // f[7] = -(f2[7]*p + f2[5]*q); // SW -> NE
        // f[8] = -(f2[8]*p + f2[6]*q); // SE -> NW
        // f[5] = -(f2[5]*p + f2[7]*q); // NE -> SW
    }


    // Flow to the right.
    #ifdef ENABLE_FLOW
    if (u_settings[0] && isSource) {
        u = u0;
        double udotu = dot(u, u);

        rho = rho0;
        for (uint i = 0; i < 9; i++) {
            f[i] = calc_feq(i, rho0, u, udotu);
        }
    }
    #endif


    // Ouput to the textures.
    o_color[0] = uvec4(isIndestructible, addWall, isSource, isWall);
    o_color[1] = uvec4(unpackDouble2x32(u.x),  unpackDouble2x32(u.y));
    o_color[2] = uvec4(unpackDouble2x32(rho),  unpackDouble2x32(f[0]));
    o_color[3] = uvec4(unpackDouble2x32(f[1]), unpackDouble2x32(f[2]));
    o_color[4] = uvec4(unpackDouble2x32(f[3]), unpackDouble2x32(f[4]));
    o_color[5] = uvec4(unpackDouble2x32(f[5]), unpackDouble2x32(f[6]));
    o_color[6] = uvec4(unpackDouble2x32(f[7]), unpackDouble2x32(f[8]));
}
