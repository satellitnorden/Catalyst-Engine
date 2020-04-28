//Includes.
#include "CatalystUserInterfaceCore.glsl"

//Constants.
#define USER_INTERFACE_ELEMENT_TYPE_BUTTON (0)
#define USER_INTERFACE_ELEMENT_TYPE_IMAGE (1)
#define USER_INTERFACE_ELEMENT_TYPE_TEXT (2)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 16) uint TYPE;
    layout (offset = 20) float WIDTH_RANGE_START;
    layout (offset = 24) float WIDTH_RANGE_END;
    layout (offset = 28) float ELEMENT_ASPECT_RATIO;
    layout (offset = 32) UserInterfaceMaterial MATERIAL;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	//Calculate the texture coordinate.
	vec2 texture_coordinate = vec2(mix(WIDTH_RANGE_START, WIDTH_RANGE_END, fragment_texture_coordinate.x), fragment_texture_coordinate.y);

	switch (TYPE)
	{
		case USER_INTERFACE_ELEMENT_TYPE_BUTTON:
		case USER_INTERFACE_ELEMENT_TYPE_IMAGE:
		{
			//Write the fragment.
			fragment = EvaluateUserInterfaceMaterial(MATERIAL, texture_coordinate, ELEMENT_ASPECT_RATIO);

			break;
		}

		case USER_INTERFACE_ELEMENT_TYPE_TEXT:
		{
			//Write the fragment.
			fragment = vec4(vec3(1.0f), texture(sampler2D(GLOBAL_TEXTURES[MATERIAL._PrimaryColorTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), texture_coordinate).r);

			break;
		}

		default:
		{
			//Write the fragment.
			fragment = EvaluateUserInterfaceMaterial(MATERIAL, texture_coordinate, ELEMENT_ASPECT_RATIO);

			break;
		}
	}
    
}