//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define BLEND_CONSTANT (1000.0f)

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D normalDepthTexture;
layout (set = 1, binding = 1) uniform sampler2D layerWeightsTexture;
layout (set = 2, binding = 0) uniform sampler2D layer1AlbedoTexture;
layout (set = 2, binding = 1) uniform sampler2D layer1NormalMapTexture;
layout (set = 2, binding = 2) uniform sampler2D layer1MaterialPropertiesTexture;
layout (set = 2, binding = 3) uniform sampler2D layer2AlbedoTexture;
layout (set = 2, binding = 4) uniform sampler2D layer2NormalMapTexture;
layout (set = 2, binding = 5) uniform sampler2D layer2MaterialPropertiesTexture;
layout (set = 2, binding = 6) uniform sampler2D layer3AlbedoTexture;
layout (set = 2, binding = 7) uniform sampler2D layer3NormalMapTexture;
layout (set = 2, binding = 8) uniform sampler2D layer3MaterialPropertiesTexture;
layout (set = 2, binding = 9) uniform sampler2D layer4AlbedoTexture;
layout (set = 2, binding = 10) uniform sampler2D layer4NormalMapTexture;
layout (set = 2, binding = 11) uniform sampler2D layer4MaterialPropertiesTexture;
layout (set = 2, binding = 12) uniform sampler2D layer5AlbedoTexture;
layout (set = 2, binding = 13) uniform sampler2D layer5NormalMapTexture;
layout (set = 2, binding = 14) uniform sampler2D layer5MaterialPropertiesTexture;

//Out parameters.
layout (location = 0) out vec4 albedoColor;
layout (location = 1) out vec4 normalDirectionDepth;
layout (location = 2) out vec4 materialProperties;

//Globals.
vec4 layer1MaterialPropertiesSampler;
vec4 layer2MaterialPropertiesSampler;
vec4 layer3MaterialPropertiesSampler;
vec4 layer4MaterialPropertiesSampler;
vec4 layer5MaterialPropertiesSampler;

vec3 absoluteNormal;
vec2 textureCoordinateYZ;
vec2 textureCoordinateXZ;
vec2 textureCoordinateXY;
vec2 finalTextureCoordinate;

vec3 fragmentWorldPosition;
vec3 fragmentWorldNormal;
vec4 fragmentLayerWeights;

float distanceToCameraSquared;
float blendSmoothing;

/*
*	Blends two terrain values.
*/
vec3 Blend(vec3 first, float firstHeight, vec3 second, float secondHeight, float alpha)
{
	float firstWeight = (1.0f - alpha) * firstHeight;
	float secondWeight = alpha * secondHeight;

    float total = 1.0f / (firstWeight + secondWeight);

    firstWeight *= total;
    secondWeight *= total;

    return mix(firstWeight > secondWeight ? first : second, mix(first, second, alpha), 0.25f);
}

/*
*	Blends two terrain values.
*/
float Blend(float first, float firstHeight, float second, float secondHeight, float alpha)
{
	float firstWeight = (1.0f - alpha) * firstHeight;
	float secondWeight = alpha * secondHeight;

    float total = 1.0f / (firstWeight + secondWeight);

    firstWeight *= total;
    secondWeight *= total;

    return mix(firstWeight > secondWeight ? first : second, mix(first, second, alpha), 0.25f);
}

/*
*	Calculates the tri-planar data.
*/
void CalculateTriPlanarData(float depth)
{
	//Calculate the absolute normal.
	absoluteNormal = abs(fragmentWorldNormal);
    float normalSum = absoluteNormal.x + absoluteNormal.y + absoluteNormal.z;
    float inverseNormalSum = 1.0f / normalSum;
    absoluteNormal *= inverseNormalSum;

	//Calculate the texture coordinates on the three planes.
	textureCoordinateYZ = fragmentWorldPosition.yz * 0.25f;
	textureCoordinateXZ = fragmentWorldPosition.xz * 0.25f;
	textureCoordinateXY = fragmentWorldPosition.xy * 0.25f;
}

/*
*	Samples a texture using tri-planar mapping.
*/
vec4 SampleTriPlanar(sampler2D textureSampler)
{
    vec4 sampleYZ = texture(textureSampler, textureCoordinateYZ);
    vec4 sampleXZ = texture(textureSampler, textureCoordinateXZ);
    vec4 sampleXY = texture(textureSampler, textureCoordinateXY);

	return sampleYZ * absoluteNormal.x + sampleXZ * absoluteNormal.y + sampleYZ * absoluteNormal.z;
}

