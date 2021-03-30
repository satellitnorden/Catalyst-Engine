//Out parameters.
layout (location = 0) out uint geometry_index;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uvec2 RESOLUTION;
	layout (offset = 8) vec2 RESOLUTION_RECIPROCAL;
	layout (offset = 16) float DISTANCE_BETWEEN_BLADES;
};

void CatalystShaderMain()
{
	//Nothing to do here except pass data along to the geometry shader.
	geometry_index = gl_VertexIndex;
}