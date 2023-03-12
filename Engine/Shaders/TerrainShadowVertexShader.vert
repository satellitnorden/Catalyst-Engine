//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) mat4 WORLD_TO_LIGHT_MATRIX;
    layout (offset = 64) vec3 WORLD_POSITION;
    layout (offset = 68) uint INDEX_MAP_TEXTURE_INDEX;
    layout (offset = 72) uint BLEND_MAP_TEXTURE_INDEX;
    layout (offset = 76) float MATERIAL_MAPS_RESOLUTION;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texture_coordinate;

void CatalystShaderMain()
{
	//Write the position.
	vec4 clip_position = WORLD_TO_LIGHT_MATRIX * vec4(WORLD_POSITION + vertex_position, 1.0f);
	clip_position.z = max(clip_position.z, FLOAT32_EPSILON);
	gl_Position = clip_position;
}