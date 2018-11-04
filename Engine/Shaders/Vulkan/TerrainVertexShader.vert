//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 patchWorldPosition;
	layout (offset = 8) float patchSize;
};

//In parameters.
layout (location = 0) in vec2 vertexPosition;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D heightTexture;

//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;

void main()
{	
    //Calculate the texture coordinate.
    fragmentTextureCoordinate = vertexPosition + 0.5f;

    //Set the position.
    gl_Position = viewMatrix * vec4(vec3(patchWorldPosition.x + (vertexPosition.x * patchSize), texture(heightTexture, fragmentTextureCoordinate).r, patchWorldPosition.y + (vertexPosition.y * patchSize)), 1.0f);
}