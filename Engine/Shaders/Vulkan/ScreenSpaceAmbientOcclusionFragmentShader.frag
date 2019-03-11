//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES (32)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_STRENGTH (24.0f) //0.25f step.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_NARROW (6.0f) //0.25f step.

//Layout specification.
layout (early_fragment_tests) in;

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

    //Convert the normal to view space.
    normal = vec4(perceiverMatrix * vec4(normal, 0.f)).xyz;

    //Retrieve the depth.
    float depth = normalDepthTextureSampler.w;

    //Retrieve the fragment view space position.
    vec3 fragmentViewSpacePosition = CalculateFragmentViewSpacePosition(fragmentTextureCoordinate, depth);

    //Calculate the random offset vector.
    vec3 randomRotation = vec3( RandomFloat(vec3(gl_FragCoord.xy, (SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES + 1) * EULERS_NUMBER)) * 2.0f - 1.0f,
                                RandomFloat(vec3(gl_FragCoord.xy, (SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES + 1) * PHI)) * 2.0f - 1.0f,
                                0.0f);

    //Calculate the tangent space matrix.
    vec3 tangent = normalize(randomRotation - normal * dot(randomRotation, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tangentSpaceMatrix = mat3(tangent, bitangent, normal);

    //Calculate the bias.
    float bias = abs(fragmentViewSpacePosition.z) * 0.001f; //0.00025f step.

    //Calculate the occlusion.
    float occlusion = 0.0f;

    for (int i = 0; i < SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES; ++i)
    {
        vec3 randomOffset = normalize(vec3( RandomFloat(vec3(gl_FragCoord.xy, float(i + 1) * EULERS_NUMBER)) * 2.0f - 1.0f,
                                            RandomFloat(vec3(gl_FragCoord.xy, float(i + 1) * PHI)) * 2.0f - 1.0f,
                                            RandomFloat(vec3(gl_FragCoord.xy, float(i + 1) * PI)))) * pow(RandomFloat(vec3(gl_FragCoord.xy, float(i + 1) * SQUARE_ROOT_OF_TWO)), SCREEN_SPACE_AMBIENT_OCCLUSION_NARROW);

        //vec3 currentSamplePosition = fragmentViewSpacePosition + (tangentSpaceMatrix * OFFSETS[i]);
        vec3 currentSamplePosition = fragmentViewSpacePosition + (tangentSpaceMatrix * randomOffset);

        vec4 offset = vec4(currentSamplePosition, 1.0f);
        offset = projectionMatrix * offset;
        offset.xyz /= offset.w;
        offset.xy = offset.xy * 0.5f + 0.5f;

        float currentSampleDepth = CalculateFragmentViewSpacePosition(offset.xy, texture(normalDepthTexture, offset.xy).w).z;

        float fade = 1.0f - min(abs(currentSampleDepth - currentSamplePosition.z), 1.0f);

        occlusion += (currentSampleDepth >= currentSamplePosition.z + bias ? 1.0f : 0.0f) * fade;
    }

    occlusion = 1.0f - (occlusion / SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES);

    //Modify the value a bit.
    occlusion = pow(occlusion, SCREEN_SPACE_AMBIENT_OCCLUSION_STRENGTH);

    //Write the fragment.
    fragment = vec4(occlusion);
}