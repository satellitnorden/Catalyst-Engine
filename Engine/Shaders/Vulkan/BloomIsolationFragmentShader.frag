//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D scene_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Sample the scene texture.
	vec3 scene = texture(scene_texture, fragment_texture_coordinate).rgb;

	//Calculate the luminance of this fragment.
	float luminance = CalculateAverage(scene);

    //Write the fragment.
    fragment = vec4(scene * luminance * bloomIntensity, 1.0f);
}