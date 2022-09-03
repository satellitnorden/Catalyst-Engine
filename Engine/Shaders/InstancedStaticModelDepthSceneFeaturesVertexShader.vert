//Includes.
#include "CatalystModelCore.glsl"
#include "CatalystVegetationCore.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 WORLD_GRID_DELTA;
    layout (offset = 16) uint MODEL_FLAGS;
    layout (offset = 20) float START_FADE_OUT_DISTANCE_SQUARED;
    layout (offset = 24) float END_FADE_OUT_DISTANCE_SQUARED;
    layout (offset = 28) uint MATERIAL_INDEX;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texture_coordinate;
layout (location = 4) in mat4 vertex_transformation;

//Out parameters.
layout (location = 0) out vec2 fragment_texture_coordinate;
layout (location = 1) out float fragment_fade_out_opacity;

void CatalystShaderMain()
{
	//Pass data to the fragment shader.
	fragment_texture_coordinate = vertex_texture_coordinate;

	//Calculate the world position.
	vec3 world_position = vec3(vertex_transformation * vec4(vertex_position, 1.0f)) + WORLD_GRID_DELTA;

	//Calculate the normal.
	vec3 normal = normalize(vec3(vertex_transformation * vec4(vertex_normal, 0.0f)));

	if (START_FADE_OUT_DISTANCE_SQUARED < FLOAT32_MAXIMUM)
	{
		fragment_fade_out_opacity = 1.0f - (clamp(LengthSquared3(world_position - CAMERA_WORLD_POSITION) - START_FADE_OUT_DISTANCE_SQUARED, 0.0f, END_FADE_OUT_DISTANCE_SQUARED - START_FADE_OUT_DISTANCE_SQUARED) / (END_FADE_OUT_DISTANCE_SQUARED - START_FADE_OUT_DISTANCE_SQUARED));
	}

	else
	{
		fragment_fade_out_opacity = 1.0f;
	}

	//Apply model flags, if necessary.
	if (TEST_BIT(MODEL_FLAGS, MODEL_FLAG_IS_VEGETATION))
	{
		//Apply the wind displacement.
		world_position += CalculateWindDisplacement(vertex_transformation[3].xyz, world_position, normal, 0.5f, 0.5f, 0.5f) * vertex_position.y;
	}

	//Write the position!
	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(world_position, 1.0f);
}