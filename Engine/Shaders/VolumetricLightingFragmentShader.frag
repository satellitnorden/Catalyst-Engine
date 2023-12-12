//Includes.
#include "CatalystGeometryMath.glsl"
#define CATALYST_LIGHTING_DATA_SET_INDEX (1)
#include "CatalystLightingData.glsl"
#include "..\Include\Rendering\Native\Shader\CatalystLighting.h"
#include "..\Include\Rendering\Native\Shader\CatalystVolumetricLighting.h"

//Constants.
#define NUMBER_OF_DIRECTIONAL_LIGHT_SAMPLES (1)
#define NUMBER_OF_POINT_LIGHT_SAMPLES (1)
#define VOLUMETRIC_SHADOWS_MODE_NONE (0)
#define VOLUMETRIC_SHADOWS_MODE_SCREEN_SPACE (1)

//Layout specification.
layout (early_fragment_tests) in;

//Descriptor set data.
layout (set = 2, binding = 0) uniform sampler2D scene_features_2_texture;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uint VOLUMETRIC_SHADOWS_MODE;
};

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Casts a directional light ray. Returns the amount occluded.
*/
float CastDirectionalLightRay(vec3 start_screen_space_position, vec3 end_screen_space_position, uint current_blue_noise_index)
{
	//Calculate the screen occlusion_factor.
	float screen_occlusion_factor = min(abs(0.5f - end_screen_space_position.x) * 2.0f * abs(0.5f - end_screen_space_position.y) * 2.0f, 1.0f);
	screen_occlusion_factor *= screen_occlusion_factor;

	//Sample the blue noise.
	vec4 blue_noise = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), current_blue_noise_index);

	//Accumulate the occlusion.
	float occlusion = 0.0f;

	//Check the positions.
	for (uint i = 0; i < 4; ++i)
	{
		//Calculate the sample position.
		vec2 sample_position  = mix(start_screen_space_position.xy, end_screen_space_position.xy, (float(i) * 0.25f) + (0.25f * blue_noise[i]));

		//Sample the depth.
		float sample_depth = texture(scene_features_2_texture, sample_position).w;

		//Accumulate the occlusion.
		occlusion += float(sample_depth > 0.0f) * 0.25f;
	}

	//Return the final disocclusion
	return mix(1.0f - occlusion, 0.5f, screen_occlusion_factor);
}

/*
*	Casts a point light ray. Returns the amount occluded.
*/
float CastPointLightRay(vec3 start_screen_space_position, vec3 end_screen_space_position, uint current_blue_noise_index)
{
	//Calculate the screen occlusion_factor.
	float screen_occlusion_factor = min(abs(0.5f - end_screen_space_position.x) * 2.0f * abs(0.5f - end_screen_space_position.y) * 2.0f, 1.0f);
	screen_occlusion_factor *= screen_occlusion_factor;

	//Sample the blue noise.
	vec4 blue_noise = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), current_blue_noise_index);

	//Accumulate the disocclusion.
	float disocclusion = 0.0f;

	//Check the positions.
	for (uint i = 0; i < 4; ++i)
	{
		//Calculate the sample position.
		vec3 sample_position  = mix(start_screen_space_position, end_screen_space_position, (float(i) * 0.25f) + (0.25f * blue_noise[i]));

		//Sample the depth.
		float sample_depth = LinearizeDepth(texture(scene_features_2_texture, sample_position.xy).w);

		//Accumulate the disocclusion.
		disocclusion += float(sample_depth < sample_position.z) * 0.25f;
	}

	//Bias the disocclusion value.
	disocclusion *= disocclusion;

	//Return the final disocclusion
	return mix(disocclusion, 0.5f, screen_occlusion_factor);
}

/*
*	Returns a dithered sample offset.
*/
float GetDitheredSampleOffset(vec4 sample_offsets, uint index)
{
	float actual_sample_offset = sample_offsets[index & 3];
	float dither_sample_offset = sample_offsets[(index + 1) & 3];

	return actual_sample_offset + mix(-0.0039211568627450f, 0.0039211568627450f, dither_sample_offset);
}

