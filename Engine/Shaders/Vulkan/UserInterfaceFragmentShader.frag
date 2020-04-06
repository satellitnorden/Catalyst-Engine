//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Constants.
#define USER_INTERFACE_ELEMENT_TYPE_BUTTON (0)
#define USER_INTERFACE_ELEMENT_TYPE_IMAGE (1)
#define USER_INTERFACE_ELEMENT_TYPE_TEXT (2)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 16) uint type;
    layout (offset = 20) uint texture_index;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	switch (type)
	{
		case USER_INTERFACE_ELEMENT_TYPE_BUTTON:
		{
			//Write the fragment.
			fragment = texture(sampler2D(GLOBAL_TEXTURES[texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);

			break;
		}

		case USER_INTERFACE_ELEMENT_TYPE_IMAGE:
		{
			//Write the fragment.
			fragment = texture(sampler2D(GLOBAL_TEXTURES[texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);

			break;
		}

		case USER_INTERFACE_ELEMENT_TYPE_TEXT:
		{
			//Write the fragment.
			fragment = vec4(vec3(1.0f), texture(sampler2D(GLOBAL_TEXTURES[texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate).r);

			break;
		}

		default:
		{
			//Write the fragment.
			fragment = texture(sampler2D(GLOBAL_TEXTURES[texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);

			break;
		}
	}
    
}