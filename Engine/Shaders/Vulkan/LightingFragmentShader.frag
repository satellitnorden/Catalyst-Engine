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

//Descriptor set data.
layout (set = 1, binding = 0) uniform sampler2D diffuseIrradianceTexture;
layout (set = 1, binding = 1) uniform sampler2D specularIrradianceTexture;
layout (set = 1, binding = 2) uniform sampler2D sceneFeatures1Texture;
layout (set = 1, binding = 3) uniform sampler2D sceneFeatures2Texture;
layout (set = 1, binding = 4) uniform sampler2D sceneFeatures3Texture;

//Out parameters.
layout (location = 0) out vec4 scene;

void main()
{
	//Sample the normal and hit distance.
	vec4 sceneFeatures2TextureSampler = texture(sceneFeatures2Texture, fragmentTextureCoordinate);
	vec3 normal = UnpackNormal(sceneFeatures2TextureSampler.y);
	float hitDistance = sceneFeatures2TextureSampler.z;

	//Only calculate the lighting if there was a hit.
	if (hitDistance < CATALYST_RAY_TRACING_T_MAXIMUM)
	{
		//Sample the diffuse irradiance.
		vec3 diffuseIrradiance = texture(diffuseIrradianceTexture, fragmentTextureCoordinate).rgb;

		//Sample the specular irradiance.
		vec3 specularIrradiance = texture(specularIrradianceTexture, fragmentTextureCoordinate).rgb;

		//Sample the albedo.
		vec3 albedo = texture(sceneFeatures1Texture, fragmentTextureCoordinate).rgb;

		//Sample the roughness and metallic.
		vec4 sceneFeatures3TextureSampler = texture(sceneFeatures3Texture, fragmentTextureCoordinate);
		float roughness = sceneFeatures3TextureSampler.x;
		float metallic = sceneFeatures3TextureSampler.y;
		float ambientOcclusion = sceneFeatures3TextureSampler.z;
		float emissive = sceneFeatures3TextureSampler.w;

		//Calculate the world position.
		vec3 worldPosition = perceiverWorldPosition + CalculateRayDirection(fragmentTextureCoordinate) * hitDistance;

		//Calculate the view direction.
		vec3 viewDirection = normalize(perceiverWorldPosition - worldPosition);

		//Calculate the composited lighting.
		vec3 compositedLighting = vec3(0.0f);

		//Add the emissive lighting.
		compositedLighting += albedo * emissive;

		//Add the highlight.
		compositedLighting += CalculateHighlight(viewDirection, normal, floatBitsToInt(sceneFeatures2TextureSampler.w));

		//Add the indirect lighting.
		compositedLighting += CalculateIndirectLighting(viewDirection,
														albedo,
														normal,
														roughness,
														metallic,
														ambientOcclusion,
														diffuseIrradiance,
														specularIrradiance);

		/*
		//Add the directional light direct lighting result.
		float directionalLightVisibility = texture(directionalLightVisibilityTexture, fragmentTextureCoordinate).x;

		compositedLighting += CalculateDirectLight(	viewDirection,
													-directionalLightDirection,
													albedo,
													normal,
													roughness,
													metallic,
													directionalLightColor * directionalLightIntensity) * directionalLightVisibility;
		*/

		//Add the lights direct lighting results.
		for (int i = 0; i < numberOfLights; ++i)
		{
			Light light = UnpackLight(i);

			float lengthToLight = length(light.position - worldPosition);
			vec3 lightDirection = vec3(light.position - worldPosition) / lengthToLight;

			//Calculate the attenuation distance.
			float attenuation = 1.0f / (1.0f + lengthToLight + (lengthToLight * lengthToLight));

			//Unpack the visibility.
			float lightVisibility = texture(lightsVisibilityTextures[i], fragmentTextureCoordinate).x;

			//Calculate the lighting.
			compositedLighting += CalculateDirectLight(	viewDirection,
														lightDirection,
														albedo,
														normal,
														roughness,
														metallic,
														light.color * light.strength) * attenuation * lightVisibility;
		}
		
		//Write the fragment.
		scene = vec4(compositedLighting, 1.0f);
	}
	
	//If there wasn't a hit, then the sky color is stored in the specular irradiance texture.
	else
	{
		//Write the fragment.
		scene = vec4(texture(specularIrradianceTexture, fragmentTextureCoordinate).rgb, 1.0f);
	}
}