void CatalystShaderMain() 
{
	//Load the scene features.
	vec4 scene_features_2 = texture(scene_features_2_texture, fragment_texture_coordinate);

	//Retrieve all properties.
	vec3 world_position = CalculateWorldPosition(fragment_texture_coordinate, scene_features_2.w);
	float hit_distance = length(world_position - CAMERA_WORLD_POSITION);
	float hit_distance_reciprocal = 1.0f / hit_distance;

	//Generate the ray direction.
	vec3 ray_direction = (world_position - CAMERA_WORLD_POSITION) * hit_distance_reciprocal;

	//Calculate the volumetric lighting.
	vec3 volumetric_lighting = vec3(0.0f);

	//Add the ambient lighting.
	volumetric_lighting += CalculateVolumetricAmbientLighting();

	//Remember the current blue noise index.
	uint current_blue_noise_index = 0;

	//Sample the offsets.
	vec4 sample_offsets = SampleBlueNoiseTexture(uvec2(gl_FragCoord.xy), current_blue_noise_index++);

	//Calculate the volumetric lighting for all volumetric lights.
	uint current_sample_offset_index = 0;

	for (uint i = 0; i < NUMBER_OF_LIGHTS; ++i)
	{
		//Unpack the light.
		Light light = UnpackLight(i);

		if (TEST_BIT(light.light_properties, LIGHT_PROPERTY_VOLUMETRIC_BIT))
		{
			switch (light.light_type)
			{
				case LIGHT_TYPE_DIRECTIONAL:
				{
					//Calculate the light world position.
					vec3 light_world_position = CAMERA_WORLD_POSITION + light.position_or_direction * VIEW_DISTANCE;

					//Calculate the light screen space position.
					vec3 light_screen_space_position;

					{
						vec4 light_view_space_position = WORLD_TO_CLIP_MATRIX * vec4(light_world_position, 1.0f);
						float light_view_space_coefficient_reciprocal = 1.0f / light_view_space_position.w;
						light_view_space_position.xyz *= light_view_space_coefficient_reciprocal;

						light_screen_space_position.xy = light_view_space_position.xy * 0.5f + 0.5f;
						light_screen_space_position.z = LinearizeDepth(light_view_space_position.z);
					}

					//Calculate the screen factor.
					float screen_factor = min(abs(light_screen_space_position.x - 0.5f) * abs(light_screen_space_position.y - 0.5f), 1.0f);
					screen_factor *= screen_factor;
					screen_factor = 1.0f - screen_factor;
					screen_factor = dot(light.position_or_direction, CAMERA_FORWARD_VECTOR) <= 0.0f ? screen_factor : 0.0f;

					for (uint sample_index = 0; sample_index < NUMBER_OF_DIRECTIONAL_LIGHT_SAMPLES; ++sample_index)
					{
						//Calculate the volumetric particle hit distance.
						float volumetric_particle_hit_distance = min(hit_distance, VOLUMETRIC_LIGHTING_DISTANCE) * GetDitheredSampleOffset(sample_offsets, current_sample_offset_index++);

						//Calculate the volumetric particle hit position.
						vec3 volumetric_particle_hit_position = CAMERA_WORLD_POSITION + ray_direction * volumetric_particle_hit_distance;

						//Calculate the volumetric particle screen space position.
						vec3 volumetric_particle_screen_space_position;

						{
							vec4 volumetric_particle_view_space_position = WORLD_TO_CLIP_MATRIX * vec4(volumetric_particle_hit_position, 1.0f);
							float volumetric_particle_view_space_coefficient_reciprocal = 1.0f / volumetric_particle_view_space_position.w;
							volumetric_particle_view_space_position.xyz *= volumetric_particle_view_space_coefficient_reciprocal;

							volumetric_particle_screen_space_position.xy = volumetric_particle_view_space_position.xy * 0.5f + 0.5f;
							volumetric_particle_screen_space_position.z = LinearizeDepth(volumetric_particle_view_space_position.z);
						}

						//Determine the visibility.
						float hit_factor = 1.0f;

						if (VOLUMETRIC_SHADOWS_MODE != VOLUMETRIC_SHADOWS_MODE_NONE
							&& TEST_BIT(light.light_properties, LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT))
						{
							hit_factor = mix(1.0f, CastDirectionalLightRay(volumetric_particle_screen_space_position, light_screen_space_position, current_blue_noise_index++), screen_factor);
						}
						
						//Add to the volumetric lighting.
						volumetric_lighting += VOLUMETRIC_LIGHTING_BASE_COLOR * light.color * light.intensity * exp(-VOLUMETRIC_LIGHTING_THICKNESS * 0.5f) * hit_factor / float(NUMBER_OF_DIRECTIONAL_LIGHT_SAMPLES);
					}

					break;
				}

				case LIGHT_TYPE_POINT:
				{
					//Do a ray/sphere intersection to determine the volumetric particle hit distance.
					Ray ray;

					ray._Origin = CAMERA_WORLD_POSITION;
					ray._Direction = ray_direction;

					Sphere sphere;

					sphere._Position = light.position_or_direction;
					sphere._Radius = light.radius;

					RaySphereIntersectionResult result = RaySphereIntersection(ray, sphere);

					if (result._Result != RAY_SPHERE_INTERSECTION_RESULT_NO_HIT)
					{
						//Calculate the light world position.
						vec3 light_world_position = light.position_or_direction;

						//Calculate the light screen space position.
						vec3 light_screen_space_position;

						{
							vec4 light_view_space_position = WORLD_TO_CLIP_MATRIX * vec4(light_world_position, 1.0f);
							float light_view_space_coefficient_reciprocal = 1.0f / light_view_space_position.w;
							light_view_space_position.xyz *= light_view_space_coefficient_reciprocal;

							light_screen_space_position.xy = light_view_space_position.xy * 0.5f + 0.5f;
							light_screen_space_position.z = LinearizeDepth(light_view_space_position.z);
						}


						for (uint sample_index = 0; sample_index < NUMBER_OF_POINT_LIGHT_SAMPLES; ++sample_index)
						{
							float volumetric_particle_hit_distance;

							switch (result._Result)
							{
								case RAY_SPHERE_INTERSECTION_RESULT_ONE_HIT:
								{
									volumetric_particle_hit_distance = mix(0.0f, min(result._ClosestHitDistance, hit_distance), GetDitheredSampleOffset(sample_offsets, current_sample_offset_index++));

									break;
								}

								case RAY_SPHERE_INTERSECTION_RESULT_TWO_HITS:
								{
									volumetric_particle_hit_distance = mix(min(result._ClosestHitDistance, hit_distance), min(result._FarthestHitDistance, hit_distance), GetDitheredSampleOffset(sample_offsets, current_sample_offset_index++));
									
									break;
								}
							}

							//Calculate the volumetric particle hit position.
							vec3 volumetric_particle_hit_position = ray._Origin + ray._Direction * volumetric_particle_hit_distance;

							//Calculate the distance to the light.
							float distance_to_light = length(light.position_or_direction - volumetric_particle_hit_position);

							//Calculate the reciprocal of the distance to light.
							float distance_to_light_reciprocal = 1.0f / distance_to_light;

							//Calculate the attenuation.
							float attenuation = CalculateAttenuation(distance_to_light, light.radius);

							//Calculate the volumetric particle screen space position.
							vec3 volumetric_particle_screen_space_position;

							{
								vec4 volumetric_particle_view_space_position = WORLD_TO_CLIP_MATRIX * vec4(volumetric_particle_hit_position, 1.0f);
								float volumetric_particle_view_space_coefficient_reciprocal = 1.0f / volumetric_particle_view_space_position.w;
								volumetric_particle_view_space_position.xyz *= volumetric_particle_view_space_coefficient_reciprocal;

								volumetric_particle_screen_space_position.xy = volumetric_particle_view_space_position.xy * 0.5f + 0.5f;
								volumetric_particle_screen_space_position.z = LinearizeDepth(volumetric_particle_view_space_position.z);
							}

							//Determine the visibility.
							float hit_factor = 1.0f;

							if (VOLUMETRIC_SHADOWS_MODE != VOLUMETRIC_SHADOWS_MODE_NONE
								&& TEST_BIT(light.light_properties, LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT))
							{
								hit_factor = CastPointLightRay(volumetric_particle_screen_space_position, light_screen_space_position, current_blue_noise_index++);
							}

							//Calculate the light to volumetric particle opacity.
							float light_to_volumetric_particle_opacity = CalculateVolumetricLightingOpacity(distance_to_light, VOLUMETRIC_LIGHTING_DISTANCE, light.position_or_direction.y, VOLUMETRIC_LIGHTING_HEIGHT, VOLUMETRIC_LIGHTING_THICKNESS, volumetric_particle_hit_position.y);

							//Calculate the volumetric particle to camera opacity.
							float volumetric_particle_to_camera_opacity = CalculateVolumetricLightingOpacity(volumetric_particle_hit_distance, VOLUMETRIC_LIGHTING_DISTANCE, volumetric_particle_hit_position.y, VOLUMETRIC_LIGHTING_HEIGHT, VOLUMETRIC_LIGHTING_THICKNESS, CAMERA_ABSOLUTE_HEIGHT);

							//Add to the volumetric lighting.
							volumetric_lighting += VOLUMETRIC_LIGHTING_BASE_COLOR * light.color * light.intensity * attenuation * (1.0f - light_to_volumetric_particle_opacity) * (1.0f - volumetric_particle_to_camera_opacity) * hit_factor / float(NUMBER_OF_POINT_LIGHT_SAMPLES);
						}
					}

					break;
				}
			}
		}
	}

	//Write the fragment.
	fragment = vec4(volumetric_lighting, 1.0f);
}