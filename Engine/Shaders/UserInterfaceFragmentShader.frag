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
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
	layout (offset = 0) mat4 TO_WORLD_MATRIX;
	layout (offset = 64) UserInterfaceMaterial MATERIAL;
    layout (offset = 80) vec4 COLOR;
    layout (offset = 96) vec2 MINIMUM;
    layout (offset = 104) vec2 MAXIMUM;
    layout (offset = 112) uint TYPE;
    layout (offset = 116) float WIDTH_RANGE_START;
    layout (offset = 120) float WIDTH_RANGE_END;
    layout (offset = 124) float PRIMITIVE_ASPECT_RATIO;
#else
    layout (offset = 0) UserInterfaceMaterial MATERIAL;
    layout (offset = 16) vec4 COLOR;
    layout (offset = 32) vec2 MINIMUM;
    layout (offset = 40) vec2 MAXIMUM;
    layout (offset = 48) uint TYPE;
    layout (offset = 52) float WIDTH_RANGE_START;
    layout (offset = 56) float WIDTH_RANGE_END;
    layout (offset = 60) float PRIMITIVE_ASPECT_RATIO;
    layout (offset = 64) float TEXT_SMOOTHING_FACTOR;
#endif
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
layout (location = 0) out vec4 scene_features_1;
layout (location = 1) out vec4 scene_features_2;
layout (location = 2) out vec4 scene_features_3;
layout (location = 3) out vec4 scene_features_4;
layout (location = 4) out vec4 scene;
#else
layout (location = 0) out vec4 fragment;
#endif

void CatalystShaderMain()
{
	//Calculate the texture coordinate.
	vec2 texture_coordinate = vec2(mix(WIDTH_RANGE_START, WIDTH_RANGE_END, fragment_texture_coordinate.x), fragment_texture_coordinate.y);

	switch (TYPE)
	{
		case USER_INTERFACE_PRIMITIVE_TYPE_IMAGE:
		{
			//Write the fragment(s).
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
			scene_features_1 = vec4(vec3(EvaluateUserInterfaceMaterial(MATERIAL, texture_coordinate, PRIMITIVE_ASPECT_RATIO) * COLOR), 1.0f);
			scene_features_2 = vec4(-PERCEIVER_FORWARD_VECTOR, gl_FragCoord.z);
			scene_features_3 = vec4(1.0f, 0.0f, 1.0f, 0.0f);
			scene_features_4 = vec4(0.0f, 0.0f, 0.0f, 0.0f);
			scene = vec4(0.0f, 0.0f, 0.0f, 1.0f);
#else
			fragment = EvaluateUserInterfaceMaterial(MATERIAL, texture_coordinate, PRIMITIVE_ASPECT_RATIO) * COLOR;
#endif

			break;
		}

		case USER_INTERFACE_PRIMITIVE_TYPE_TEXT:
		{
			//Sample the distance.
			float distance = texture(sampler2D(GLOBAL_TEXTURES[MATERIAL._PrimaryColorTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), texture_coordinate).r;

			//Calculate the opacity.
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
			float opacity = float(distance >= 0.5f);
#else
			float opacity = smoothstep(TEXT_SMOOTHING_FACTOR, 0.5f, distance);
#endif

			//Write the fragment(s).
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
			scene_features_1 = vec4(COLOR.rgb, 1.0f);
			scene_features_2 = vec4(-PERCEIVER_FORWARD_VECTOR, gl_FragCoord.z);
			scene_features_3 = vec4(1.0f, 0.0f, 1.0f, 0.0f);
			scene_features_4 = vec4(0.0f, 0.0f, 0.0f, 0.0f);
			scene = vec4(0.0f, 0.0f, 0.0f, 1.0f);
#else
			fragment = vec4(COLOR.rgb, opacity * COLOR.a);
#endif

			break;
		}

		default:
		{
			//Write the fragment(s).
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
			scene_features_1 = vec4(vec3(EvaluateUserInterfaceMaterial(MATERIAL, texture_coordinate, PRIMITIVE_ASPECT_RATIO) * COLOR), 1.0f);
			scene_features_2 = vec4(-PERCEIVER_FORWARD_VECTOR, gl_FragCoord.z);
			scene_features_3 = vec4(1.0f, 0.0f, 1.0f, 0.0f);
			scene_features_4 = vec4(0.0f, 0.0f, 0.0f, 0.0f);
			scene = vec4(0.0f, 0.0f, 0.0f, 1.0f);
#else
			fragment = EvaluateUserInterfaceMaterial(MATERIAL, texture_coordinate, PRIMITIVE_ASPECT_RATIO) * COLOR;
#endif

			break;
		}
	}
    
}