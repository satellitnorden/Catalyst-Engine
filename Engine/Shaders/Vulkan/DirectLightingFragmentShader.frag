//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystLightingData.glsl"
#include "CatalystModelData.glsl"
#include "CatalystPackingUtilities.glsl"
#include "CatalystRayTracingCore.glsl"
#include "CatalystRenderingUtilities.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 3, binding = 0) uniform sampler2D sceneFeatures1Texture;
layout (set = 3, binding = 1) uniform sampler2D sceneFeatures2Texture;
layout (set = 3, binding = 2) uniform sampler2D sceneFeatures3Texture;
layout (set = 3, binding = 3) uniform sampler2D ambient_occlusion_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Retrieve the scene features.
	vec4 sceneFeatures1 = texture(sceneFeatures1Texture, fragmentTextureCoordinate);
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, fragmentTextureCoordinate);
	vec4 sceneFeatures3 = texture(sceneFeatures3Texture, fragmentTextureCoordinate);
	vec4 ambient_occlusion = ambientOcclusionMode == AMBIENT_OCCLUSION_MODE_NONE ? vec4(1.0f) : Upsample(ambient_occlusion_texture, fragmentTextureCoordinate);

	//Retrieve all properties.
	Material material = GLOBAL_MATERIALS[int(sceneFeatures1.w * 255.0f)];
	vec3 albedo = sceneFeatures1.rgb;
	float depth = sceneFeatures2.w;
	vec3 shadingNormal = UnpackNormal(sceneFeatures2.x);
	float roughness = sceneFeatures3.x;
	float metallic = sceneFeatures3.y;
	float ambientOcclusion = pow(sceneFeatures3.z * pow(ambient_occlusion.x, AMBIENT_OCCLUSION_POWER), AMBIENT_OCCLUSION_POWER);

	//Calculate the hit position.
	vec3 world_position = CalculateWorldPosition(fragmentTextureCoordinate, depth);

	//Generate the view direction.
	vec3 view_direction = normalize(world_position - PERCEIVER_WORLD_POSITION);

	//Calculate the direct lighting.
	vec3 directLighting = vec3(0.0f);

	//Calculate all lights.
	for (int i = 0; i < numberOfLights; ++i)
	{
		Light light = UnpackLight(i);

		switch (light.type)
		{
			case LIGHT_TYPE_DIRECTIONAL:
			{
				directLighting += CalculateDirectLight(	-view_direction,
														-light.position_or_direction,
														albedo,
														shadingNormal,
														roughness,
														metallic,
														ambientOcclusion,
														material.thickness,
														light.luminance) * (1.0f - CLOUD_DENSITY);

				break;
			}

			case LIGHT_TYPE_POINT:
			{
				float lengthToLight = length(light.position_or_direction - world_position);
				vec3 lightDirection = vec3(light.position_or_direction - world_position) / lengthToLight;

				//Calculate the attenuation.
				float attenuation = 1.0f / (1.0f + lengthToLight + (lengthToLight * lengthToLight));

				directLighting += CalculateDirectLight(	-view_direction,
														lightDirection,
														albedo,
														shadingNormal,
														roughness,
														metallic,
														ambientOcclusion,
														material.thickness,
														light.luminance) * attenuation;

				break;
			}
		}
	}

	//Write the fragment.
	fragment = vec4(directLighting, 1.0f);
}