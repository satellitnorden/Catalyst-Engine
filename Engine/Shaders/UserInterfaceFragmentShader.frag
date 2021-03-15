//Includes.
#include "CatalystUserInterfaceCore.glsl"

//Constants.
#define USER_INTERFACE_PRIMITIVE_TYPE_IMAGE (0)
#define USER_INTERFACE_PRIMITIVE_TYPE_TEXT (1)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) UserInterfaceMaterial MATERIAL;
    layout (offset = 16) vec4 COLOR;
    layout (offset = 32) vec2 MINIMUM;
    layout (offset = 40) vec2 MAXIMUM;
    layout (offset = 48) uint TYPE;
    layout (offset = 52) float WIDTH_RANGE_START;
    layout (offset = 56) float WIDTH_RANGE_END;
    layout (offset = 60) float PRIMITIVE_ASPECT_RATIO;
    layout (offset = 64) float TEXT_SMOOTHING_FACTOR;
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
		case USER_INTERFACE_PRIMITIVE_TYPE_IMAGE:
		{
			//Write the fragment.
			fragment = EvaluateUserInterfaceMaterial(MATERIAL, texture_coordinate, PRIMITIVE_ASPECT_RATIO) * COLOR;

			break;
		}

		case USER_INTERFACE_PRIMITIVE_TYPE_TEXT:
		{
			//Sample the distance.
			float distance = texture(sampler2D(GLOBAL_TEXTURES[MATERIAL._PrimaryColorTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), texture_coordinate).r;

			//Calculate the opacity.
			float opacity = smoothstep(TEXT_SMOOTHING_FACTOR, 0.5f, distance);

			//Write the fragment.
			fragment = vec4(COLOR.rgb, opacity * COLOR.a);

			break;
		}

		default:
		{
			//Write the fragment.
			fragment = EvaluateUserInterfaceMaterial(MATERIAL, texture_coordinate, PRIMITIVE_ASPECT_RATIO);

			break;
		}
	}
    
}