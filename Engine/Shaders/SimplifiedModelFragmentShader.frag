//Includes.
#define CATALYST_LIGHTING_DATA_SET_INDEX (1)
#include "CatalystLightingData.glsl"
#include "CatalystMaterialCore.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystToneMapping.h"
#include "..\Include\Rendering\Native\Shader\CatalystVolumetricLighting.h"

layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) mat4 MODEL_MATRIX;
    layout (offset = 64) vec3 SKY_LIGHT_LUMINANCE;
    layout (offset = 80) uint MATERIAL_INDEX;
};

//In parameters.
layout (location = 0) in vec3 fragment_world_position;
layout (location = 1) in vec3 fragment_normal;
layout (location = 2) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 scene;

void CatalystShaderMain()
{
   	//Retrieve the material.
	Material material = GLOBAL_MATERIALS[MATERIAL_INDEX];

	//Evaluate the material.
	vec4 albedo_thickness;
	vec4 normal_map_displacement;
	vec4 material_properties;
	vec4 opacity;

	EvaluateMaterial(material, fragment_texture_coordinate, albedo_thickness, normal_map_displacement, material_properties, opacity);

	//Calculate the hit distance.
	float hit_distance = length(fragment_world_position - PERCEIVER_WORLD_POSITION);

	//Calculate the hit distance reciprocal.
	float hit_distance_reciprocal = 1.0f / hit_distance;

	//Calculate the view direction.
	vec3 view_direction = (fragment_world_position - PERCEIVER_WORLD_POSITION) * hit_distance_reciprocal;

	//Calculate the diffuse component.
	float diffuse_component = material_properties[0] * (1.0f - material_properties[1]);

	//Calculate the indirect lighting direction.
	vec3 indirect_lighting_direction;

	{
		vec3 diffuse_indirect_lighting_direction = fragment_normal;
		vec3 specular_indirect_lighting_direction = reflect(view_direction, fragment_normal);

		indirect_lighting_direction = normalize(mix(specular_indirect_lighting_direction, diffuse_indirect_lighting_direction, diffuse_component));
	}

	//Pre-calculate the volumetric lighting.
	vec3 volumetric_lighting = vec3(0.0f);

	{
		//Add the volumetric ambient lighting.
		volumetric_lighting += CalculateVolumetricAmbientLighting();

		//Add the sky lighting.
		volumetric_lighting += VOLUMETRIC_LIGHTING_BASE_COLOR * SKY_LIGHT_LUMINANCE * VOLUMETRIC_LIGHTING_DENSITY_MULTIPLIER;
	}

	//Calculate the final lighting.
	vec3 final_lighting = vec3(0.0f);

	//Add indirect lighting.
	{
		//Calculate the indirect lighting.
		vec3 indirect_lighting = SampleSky(indirect_lighting_direction, MAX_SKY_TEXTURE_MIPMAP_LEVEL * diffuse_component) * material_properties[2];

		//Blend the indirect lighting with volumetric lighting.
		{
			//Calculate the volumetric lighting opacity.
			float volumetric_lighting_opacity = CalculateVolumetricLightingOpacity(VIEW_DISTANCE, VOLUMETRIC_LIGHTING_DISTANCE, vec3(fragment_world_position + indirect_lighting_direction * VIEW_DISTANCE).y, VOLUMETRIC_LIGHTING_HEIGHT, VOLUMETRIC_LIGHTING_THICKNESS, fragment_world_position.y);

			//Blend the volumetric lighting with the indirect lighting.
			indirect_lighting = mix(indirect_lighting, volumetric_lighting * 0.125f, volumetric_lighting_opacity * 0.5f);
		}

		//Add the indirect lighting to the final lighting.
		final_lighting += indirect_lighting;
	}

	//Add direct lighting.
	{
		//Add direct lighting from all the lights.
		for (uint light_index = 0; light_index < NUMBER_OF_LIGHTS; ++light_index)
		{
			Light light = UnpackLight(light_index);

			switch (light.light_type)
			{
				case LIGHT_TYPE_DIRECTIONAL:
				{
					final_lighting += albedo_thickness.rgb * light.color * light.intensity * max(dot(fragment_normal, -light.position_or_direction), 0.0f);

					break;
				}

				case LIGHT_TYPE_POINT:
				{
					//To do.

					break;
				}
			}
		}
	}

	//Add emissive lighting.
	final_lighting += albedo_thickness.rgb * material_properties[3] * material._EmissiveMultiplier;

	//Add volumetric lighting.
	{
		//Calculate the volumetric lighting opacity.
		float volumetric_lighting_opacity = CalculateVolumetricLightingOpacity(hit_distance, VOLUMETRIC_LIGHTING_DISTANCE, fragment_world_position.y, VOLUMETRIC_LIGHTING_HEIGHT, VOLUMETRIC_LIGHTING_THICKNESS, PERCEIVER_WORLD_POSITION.y);

		//Blend the volumetric lighting with the final lighting.
		final_lighting = mix(final_lighting, volumetric_lighting, volumetric_lighting_opacity);
	}

	//Apply tone mapping.
	final_lighting = ApplyToneMapping(final_lighting);

	//Write the fragment.
	scene = vec4(final_lighting, 1.0f);
}