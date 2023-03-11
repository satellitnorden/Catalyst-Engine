//Includes.
#include "CatalystModelCore.glsl"
#include "CatalystWindCore.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) mat4 WORLD_TO_LIGHT_MATRIX;
    layout (offset = 64) vec3 WORLD_GRID_DELTA;
    layout (offset = 80) uint MODEL_FLAGS;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texture_coordinate;
layout (location = 4) in mat4 vertex_transformation;

void CatalystShaderMain()
{
	//Calculate the world position.
	vec3 world_position = vec3(vertex_transformation * vec4(vertex_position, 1.0f)) + WORLD_GRID_DELTA;

	//Apply model flags, if necessary.
	if (TEST_BIT(MODEL_FLAGS, MODEL_FLAG_IS_VEGETATION))
	{
		//Calculate the normal.
		vec3 normal = normalize(vec3(vertex_transformation * vec4(vertex_normal, 0.0f)));

		//Apply the wind displacement.
		world_position += CalculateWindDisplacement(vertex_transformation[3].xyz, world_position, normal, 1.0f, 1.0f, 1.0f) * vertex_position.y;
	}

	gl_Position = WORLD_TO_LIGHT_MATRIX * vec4(world_position, 1.0f);
}