/*
*	Returns the albedo.
*/
vec3 GetAlbedo()
{
    vec3 layer1Albedo = SampleTriPlanar(layer1AlbedoTexture).rgb;
    vec3 layer2Albedo = SampleTriPlanar(layer2AlbedoTexture).rgb;
    vec3 layer3Albedo = SampleTriPlanar(layer3AlbedoTexture).rgb;
    vec3 layer4Albedo = SampleTriPlanar(layer4AlbedoTexture).rgb;
    vec3 layer5Albedo = SampleTriPlanar(layer5AlbedoTexture).rgb;

    vec3 blend1 = Blend(layer1Albedo, layer1MaterialPropertiesSampler.w, layer2Albedo, layer2MaterialPropertiesSampler.w, fragmentLayerWeights.x);
    vec3 blend2 = Blend(blend1, layer2MaterialPropertiesSampler.w, layer3Albedo, layer3MaterialPropertiesSampler.w, fragmentLayerWeights.y);
    vec3 blend3 = Blend(blend2, layer3MaterialPropertiesSampler.w, layer4Albedo, layer4MaterialPropertiesSampler.w, fragmentLayerWeights.z);

    return Blend(blend3, layer4MaterialPropertiesSampler.w, layer5Albedo, layer5MaterialPropertiesSampler.w, fragmentLayerWeights.w);
}

/*
*	Returns the normal direction.
*/
vec3 GetNormalDirection()
{
	vec3 layer1NormalDirection = SampleTriPlanar(layer1NormalMapTexture).rgb;
    vec3 layer2NormalDirection = SampleTriPlanar(layer2NormalMapTexture).rgb;
    vec3 layer3NormalDirection = SampleTriPlanar(layer3NormalMapTexture).rgb;
    vec3 layer4NormalDirection = SampleTriPlanar(layer4NormalMapTexture).rgb;
    vec3 layer5NormalDirection = SampleTriPlanar(layer5NormalMapTexture).rgb;

    vec3 blend1 = Blend(layer1NormalDirection, layer1MaterialPropertiesSampler.w, layer2NormalDirection, layer2MaterialPropertiesSampler.w, fragmentLayerWeights.x);
    vec3 blend2 = Blend(blend1, layer2MaterialPropertiesSampler.w, layer3NormalDirection, layer3MaterialPropertiesSampler.w, fragmentLayerWeights.y);
    vec3 blend3 = Blend(blend2, layer3MaterialPropertiesSampler.w, layer4NormalDirection, layer4MaterialPropertiesSampler.w, fragmentLayerWeights.z);

    return Blend(blend3, layer4MaterialPropertiesSampler.w, layer5NormalDirection, layer5MaterialPropertiesSampler.w, fragmentLayerWeights.w) * 2.0f - 1.0f;
}

/*
*	Returns the roughness.
*/
float GetRoughness()
{
	float layer1Roughness = layer1MaterialPropertiesSampler.r;
    float layer2Roughness = layer2MaterialPropertiesSampler.r;
    float layer3Roughness = layer3MaterialPropertiesSampler.r;
    float layer4Roughness = layer4MaterialPropertiesSampler.r;
    float layer5Roughness = layer5MaterialPropertiesSampler.r;

    float blend1 = Blend(layer1Roughness, layer1MaterialPropertiesSampler.w, layer2Roughness, layer2MaterialPropertiesSampler.w, fragmentLayerWeights.x);
    float blend2 = Blend(blend1, layer2MaterialPropertiesSampler.w, layer3Roughness, layer3MaterialPropertiesSampler.w, fragmentLayerWeights.y);
    float blend3 = Blend(blend2, layer3MaterialPropertiesSampler.w, layer4Roughness, layer4MaterialPropertiesSampler.w, fragmentLayerWeights.z);

    return Blend(blend3, layer4MaterialPropertiesSampler.w, layer5Roughness, layer5MaterialPropertiesSampler.w, fragmentLayerWeights.w);
}

