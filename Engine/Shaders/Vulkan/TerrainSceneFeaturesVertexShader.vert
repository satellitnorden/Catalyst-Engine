//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Constants.
#define VERTEX_BORDER_OFFSET_FIRST (1.0f / (64.0f))
#define VERTEX_BORDER_OFFSET_SECOND (1.0f / (32.0f))

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec2 worldPosition;
    layout (offset = 8) float patchSize;
    layout (offset = 12) int borders;
    layout (offset = 16) int heightTextureIndex;
};

//In parameters.
layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in int vertexBorders;

//Out parameters.
layout (location = 0) out vec3 fragmentWorldPosition;
layout (location = 1) out vec2 fragmentTextureCoordinate;

void main()
{
	//Set the position.
	vec2 position = vertexPosition;

	//Calculate the horizontal border offset multiplier.
	float isUpperMultiplier = float((vertexBorders & BIT(0)) & (borders & BIT(0)));
	float isLowerMultiplier = float((vertexBorders & BIT(4)) & (borders & BIT(4)));
	float horizontalBorderOffsetWeight = min(isUpperMultiplier + isLowerMultiplier, 1.0f);

	//Calculate the vertical border offset multiplier.
	float isRightMultiplier = float((vertexBorders & BIT(2)) & (borders & BIT(2)));
	float isLeftMultiplier = float((vertexBorders & BIT(6)) & (borders & BIT(6)));
	float verticalBorderOffsetWeight = min(isRightMultiplier + isLeftMultiplier, 1.0f);

	position.x -= VERTEX_BORDER_OFFSET_FIRST * horizontalBorderOffsetWeight;
	position.y -= VERTEX_BORDER_OFFSET_FIRST * verticalBorderOffsetWeight;

	//Calculate the horizontal border offset multiplier.
	isUpperMultiplier = float((vertexBorders & BIT(1)) & (borders & BIT(1)));
	isLowerMultiplier = float((vertexBorders & BIT(5)) & (borders & BIT(5)));
	horizontalBorderOffsetWeight = min(isUpperMultiplier + isLowerMultiplier, 1.0f);

	//Calculate the vertical border offset multiplier.
	isRightMultiplier = float((vertexBorders & BIT(3)) & (borders & BIT(3)));
	isLeftMultiplier = float((vertexBorders & BIT(7)) & (borders & BIT(7)));
	verticalBorderOffsetWeight = min(isRightMultiplier + isLeftMultiplier, 1.0f);

	position.x -= VERTEX_BORDER_OFFSET_SECOND * horizontalBorderOffsetWeight;
	position.y -= VERTEX_BORDER_OFFSET_SECOND * verticalBorderOffsetWeight;

	//Write parameters to the fragment shader.
	fragmentWorldPosition = vec3(worldPosition.x, 0.0f, worldPosition.y) + vec3(position.x, 0.0f, position.y) * patchSize;
	fragmentTextureCoordinate = position + 0.5f;

	//Apply the height.
	fragmentWorldPosition.y = texture(sampler2D(globalTextures[heightTextureIndex], globalSamplers[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragmentTextureCoordinate).x;

	gl_Position = viewMatrix * vec4(fragmentWorldPosition, 1.0f);
}