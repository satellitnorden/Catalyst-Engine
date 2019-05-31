//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Constants.
#define TEMPORAL_ANTI_ALIASING_SAMPLES (4)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D previousFrameTextures[TEMPORAL_ANTI_ALIASING_SAMPLES];
layout (set = 1, binding = 1) uniform sampler2D sceneTexture;

//Out parameters.
layout (location = 0) out vec4 previousFrame;
layout (location = 1) out vec4 scene;

void main()
{
    //Just sum up all previous frames.
    vec4 previousFrames = vec4(0.0f);

    for (int i = 0; i < TEMPORAL_ANTI_ALIASING_SAMPLES; ++i)
    {
    	previousFrames += texture(previousFrameTextures[i], fragmentTextureCoordinate);	
    }

    previousFrames /= TEMPORAL_ANTI_ALIASING_SAMPLES;

    //Write the fragments.
    previousFrame = texture(sceneTexture, fragmentTextureCoordinate);
    scene = previousFrames;
}