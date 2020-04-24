//In parameters.
layout (location = 0) in mat4 transformation;

//Out parameters.
layout (location = 0) out vec3 geometry_position;

void CatalystShaderMain()
{
	//Nothing to do here except pass the geometry_position.
	geometry_position = transformation[3].xyz;
}