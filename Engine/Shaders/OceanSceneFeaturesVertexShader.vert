//Includes.
#include "CatalystOceanCore.glsl"

//Constants.
#define VERTEX_BORDER_OFFSET_FIRST (1.0f / (64.0f))
#define VERTEX_BORDER_OFFSET_SECOND (1.0f / (32.0f))

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec2 world_position;
    layout (offset = 8) int borders;
    layout (offset = 12) float patch_size;
    layout (offset = 16) int ocean_texture_index;
};

//In parameters.
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in int vertex_borders;

//Out parameters.
layout (location = 0) out vec3 fragment_world_position;

void CatalystShaderMain()
{
	//Set the position.
	vec2 position = vertex_position.xy;

	//Calculate the horizontal border offset multiplier.
	float isUpperMultiplier = float((vertex_borders & BIT(0)) & (borders & BIT(0)));
	float isLowerMultiplier = float((vertex_borders & BIT(4)) & (borders & BIT(4)));
	float horizontalBorderOffsetWeight = min(isUpperMultiplier + isLowerMultiplier, 1.0f);

	//Calculate the vertical border offset multiplier.
	float isRightMultiplier = float((vertex_borders & BIT(2)) & (borders & BIT(2)));
	float isLeftMultiplier = float((vertex_borders & BIT(6)) & (borders & BIT(6)));
	float verticalBorderOffsetWeight = min(isRightMultiplier + isLeftMultiplier, 1.0f);

	position.x -= VERTEX_BORDER_OFFSET_FIRST * horizontalBorderOffsetWeight;
	position.y -= VERTEX_BORDER_OFFSET_FIRST * verticalBorderOffsetWeight;

	//Calculate the horizontal border offset multiplier.
	isUpperMultiplier = float((vertex_borders & BIT(1)) & (borders & BIT(1)));
	isLowerMultiplier = float((vertex_borders & BIT(5)) & (borders & BIT(5)));
	horizontalBorderOffsetWeight = min(isUpperMultiplier + isLowerMultiplier, 1.0f);

	//Calculate the vertical border offset multiplier.
	isRightMultiplier = float((vertex_borders & BIT(3)) & (borders & BIT(3)));
	isLeftMultiplier = float((vertex_borders & BIT(7)) & (borders & BIT(7)));
	verticalBorderOffsetWeight = min(isRightMultiplier + isLeftMultiplier, 1.0f);

	position.x -= VERTEX_BORDER_OFFSET_SECOND * horizontalBorderOffsetWeight;
	position.y -= VERTEX_BORDER_OFFSET_SECOND * verticalBorderOffsetWeight;

	//Write parameters to the fragment shader.
	fragment_world_position = vec3(world_position.x, 0.0f, world_position.y) + vec3(position.x, 0.0f, position.y) * patch_size;

	//Apply the height.
	fragment_world_position.y += SampleOceanHeight(fragment_world_position, ocean_texture_index);

	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(fragment_world_position, 1.0f);
	
}