/*
*	Returns the metallic.
*/
float GetMetallic()
{
	float layer1Metallic = layer1MaterialPropertiesSampler.y;
    float layer2Metallic = layer2MaterialPropertiesSampler.y;
    float layer3Metallic = layer3MaterialPropertiesSampler.y;
    float layer4Metallic = layer4MaterialPropertiesSampler.y;
    float layer5Metallic = layer5MaterialPropertiesSampler.y;

    float blend1 = Blend(layer1Metallic, layer1MaterialPropertiesSampler.w, layer2Metallic, layer2MaterialPropertiesSampler.w, fragmentLayerWeights.x);
    float blend2 = Blend(blend1, layer2MaterialPropertiesSampler.w, layer3Metallic, layer3MaterialPropertiesSampler.w, fragmentLayerWeights.y);
    float blend3 = Blend(blend2, layer3MaterialPropertiesSampler.w, layer4Metallic, layer4MaterialPropertiesSampler.w, fragmentLayerWeights.z);

    return Blend(blend3, layer4MaterialPropertiesSampler.w, layer5Metallic, layer5MaterialPropertiesSampler.w, fragmentLayerWeights.w);
}

/*
*	Returns the ambient occlusion.
*/
float GetAmbientOcclusion()
{
	float layer1AmbientOcclusion = layer1MaterialPropertiesSampler.z;
    float layer2AmbientOcclusion = layer2MaterialPropertiesSampler.z;
    float layer3AmbientOcclusion = layer3MaterialPropertiesSampler.z;
    float layer4AmbientOcclusion = layer4MaterialPropertiesSampler.z;
    float layer5AmbientOcclusion = layer5MaterialPropertiesSampler.z;

    float blend1 = Blend(layer1AmbientOcclusion, layer1MaterialPropertiesSampler.w, layer2AmbientOcclusion, layer2MaterialPropertiesSampler.w, fragmentLayerWeights.x);
    float blend2 = Blend(blend1, layer2MaterialPropertiesSampler.w, layer3AmbientOcclusion, layer3MaterialPropertiesSampler.w, fragmentLayerWeights.y);
    float blend3 = Blend(blend2, layer3MaterialPropertiesSampler.w, layer4AmbientOcclusion, layer4MaterialPropertiesSampler.w, fragmentLayerWeights.z);

    return Blend(blend3, layer4MaterialPropertiesSampler.w, layer5AmbientOcclusion, layer5MaterialPropertiesSampler.w, fragmentLayerWeights.w);
}

void main()
{
    //Sample the normal depth texture.
    vec4 normalDepthTextureSampler = texture(normalDepthTexture, fragmentTextureCoordinate);

    //Calculate the fragment world position.
    fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, normalDepthTextureSampler.w);

    //Calculate the distance to the camera.
    distanceToCameraSquared = LengthSquared3(cameraWorldPosition - fragmentWorldPosition);

   	//Calculate the blend smoothing.
   	blendSmoothing = min(distanceToCameraSquared * (1.0f / (1000.0f * 1000.0f)), 1.0f);

    //Set the fragment world normal.
    fragmentWorldNormal = normalDepthTextureSampler.xyz;

    //Set the fragment layer weights.
    fragmentLayerWeights = texture(layerWeightsTexture, fragmentTextureCoordinate);

	//Calculate the tri-planar data.
	CalculateTriPlanarData(normalDepthTextureSampler.w);

    //Sampler the material properties.
    layer1MaterialPropertiesSampler = SampleTriPlanar(layer1MaterialPropertiesTexture);
    layer2MaterialPropertiesSampler = SampleTriPlanar(layer2MaterialPropertiesTexture);
    layer3MaterialPropertiesSampler = SampleTriPlanar(layer3MaterialPropertiesTexture);
    layer4MaterialPropertiesSampler = SampleTriPlanar(layer4MaterialPropertiesTexture);
    layer5MaterialPropertiesSampler = SampleTriPlanar(layer5MaterialPropertiesTexture);

	//Set the albedo color.
	albedoColor = vec4(GetAlbedo(), 1.0f);

	//Calculate the tangent space matrix.
	vec3 normal = fragmentWorldNormal;
	vec3 tangent = cross(vec3(0.0f, 0.0f, 1.0f), normal);
	vec3 bitangent = cross(tangent, normal);

	mat3 tangentSpaceMatrix = mat3(tangent, bitangent, normal);

	//Set the normal.
    vec3 normalDirection = GetNormalDirection();
    normalDirection = tangentSpaceMatrix * normalDirection;
    normalDirection = normalize(normalDirection);
    normalDirectionDepth = vec4(normalDirection, normalDepthTextureSampler.w);

	//Set the roughness.
    materialProperties.r = GetRoughness();

    //Set the metallic.
    materialProperties.g = GetMetallic();

    //Set the ambient occlusion.
    materialProperties.b = GetAmbientOcclusion();

    //Write the thickness.
    materialProperties.a = 1.0f;
}