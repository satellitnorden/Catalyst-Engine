//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_BIAS (0.000002f) //0.00000025f step.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES (16)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_STRENGTH (12.0f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D normalDepthTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

//Globals.
const vec4 OFFSETS[SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES] = vec4[]
(
    vec4(0.160342f, 0.134352f, 0.977875f, 0.417016f),
    vec4(-0.555022f, -0.0352526f, 0.831089f, 0.361247f),
    vec4(-0.122818f, 0.0766132f, 0.989468f, 0.716011f),
    vec4(0.0298833f, -0.186107f, 0.982075f, 0.918594f),
    vec4(0.192722f, 0.2321f, 0.953408f, 0.610965f),
    vec4(-0.0235938f, -0.51514f, 0.856781f, 0.581424f),
    vec4(-0.0845561f, 0.157048f, 0.983964f, 0.12091f),
    vec4(-0.0370264f, 0.0192786f, 0.999128f, 0.950775f),
    vec4(-0.205923f, -0.209953f, 0.95578f, 0.773565f),
    vec4(0.0912673f, 0.228398f, 0.96928f, 0.990999f),
    vec4(0.767193f, -0.275477f, 0.579248f, 0.24417f),
    vec4(0.205055f, 0.0917852f, 0.974437f, 0.630981f),
    vec4(0.124231f, 0.356073f, 0.926163f, 0.302339f),
    vec4(0.265947f, 0.658822f, 0.703723f, 0.920877f),
    vec4(-0.102848f, -0.216343f, 0.970885f, 0.23776f),
    vec4(-0.575781f, 0.229497f, 0.784734f, 0.332612f)
);

const vec2 RANDOM_ROTATIONS[SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES] = vec2[]
(
    vec2(0.675226f, -0.737611f),
    vec2(0.110247f, 0.993904f),
    vec2(-0.93761f, 0.34769f),
    vec2(-0.43239f, 0.901687f),
    vec2(0.781061f, -0.624455f),
    vec2(-0.163937f, 0.986471f),
    vec2(-0.53297f, 0.846134f),
    vec2(0.634454f, -0.772961f),
    vec2(-0.869561f, 0.493825f),
    vec2(0.137257f, 0.990535f),
    vec2(0.278903f, 0.960319f),
    vec2(-0.549912f, 0.835222f),
    vec2(-0.554601f, 0.832117f),
    vec2(0.979976f, 0.199117f),
    vec2(-0.869843f, 0.493328f),
    vec2(-0.496487f, 0.868044f)
);

void main()
{
    //Sample the normal depth texture.
    vec4 normalDepthTextureSampler = texture(normalDepthTexture, fragmentTextureCoordinate);

    //Retrieve the normal.
    vec3 normal = normalDepthTextureSampler.xyz;

    //Retrieve the depth.
    float depth = normalDepthTextureSampler.w;

    //Retrieve the fragment world position.
    vec3 fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, depth);

    //Calculate the random offset vector.
    vec3 randomRotation = vec3(RANDOM_ROTATIONS[int(RandomFloat(vec3(gl_FragCoord.xy, depth)) * SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES)], 0.0f);

    //Calculate the tangent space matrix.
    vec3 tangent = normalize(randomRotation - normal * dot(randomRotation, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tangentSpaceMatrix = mat3(tangent, bitangent, normal);

    //Calculate the occlusion.
    float occlusion = 0.0f;

    for (int i = 0; i < SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES; ++i)
    {
        vec3 currentSamplePosition = fragmentWorldPosition + (tangentSpaceMatrix * OFFSETS[i].xyz) * OFFSETS[i].w;

        vec4 offset = vec4(currentSamplePosition, 1.0f);
        offset = viewMatrix * offset;
        offset.xyz /= offset.w;
        offset.xy = offset.xy * 0.5f + 0.5f;

        float currentSampleDepth = texture(normalDepthTexture, offset.xy).w;
        vec3 currentSampleActualPosition = CalculateFragmentWorldPosition(offset.xy, currentSampleDepth);

        float fade = mix(1.0f, 0.0f, SmoothStep(min(LengthSquared3(currentSamplePosition - currentSampleActualPosition), 1.0f)));

        occlusion += (offset.z < currentSampleDepth - SCREEN_SPACE_AMBIENT_OCCLUSION_BIAS ? 1.0f : 0.0f) * fade;
    }

    occlusion = 1.0f - (occlusion / SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES);

    //Modify the value a bit.
    occlusion = pow(occlusion, SCREEN_SPACE_AMBIENT_OCCLUSION_STRENGTH);

    //Write the fragment.
    fragment = vec4(occlusion);
}