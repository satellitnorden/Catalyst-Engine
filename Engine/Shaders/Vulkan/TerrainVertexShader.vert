//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define VERTEX_BORDER_OFFSET (1.0f / (63.0f))

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec2 patchWorldPosition;
	layout (offset = 8) float patchSize;
	layout (offset = 12) int borders;
};

//In parameters.
layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in int vertexBorders;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D heightTexture;

//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;

void main()
{	
	/*
	//Calculate the horizontal border offset multiplier.
	float isUpperMultiplier = float(vertexBorders & (1 << 0));
	float isLowerMultiplier = float(vertexBorders & (1 << 2));
	float isFirstMultipleOfMultiplier = float(mod(gl_VertexIndex, 128) == 0);
	float isSecondMultipleOfMultiplier = float(mod(gl_VertexIndex + 64, 128) == 0);
	float horizontalBorderOffsetWeight = min(isUpperMultiplier + isLowerMultiplier, 1.0f) * (isFirstMultipleOfMultiplier + isSecondMultipleOfMultiplier);

	//Calculate the vertical border offset multiplier.
	float isRightMultiplier = float(vertexBorders & (1 << 1));
	float isLeftMultiplier = float(vertexBorders & (1 << 3));
	float isOddMultiplier = float(gl_VertexIndex & 1);
	float verticalBorderOffsetWeight = min(isRightMultiplier + isLeftMultiplier, 1.0f) * isOddMultiplier;

	vec2 position = vertexPosition;
	position.x -= VERTEX_BORDER_OFFSET * horizontalBorderOffsetWeight;
	position.y -= VERTEX_BORDER_OFFSET * verticalBorderOffsetWeight;
	*/

	vec2 position = vertexPosition;

    //Calculate the texture coordinate.
    fragmentTextureCoordinate = position + 0.5f;

    //Set the position.
    gl_Position = viewMatrix * vec4(vec3(patchWorldPosition.x + (position.x * patchSize), texture(heightTexture, fragmentTextureCoordinate).r, patchWorldPosition.y + (position.y * patchSize)), 1.0f);
}