#if !defined(CATALYST_SIMPLEX_NOISE)
#define CATALYST_SIMPLEX_NOISE

vec3 SimplexNoise_Permute(vec3 x)
{
    return mod(((x*34.0)+1.0)*x, 289.0);
}

/*
*   Returns noise in the range [0.0f, 1.0f] at the given coordinate.
*/
float SimplexNoise_Generate(vec2 coordinate, float seed)
{
    coordinate.xy += seed;

    #define SIMPLEX_NOISE_CONSTANT (vec4(0.211324865405187f, 0.366025403784439f, -0.577350269189626f, 0.024390243902439f))

    vec2 i  = floor(coordinate + dot(coordinate, SIMPLEX_NOISE_CONSTANT.yy) );
    vec2 x0 = coordinate -   i + dot(i, SIMPLEX_NOISE_CONSTANT.xx);
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + SIMPLEX_NOISE_CONSTANT.xxzz;
    x12.xy -= i1;
    i = mod(i, 289.0);
    vec3 p = SimplexNoise_Permute( SimplexNoise_Permute( i.y + vec3(0.0, i1.y, 1.0 )) + i.x + vec3(0.0, i1.x, 1.0 ));
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    vec3 x = 2.0 * fract(p * SIMPLEX_NOISE_CONSTANT.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    vec3 g;

    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;

    #undef SIMPLEX_NOISE_CONSTANT

    return (130.0f * dot(m, g)) * 0.5f + 0.5f;
}

#endif