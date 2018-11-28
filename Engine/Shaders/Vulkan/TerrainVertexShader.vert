//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define VERTEX_BORDER_OFFSET_FIRST (1.0f / (64.0f))
#define VERTEX_BORDER_OFFSET_SECOND (1.0f / (32.0f))

//In parameters.
layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in int vertexBorders;

//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;
layout (location = 1) out flat int patchIndex;

void main()
{	
	//Determine the patch index.
	patchIndex = gl_InstanceIndex;

	//Set the position.
	vec2 position = vertexPosition;

	//Calculate the horizontal border offset multiplier.
	float isUpperMultiplier = float((vertexBorders & BIT(0)) & (terrainPatchData[patchIndex].borders & BIT(0)));
	float isLowerMultiplier = float((vertexBorders & BIT(4)) & (terrainPatchData[patchIndex].borders & BIT(4)));
	float horizontalBorderOffsetWeight = min(isUpperMultiplier + isLowerMultiplier, 1.0f);

	//Calculate the vertical border offset multiplier.
	float isRightMultiplier = float((vertexBorders & BIT(2)) & (terrainPatchData[patchIndex].borders & BIT(2)));
	float isLeftMultiplier = float((vertexBorders & BIT(6)) & (terrainPatchData[patchIndex].borders & BIT(6)));
	float verticalBorderOffsetWeight = min(isRightMultiplier + isLeftMultiplier, 1.0f);

	position.x -= VERTEX_BORDER_OFFSET_FIRST * horizontalBorderOffsetWeight;
	position.y -= VERTEX_BORDER_OFFSET_FIRST * verticalBorderOffsetWeight;

	//Calculate the horizontal border offset multiplier.
	isUpperMultiplier = float((vertexBorders & BIT(1)) & (terrainPatchData[patchIndex].borders & BIT(1)));
	isLowerMultiplier = float((vertexBorders & BIT(5)) & (terrainPatchData[patchIndex].borders & BIT(5)));
	horizontalBorderOffsetWeight = min(isUpperMultiplier + isLowerMultiplier, 1.0f);

	//Calculate the vertical border offset multiplier.
	isRightMultiplier = float((vertexBorders & BIT(3)) & (terrainPatchData[patchIndex].borders & BIT(3)));
	isLeftMultiplier = float((vertexBorders & BIT(7)) & (terrainPatchData[patchIndex].borders & BIT(7)));
	verticalBorderOffsetWeight = min(isRightMultiplier + isLeftMultiplier, 1.0f);

	position.x -= VERTEX_BORDER_OFFSET_SECOND * horizontalBorderOffsetWeight;
	position.y -= VERTEX_BORDER_OFFSET_SECOND * verticalBorderOffsetWeight;

    //Calculate the texture coordinate.
    fragmentTextureCoordinate = position + 0.5f;

    //Set the position.
    gl_Position = viewMatrix * vec4(vec3(terrainPatchData[patchIndex].worldPosition.x + (position.x * terrainPatchData[patchIndex].patchSize), texture(terrainHeightTextures[terrainPatchData[patchIndex].heightTextureIndex], fragmentTextureCoordinate).r, terrainPatchData[patchIndex].worldPosition.y + (position.y * terrainPatchData[patchIndex].patchSize)), 1.0f);
}