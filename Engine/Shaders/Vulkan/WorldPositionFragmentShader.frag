//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D normalDepthTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	 //Sample the depth of the scene at this point.
    float sceneDepth = texture(normalDepthTexture, fragmentTextureCoordinate).w;

    //Calculate the scene world position.
    vec3 sceneWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, sceneDepth);

    //Write the fragment.
    fragment = vec4(sceneWorldPosition, sceneDepth == 0.0f ? 1.0f : 0.0f);
}