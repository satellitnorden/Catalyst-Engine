//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) int rendering_reference_texture_index;
	layout (offset = 4) float iterations;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Sample the rendering reference texture.
	vec4 rendering_reference_texture = texture(sampler2D(GLOBAL_TEXTURES[rendering_reference_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);

	//Write the fragment.
	fragment = vec4(rendering_reference_texture.rgb / iterations, 1.0f);
}