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
	layout (offset = 8) float patchHalfSize;
	layout (offset = 12) float patchInverseSize;
};

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec4 vertexLayerWeights;

//Out parameters.
layout (location = 0) out vec4 fragmentLayerWeights;
layout (location = 1) out vec2 fragmentTextureCoordinate;

void main()
{	
	//Pass information to the fragment shader.
    fragmentLayerWeights = vertexLayerWeights;

    //Calculate the texture coordinate.
    fragmentTextureCoordinate = vec2(vertexPosition.xz - patchWorldPosition + patchHalfSize) * patchInverseSize;

    //Set the position.
    gl_Position = viewMatrix * vec4(vertexPosition, 1.0f);
}