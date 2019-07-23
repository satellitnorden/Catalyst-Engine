//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 16) int normalTextureIndex;
};

//In parameters.
layout (location = 0) in vec3 fragmentWorldPosition;
layout (location = 1) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;
layout (location = 3) out vec4 sceneFeatures4;
layout (location = 4) out vec4 velocity;

void main()
{
    //Retrieve the terrain normal.
    vec3 terrainNormal = normalize(texture(sampler2D(globalTextures[normalTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate).xyz * 2.0f - 1.0f);

    //Write the fragments.
    sceneFeatures1 = vec4(pow(vec3(0.0f, 1.0f, 0.0f), vec3(2.2f)), 0.0f);
    sceneFeatures2 = vec4(vec3(0.0f, 1.0f, 0.0f), length(fragmentWorldPosition - perceiverWorldPosition));
    sceneFeatures3 = vec4(vec3(0.0f, 1.0f, 0.0f), 0.0f);
    sceneFeatures4 = vec4(1.0f, 0.0f, 1.0f, 0.0f);
    velocity = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}