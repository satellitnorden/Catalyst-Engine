//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_BIAS 0.00001f
#define SCREEN_SPACE_AMBIENT_OCCLUSION_RADIUS 0.25f
#define SCREEN_SPACE_AMBIENT_OCCLUSION_RADIUS_SQUARED SCREEN_SPACE_AMBIENT_OCCLUSION_RADIUS * SCREEN_SPACE_AMBIENT_OCCLUSION_RADIUS
#define SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES 32

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Post processing data.
layout (std140, set = 1, binding = 0) uniform ScreenSpaceAmbientOcclusionUniformData
{
    vec4 samples[SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES];
};

//Texture samplers.
layout (set = 1, binding = 1) uniform sampler2D normalDepthTexture;
layout (set = 1, binding = 2) uniform sampler2D noiseTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

//Push constant
layout (push_constant) uniform ScreenSpaceAmbientOcclusionData
{
    vec2 noiseScale;
};

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

    //Calculate the TBN matrix.
    vec3 randomVector = normalize(texture(noiseTexture, fragmentTextureCoordinate * noiseScale).xyz);
    vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    //Calculate the occlusion factor.
    float occlusion = 0.0f;

    for (int i = 0; i < SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES; ++i)
    {
        vec3 currentSample = TBN * samples[i].xyz;
        currentSample = fragmentWorldPosition + currentSample * SCREEN_SPACE_AMBIENT_OCCLUSION_RADIUS;

        vec4 offset = vec4(currentSample, 1.0f);
        offset = viewMatrix * offset;
        offset.xyz /= offset.w;
        offset.xy = offset.xy * 0.5f + 0.5f;

        float sampleDepth = texture(normalDepthTexture, offset.xy).w;
        vec3 actualSamplePosition = CalculateFragmentWorldPosition(offset.xy, sampleDepth);

        float rangeCheck = smoothstep(0.0f, 1.0f, SCREEN_SPACE_AMBIENT_OCCLUSION_RADIUS_SQUARED / LengthSquared3(currentSample - actualSamplePosition));
        occlusion += (offset.z >= sampleDepth + SCREEN_SPACE_AMBIENT_OCCLUSION_BIAS ? 1.0f : 0.0f) * rangeCheck;    
    }

    occlusion = 1.0f - (occlusion / SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES);

    //Modify the value a bit.
    occlusion = pow(occlusion, 5.0f);

    //Write the fragment.
    fragment = vec4(occlusion);
}