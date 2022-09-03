//Includes.
#include "CatalystRayTracingCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float DEPTH_OF_FIELD_FOCUS_DISTANCE;
	layout (offset = 4) float DEPTH_OF_FIELD_SIZE;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D SCENE_FEATURES_2_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	//Sample the depth of field.
	vec3 depth_of_field = texture(sampler2D(RENDER_TARGETS[INTERMEDIATE_RGBA_FLOAT32_QUARTER_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate).rgb;

	//Calculate the view distance.
	float view_distance = -(CalculateViewSpacePosition(fragment_texture_coordinate, texture(SCENE_FEATURES_2_TEXTURE, fragment_texture_coordinate).w).z);

	//Calculate the depth of field weight.
	float depth_of_field_weight = min(abs(view_distance - DEPTH_OF_FIELD_FOCUS_DISTANCE) / DEPTH_OF_FIELD_FOCUS_DISTANCE, 1.0f);

    //Write the fragment.
    fragment = vec4(depth_of_field, smoothstep(0.1f, 0.2f, depth_of_field_weight));
}