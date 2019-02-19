//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 color;
    layout (offset = 16) vec3 worldPosition;
    layout (offset = 32) float attenuationDistance;
    layout (offset = 36) float intensity;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D albedoTexture;
layout (set = 1, binding = 1) uniform sampler2D normalDirectionDepthTexture;
layout (set = 1, binding = 2) uniform sampler2D roughnessMetallicAmbientOcclusionTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Write the fragment.
    fragment = vec4(color, 1.0f);
}