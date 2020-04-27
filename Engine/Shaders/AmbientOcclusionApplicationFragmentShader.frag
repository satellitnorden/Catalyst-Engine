//Defines.
#define BIAS_AMBIENT_OCCLUSION(X) (X * X * X * X)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_features_3_texture;
layout (set = 1, binding = 1) uniform sampler2D ambient_occlusion_texture;

//Out parameters.
layout (location = 0) out vec4 scene_features_3;

void CatalystShaderMain()
{
	//Sample the scene features 3 texture.
	vec4 scene_features_3_texture_sampler = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_3_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);

	//Sample the ambient occlusion texture.
	vec4 ambient_occlusion_texture_sampler = texture(sampler2D(RENDER_TARGETS[AMBIENT_OCCLUSION_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), fragment_texture_coordinate);

	//Write the fragment.
	scene_features_3 = vec4(scene_features_3_texture_sampler.x,
							scene_features_3_texture_sampler.y,
							scene_features_3_texture_sampler.z * BIAS_AMBIENT_OCCLUSION(ambient_occlusion_texture_sampler.x),
							scene_features_3_texture_sampler.w);
}