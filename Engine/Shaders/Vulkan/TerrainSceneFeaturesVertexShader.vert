//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec2 worldPosition;
    layout (offset = 8) float patchSize;
    layout (offset = 12) int heightTextureIndex;
};

//In parameters.
layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in int borders;

//Out parameters.
layout (location = 0) out vec3 fragmentWorldPosition;
layout (location = 1) out vec2 fragmentTextureCoordinate;

void main()
{
	//Write parameters to the fragments shader.
	fragmentWorldPosition = vec3(worldPosition.x, 0.0f, worldPosition.y) + vec3(vertexPosition.x, 0.0f, vertexPosition.y) * patchSize;
	fragmentTextureCoordinate = vertexPosition + 0.5f;

	//Apply the height.
	fragmentWorldPosition.y = texture(sampler2D(globalTextures[heightTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate).x;

	gl_Position = viewMatrix * vec4(fragmentWorldPosition, 1.0f);
}