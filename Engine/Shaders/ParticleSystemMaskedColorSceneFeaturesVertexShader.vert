//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_velocity;
layout (location = 2) in vec2 vertex_scale;
layout (location = 3) in float vertex_opacity;

//Out parameters.
layout (location = 0) out vec3 geometry_position;
layout (location = 1) out vec3 geometry_velocity;
layout (location = 2) out vec2 geometry_scale;

void CatalystShaderMain()
{
	//Nothing to do here except pass data along to the geometry shader.
	geometry_position = vertex_position;
	geometry_velocity = vertex_velocity;
	geometry_scale = vertex_scale;
}