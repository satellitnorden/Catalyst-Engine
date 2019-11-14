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

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Retrieve the scene features.
	vec4 sceneFeatures1 = texture(sceneFeatures1Texture, fragmentTextureCoordinate);
	vec4 sceneFeatures2 = texture(sceneFeatures2Texture, fragmentTextureCoordinate);
	vec4 sceneFeatures3 = texture(sceneFeatures3Texture, fragmentTextureCoordinate);

	//Retrieve all properties.
	Material material = GLOBAL_MATERIALS[int(sceneFeatures1.w * 255.0f)];
	vec3 albedo = sceneFeatures1.rgb;
	float hitDistance = sceneFeatures2.w;
	vec3 shadingNormal = UnpackNormal(sceneFeatures2.x);
	float roughness = sceneFeatures3.x;
	float metallic = sceneFeatures3.y;
	float ambientOcclusion = sceneFeatures3.z;

	//Generate the ray direction.
	vec3 rayDirection = CalculateRayDirection(fragmentTextureCoordinate);

	//Calculate the hit position.
	vec3 hitPosition = perceiverWorldPosition + rayDirection * hitDistance;

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
				//Sample the cloud density.
				float cloud_density = SampleCloudDensityInDirection(hitPosition, -light.position_or_direction, 2);

				directLighting += CalculateDirectLight(	-rayDirection,
														-light.position_or_direction,
														albedo,
														shadingNormal,
														roughness,
														metallic,
														material.thickness,
														light.luminance) * (1.0f - cloud_density);

				break;
			}

			case LIGHT_TYPE_POINT:
			{
				float lengthToLight = length(light.position_or_direction - hitPosition);
				vec3 lightDirection = vec3(light.position_or_direction - hitPosition) / lengthToLight;

				//Calculate the attenuation.
				float attenuation = 1.0f / (1.0f + lengthToLight + (lengthToLight * lengthToLight));

				directLighting += CalculateDirectLight(	-rayDirection,
														lightDirection,
														albedo,
														shadingNormal,
														roughness,
														metallic,
														material.thickness,
														light.luminance) * attenuation;

				break;
			}
		}
	}

	//Write the fragment.
	fragment = vec4(directLighting, 1.0f);
}