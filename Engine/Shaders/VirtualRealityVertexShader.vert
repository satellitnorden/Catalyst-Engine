//Includes.
#define VIRTUAL_REALITY_UNIFORM_DATA_SET_INDEX (1)
#include "CatalystVirtualRealityData.glsl"

//Enable extensions.
#extension GL_EXT_multiview : enable

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) mat4 MODEL_MATRIX;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texture_coordinate;

//Out parameters.
layout (location = 0) out vec3 fragment_world_position;
layout (location = 1) out vec3 fragment_normal;
layout (location = 2) out vec2 fragment_texture_coordinate;

void CatalystShaderMain()
{
	//Pass data to the fragment shader.
	fragment_world_position = vec3(MODEL_MATRIX * vec4(vertex_position, 1.0f));
	fragment_normal = normalize(vec3(MODEL_MATRIX * vec4(vertex_normal, 0.0f)));
	fragment_texture_coordinate = vertex_texture_coordinate;

	//Write the position.
	gl_Position = EYE_WORLD_TO_CLIP_MATRICES[gl_ViewIndex] * vec4(fragment_world_position, 1.0f);
} 