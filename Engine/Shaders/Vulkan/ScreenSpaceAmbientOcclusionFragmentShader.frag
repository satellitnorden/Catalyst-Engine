//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_BIAS (0.000001f)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES (8)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_STRENGTH (16.0f)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec4 offsets[SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES];
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D normalDepthTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

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
    vec3 randomOffsetVector = vec3( RandomFloat(vec3(gl_FragCoord.xy, depth * PI)) * 2.0f - 1.0f,
                                    RandomFloat(vec3(gl_FragCoord.xy, depth * PHI)) * 2.0f - 1.0f,
                                    0.0f);

    //Calculate the tangent space matrix.
    vec3 tangent = normalize(randomOffsetVector - normal * dot(randomOffsetVector, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tangentSpaceMatrix = mat3(tangent, bitangent, normal);

    //Calculate the occlusion.
    float occlusion = 0.0f;

    for (int i = 0; i < SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES; ++i)
    {
        vec3 currentSamplePosition = fragmentWorldPosition + (tangentSpaceMatrix * offsets[i].xyz) * offsets[i].w;

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