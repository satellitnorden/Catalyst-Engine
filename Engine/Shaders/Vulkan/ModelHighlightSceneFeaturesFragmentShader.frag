//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"

layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 64) vec3 highlight_color;
    layout (offset = 80) float highlight_strength;
    layout (offset = 84) int material_index;
};

//In parameters.
layout (location = 0) in mat3 fragment_tangent_space_matrix;
layout (location = 3) in vec3 fragment_current_world_position;
layout (location = 4) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 scene_features_1;
layout (location = 1) out vec4 scene_features_3;

void main()
{
   	//Retrieve the material.
	Material material = GLOBAL_MATERIALS[material_index];

	//Sample the albedo.
	vec3 albedo = RetrieveAlbedo(material, fragment_texture_coordinate);

	//Calculate the shading normal.
	vec3 shading_normal;

	if (bool(material.properties & MATERIAL_PROPERTY_NO_NORMAL_MAP_TEXTURE_BIT))
	{
		shading_normal = fragment_tangent_space_matrix[2];
	}

	else
	{
	 	//Sample the normal map.
	 	vec3 normal_map = texture(sampler2D(GLOBAL_TEXTURES[material.normal_map_texture_index], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragment_texture_coordinate).xyz;
	 	shading_normal = normal_map * 2.0f - 1.0f;
	 	shading_normal = fragment_tangent_space_matrix * shading_normal;
	 	shading_normal = normalize(shading_normal);
	}

	//Sample the material properties.
	vec4 material_properties = RetrieveMaterialProperties(material, fragment_texture_coordinate);

	//Calculate the view direction.
	vec3 view_direction = normalize(fragment_current_world_position - PERCEIVER_WORLD_POSITION);

	//Calculate the highlight weight.
	float highlight_weight = (1.0f - max(dot(shading_normal, -view_direction), 0.0f)) * highlight_strength;

	//Apply the highlight properties.
	albedo = mix(albedo, highlight_color, highlight_weight);
	material_properties = mix(material_properties, vec4(material_properties.x, material_properties.y, material_properties.z, 1.0f), highlight_weight);

    //Write the fragments.
    scene_features_1 = vec4(albedo, float(material_index) / 255.0f);
    scene_features_3 = material_properties;
}