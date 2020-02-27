//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

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

void main()
{
	//Sample the scene features 3 texture.
	vec4 scene_features_3_texture_sampler = texture(scene_features_3_texture, fragment_texture_coordinate);

	//Sample the ambient occlusion texture.
	vec4 ambient_occlusion_texture_sampler = Upsample(ambient_occlusion_texture, fragment_texture_coordinate);

	//Write the fragment.
	scene_features_3 = vec4(scene_features_3_texture_sampler.x,
							scene_features_3_texture_sampler.y,
							scene_features_3_texture_sampler.z * BIAS_AMBIENT_OCCLUSION(ambient_occlusion_texture_sampler.x),
							scene_features_3_texture_sampler.w);
}