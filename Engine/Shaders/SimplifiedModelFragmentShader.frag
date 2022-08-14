//Includes.
#define CATALYST_LIGHTING_DATA_SET_INDEX (1)
#include "CatalystLightingData.glsl"
#include "CatalystMaterialCore.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystLighting.h"
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
layout (location = 0) in mat3 fragment_tangent_space_matrix;
layout (location = 3) in vec3 fragment_world_position;
layout (location = 4) in vec2 fragment_texture_coordinate;

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

	//Calculate the shading normal.
	vec3 shading_normal = normal_map_displacement.xyz * 2.0f - 1.0f;
	shading_normal = fragment_tangent_space_matrix * shading_normal;
	shading_normal = shading_normal;

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
		vec3 diffuse_indirect_lighting_direction = shading_normal;
		vec3 specular_indirect_lighting_direction = reflect(view_direction, shading_normal);

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
		vec3 indirect_lighting = SampleSky(indirect_lighting_direction, MAX_SKY_TEXTURE_MIPMAP_LEVEL * diffuse_component);

		indirect_lighting *= mix(0.125f, 8.0f, diffuse_component);

		final_lighting += CalculateSimplifiedLighting(	-view_direction,
														albedo_thickness.rgb,
														shading_normal,
														material_properties[0],
														material_properties[1],
														mix(1.0f, material_properties[2], diffuse_component),
														1.0f,
														-indirect_lighting_direction,
														indirect_lighting);

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
					final_lighting += CalculateSimplifiedLighting(	-view_direction,
																	albedo_thickness.rgb,
																	shading_normal,
																	material_properties[0],
																	material_properties[1],
																	material_properties[2],
																	albedo_thickness.a,
																	light.position_or_direction,
																	light.color * light.intensity);

					break;
				}

				case LIGHT_TYPE_POINT:
				{
					//Calculate the light direction.
					vec3 light_direction = fragment_world_position - light.position_or_direction;

					//Determine the distance to the light.
					float distance_to_light = LengthSquared3(light_direction);

					//Only calculate lighting if the the world position is within the light's radius.
					if (distance_to_light < light.radius * light.radius)
					{
						//Normalize.
						distance_to_light = sqrt(distance_to_light);
						light_direction /= distance_to_light;

						//Calculate the attenuation.
						float attenuation = CalculateAttenuation(distance_to_light, light.radius);

						final_lighting += albedo_thickness.rgb * light.color * light.intensity * max(dot(shading_normal, -light_direction), 0.0f) * attenuation;
					}

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