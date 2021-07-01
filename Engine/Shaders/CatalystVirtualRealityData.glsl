#if !defined(CATALYST_VIRTUAL_REALITY_DATA)
#define CATALYST_VIRTUAL_REALITY_DATA

//Virtual reality uniform data.
layout (std140, set = VIRTUAL_REALITY_UNIFORM_DATA_SET_INDEX, binding = 0) uniform VIRTUAL_REALITY_UNIFORM_DATA_BUFFER
{
	layout (offset = 0) mat4 EYE_WORLD_TO_CLIP_MATRICES[2];
};

#endif