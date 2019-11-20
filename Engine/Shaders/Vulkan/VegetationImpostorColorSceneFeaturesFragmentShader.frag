//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 8) int material_index;
};

//In parameters.
layout (location = 0) in vec3 fragment_normal;
layout (location = 1) in vec2 fragment_texture_coordinate;
layout (location = 2) in float fragment_hit_distance;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;

void main()
{
	//Retrieve the material.
  	Material material = GLOBAL_MATERIALS[material_index];

	//Sample the albedo.
  	vec3 albedo = texture(sampler2D(globalTextures[material.albedo_texture_index], globalSamplers[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragment_texture_coordinate).rgb;

    //Write the fragments.
    sceneFeatures1 = vec4(pow(albedo, vec3(2.2f)), float(material_index) / 255.0f);
    sceneFeatures2 = vec4(PackNormal(fragment_normal), 0.0f, 0.0f, fragment_hit_distance);
    sceneFeatures3 = vec4(1.0f, 0.0f, 1.0f, 0.0f);
}