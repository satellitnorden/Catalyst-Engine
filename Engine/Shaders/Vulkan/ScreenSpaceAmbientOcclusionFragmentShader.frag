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
    vec4(0.219829f, -0.0181393f, 0.97537f, 0.837577f),
    vec4(-0.0338448f, 0.322444f, 0.945983f, 0.559986f),
    vec4(0.186517f, -0.224047f, 0.956564f, 0.52743f),
    vec4(0.181267f, -0.000210787f, 0.983434f, 0.288236f),
    vec4(0.12784f, 0.192726f, 0.972889f, 0.324175f),
    vec4(-0.0123094f, 0.156803f, 0.987553f, 0.915912f),
    vec4(0.102128f, -0.0165396f, 0.994634f, 0.807652f),
    vec4(0.00427068f, -0.0653888f, 0.997851f, 0.674581f),
    vec4(0.302492f, 0.231113f, 0.924708f, 0.66501f),
    vec4(0.143447f, -0.0134358f, 0.989567f, 0.203218f),
    vec4(-0.12827f, -0.298121f, 0.94587f, 0.700439f),
    vec4(0.360029f, 0.226355f, 0.905065f, 0.17086f),
    vec4(-0.0623375f, 0.156204f, 0.985756f, 0.179935f),
    vec4(-0.773697f, -0.408185f, 0.48454f, 0.117954f),
    vec4(-0.190283f, 0.201299f, 0.96087f, 0.742108f),
    vec4(-0.180338f, 0.106679f, 0.977803f, 0.3767f)
);

const vec2 RANDOM_ROTATIONS[SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES] = vec2[]
(
    vec2(0.580382f, -0.814345f),
    vec2(-0.952523f, -0.304466f),
    vec2(0.532284f, 0.846566f),
    vec2(0.10381f, -0.994597f),
    vec2(-0.29368f, 0.955904f),
    vec2(0.745307f, 0.666721f),
    vec2(0.796271f, 0.60494f),
    vec2(0.998482f, -0.0550783f),
    vec2(-0.999656f, -0.0262286f),
    vec2(0.565609f, -0.824673f),
    vec2(0.692846f, 0.721086f),
    vec2(0.127573f, 0.991829f),
    vec2(0.935527f, 0.353254f),
    vec2(-0.980995f, -0.194034f),
    vec2(-0.725905f, 0.687795f),
    vec2(0.170237f, 0.985403f)
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
    vec3 randomRotation = vec3(RANDOM_ROTATIONS[int(gl_FragCoord.x + gl_FragCoord.y) & (SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES - 1)], 0.0f);

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