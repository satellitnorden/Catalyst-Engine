//Includes.
#include "CatalystUserInterfaceCore.glsl"

//Constants.
#define USER_INTERFACE_PRIMITIVE_TYPE_IMAGE (0)
#define USER_INTERFACE_PRIMITIVE_TYPE_TEXT (1)

#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
//User interface uniform data.
layout (std140, set = 1, binding = 0) uniform UserInterfaceUniformData
{
	layout (offset = 0) mat4 TO_WORLD_MATRIX;
	layout (offset = 64) vec3 NORMAL;
	layout (offset = 80) vec2 SCALE;
	layout (offset = 88) float ROUGHNESS;
	layout (offset = 92) float METALLIC;
	layout (offset = 96) float AMBIENT_OCCLUSION;
	layout (offset = 100) float EMISSIVE_MULTIPLIER;
};
#endif

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
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
layout (location = 0) in vec3 fragment_world_position;
layout (location = 1) in vec2 fragment_texture_coordinate;
#else
layout (location = 0) in vec2 fragment_texture_coordinate;
#endif

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

	//Sample the blue noise texture.
	vec4 blue_noise_sample = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), 0);

	switch (TYPE)
	{
		case USER_INTERFACE_PRIMITIVE_TYPE_IMAGE:
		{
			//Write the fragment(s).
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
			vec4 evaluated_material = EvaluateUserInterfaceMaterial(MATERIAL, texture_coordinate, PRIMITIVE_ASPECT_RATIO);
			evaluated_material.rgb = evaluated_material.rgb * evaluated_material.a * COLOR.rgb * COLOR.a;

			scene_features_1 = vec4(evaluated_material.rgb, 1.0f);
			scene_features_2 = vec4(NORMAL, gl_FragCoord.z);
			scene_features_3 = vec4(ROUGHNESS, METALLIC, AMBIENT_OCCLUSION, 1.0f);
			scene_features_4 = vec4(CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, fragment_world_position) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, fragment_world_position), 0.0f, 0.0f);
			scene = vec4(evaluated_material.rgb * EMISSIVE_MULTIPLIER, 1.0f);
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
			float opacity = smoothstep(0.5f - TEXT_SMOOTHING_FACTOR, 0.5f, distance) * COLOR.a;

			//Write the fragment(s).
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
			if (opacity < blue_noise_sample[0])
			{
				discard;
			}
			
			else
			{
				scene_features_1 = vec4(COLOR.rgb * COLOR.a, 1.0f);
				scene_features_2 = vec4(NORMAL, gl_FragCoord.z);
				scene_features_3 = vec4(ROUGHNESS, METALLIC, AMBIENT_OCCLUSION, 1.0f);
				scene_features_4 = vec4(CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, fragment_world_position) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, fragment_world_position), 0.0f, 0.0f);
				scene = vec4(COLOR.rgb * COLOR.a * EMISSIVE_MULTIPLIER, 1.0f);
			}
#else
			fragment = vec4(COLOR.rgb, opacity);
#endif

			break;
		}

		default:
		{
			//Write the fragment(s).
#if defined(THREE_DIMENSIONAL_USER_INTERFACE)
			vec4 evaluated_material = EvaluateUserInterfaceMaterial(MATERIAL, texture_coordinate, PRIMITIVE_ASPECT_RATIO);
			evaluated_material.rgb = evaluated_material.rgb * evaluated_material.a * COLOR.rgb * COLOR.a;

			scene_features_1 = vec4(evaluated_material.rgb, 1.0f);
			scene_features_2 = vec4(NORMAL, gl_FragCoord.z);
			scene_features_3 = vec4(ROUGHNESS, METALLIC, AMBIENT_OCCLUSION, 1.0f);
			scene_features_4 = vec4(CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, fragment_world_position) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, fragment_world_position), 0.0f, 0.0f);
			scene = vec4(evaluated_material.rgb * EMISSIVE_MULTIPLIER, 1.0f);
#else
			fragment = EvaluateUserInterfaceMaterial(MATERIAL, texture_coordinate, PRIMITIVE_ASPECT_RATIO) * COLOR;
#endif

			break;
		}
	}
    
}