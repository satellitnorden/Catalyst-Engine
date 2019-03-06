//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_BIAS (0.0f) //0.00000025f step.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES (32)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_STRENGTH (16.0f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D normalDepthTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

//Globals.
const vec3 OFFSETS[SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES] = vec3[]
(
	vec3(0.00146439f, -0.00340203f, 0.0164241f),
	vec3(-0.0398421f, -0.03644f, 0.024866f),
	vec3(-0.0125847f, -0.00686353f, 0.000456982f),
	vec3(0.012722f, 0.0486458f, 0.0450291f),
	vec3(-0.0373146f, 0.0142463f, 0.00425753f),
	vec3(0.0190431f, 0.000319f, 0.0293914f),
	vec3(-0.0218543f, 0.0995196f, 0.0696732f),
	vec3(0.0367944f, -0.11167f, 0.0623511f),
	vec3(-0.126983f, 0.0574264f, 0.0531446f),
	vec3(0.0503008f, -0.0292847f, 0.0492497f),
	vec3(0.118708f, -0.0628995f, 0.0384283f),
	vec3(0.0199865f, 0.0248185f, 0.117677f),
	vec3(0.059041f, -0.0438434f, 0.00158785f),
	vec3(0.046415f, -0.0343496f, 0.0273436f),
	vec3(0.113579f, -0.125047f, 0.166042f),
	vec3(-0.122868f, -0.201911f, 0.0871577f),
	vec3(0.00495604f, 0.0567349f, 0.0518003f),
	vec3(-0.0302869f, 0.0226982f, 0.00356777f),
	vec3(-0.0403451f, -0.221049f, 0.196623f),
	vec3(-0.0100627f, -0.0154811f, 0.016966f),
	vec3(-0.171261f, 0.111656f, 0.013134f),
	vec3(-0.0416877f, -0.0352276f, 0.0496381f),
	vec3(0.0868384f, 0.0312574f, 0.182653f),
	vec3(-0.0524546f, 0.166166f, 0.205249f),
	vec3(0.453322f, 0.32458f, 0.114085f),
	vec3(-0.187922f, -0.527654f, 0.00765433f),
	vec3(0.000103954f, -5.39507e-05f, 1.99589e-05f),
	vec3(-0.555813f, 0.14699f, 0.0488688f),
	vec3(-0.101826f, -0.318647f, 0.176888f),
	vec3(-0.23002f, 0.188275f, 0.1806f),
	vec3(0.244486f, 0.38319f, 0.367251f),
	vec3(0.0387352f, 0.0986341f, 0.162456f)
);

const vec2 RANDOM_ROTATIONS[SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES] = vec2[]
(
	vec2(-0.635628f, -0.340751f),
	vec2(0.302561f, 0.538286f),
	vec2(0.524376f, 0.577293f),
	vec2(-0.834036f, 0.17623f),
	vec2(-0.128171f, 0.238632f),
	vec2(-0.106862f, -0.428093f),
	vec2(0.0687448f, 0.856102f),
	vec2(-0.24071f, 0.666748f),
	vec2(-0.995048f, -0.227991f),
	vec2(0.862235f, -0.68251f),
	vec2(-0.246354f, -0.731874f),
	vec2(-0.517357f, 0.872618f),
	vec2(0.112809f, -0.268565f),
	vec2(-0.830996f, 0.598792f),
	vec2(0.0902631f, -0.713822f),
	vec2(-0.999803f, 0.5382f),
	vec2(0.942284f, -0.118574f),
	vec2(0.172282f, -0.251799f),
	vec2(-0.540676f, -0.209622f),
	vec2(-0.683223f, -0.970312f),
	vec2(0.638081f, 0.172178f),
	vec2(-0.929329f, -0.674531f),
	vec2(0.185346f, -0.726071f),
	vec2(0.629454f, -0.0489091f),
	vec2(-0.935276f, 0.778103f),
	vec2(0.406472f, 0.853737f),
	vec2(0.375813f, -0.0608274f),
	vec2(0.19583f, -0.334173f),
	vec2(-0.0655024f, -0.750306f),
	vec2(0.498495f, 0.808134f),
	vec2(0.952222f, -0.129607f),
	vec2(-0.56797f, 0.679685f)
);

void main()
{
    //Sample the normal depth texture.
    vec4 normalDepthTextureSampler = texture(normalDepthTexture, fragmentTextureCoordinate);

    //Retrieve the normal.
    vec3 normal = normalDepthTextureSampler.xyz;

    //Convert the normal to view space.
    normal = vec4(perceiverMatrix * vec4(normal, 0.f)).xyz;

    //Retrieve the depth.
    float depth = normalDepthTextureSampler.w;

    //Retrieve the fragment view space position.
    vec3 fragmentViewSpacePosition = CalculateFragmentViewSpacePosition(fragmentTextureCoordinate, depth);

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
        vec3 currentSamplePosition = fragmentViewSpacePosition + (tangentSpaceMatrix * OFFSETS[i]);

        vec4 offset = vec4(currentSamplePosition, 1.0f);
        offset = projectionMatrix * offset;
        offset.xyz /= offset.w;
        offset.xy = offset.xy * 0.5f + 0.5f;

        float currentSampleDepth = CalculateFragmentViewSpacePosition(offset.xy, texture(normalDepthTexture, offset.xy).w).z;

        float fade = mix(0.0f, 1.0f, SmoothStep(abs(fragmentViewSpacePosition.z - currentSampleDepth)));

        occlusion += (currentSampleDepth >= currentSamplePosition.z + SCREEN_SPACE_AMBIENT_OCCLUSION_BIAS ? 1.0f : 0.0f) * fade;
    }

    occlusion = 1.0f - (occlusion / SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES);

    //Modify the value a bit.
    occlusion = pow(occlusion, SCREEN_SPACE_AMBIENT_OCCLUSION_STRENGTH);

    //Write the fragment.
    fragment = vec4(occlusion);
}