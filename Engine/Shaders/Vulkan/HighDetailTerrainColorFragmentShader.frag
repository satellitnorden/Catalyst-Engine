//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

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

//Forward declarations.
float RandomFloat(vec3 seed);

/*
*   Given a coordinate and a seed, returns a random number.
*/
float RandomFloat(vec3 seed)
{
    return fract(sin(dot(seed.xy * seed.z, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

/*
*	Calculates the tri-planar data.
*/
void CalculateTriPlanarData(float depth)
{
	//Calculate the absolute normal.
	absoluteNormal = abs(fragmentWorldNormal);

	//Calculate the texture coordinates on the three planes.
	textureCoordinateYZ = fragmentWorldPosition.yz;
	textureCoordinateXZ = fragmentWorldPosition.xz;
	textureCoordinateXY = fragmentWorldPosition.xy;

    //Calculate the random float.
    float randomFloat = RandomFloat(vec3(gl_FragCoord.xy, depth));

    //Pick which plane to sample.
    if (absoluteNormal.x > absoluteNormal.y && absoluteNormal.x > absoluteNormal.z && absoluteNormal.x > randomFloat)
    {
        finalTextureCoordinate = textureCoordinateYZ;
    }

    else if (absoluteNormal.y > absoluteNormal.x && absoluteNormal.y > absoluteNormal.z && absoluteNormal.y > randomFloat)
    {
        finalTextureCoordinate = textureCoordinateXZ;
    }

    else
    {
        finalTextureCoordinate = textureCoordinateXY;
    }

    finalTextureCoordinate *= 0.25f;
}

/*
*	Samples a texture using tri-planar mapping.
*/
vec4 SampleTriPlanar(sampler2D textureSampler)
{
	return texture(textureSampler, finalTextureCoordinate);
}

/*
*	Returns the albedo.
*/
vec4 GetAlbedo()
{
    vec4 layer1Albedo = SampleTriPlanar(layer1AlbedoTexture);
    vec4 layer2Albedo = SampleTriPlanar(layer2AlbedoTexture);
    vec4 layer3Albedo = SampleTriPlanar(layer3AlbedoTexture);
    vec4 layer4Albedo = SampleTriPlanar(layer4AlbedoTexture);
    vec4 layer5Albedo = SampleTriPlanar(layer5AlbedoTexture);

    vec4 blend1 = mix(layer1Albedo, layer2Albedo, fragmentLayerWeights.x);
    vec4 blend2 = mix(blend1, layer3Albedo, fragmentLayerWeights.y);
    vec4 blend3 = mix(blend2, layer4Albedo, fragmentLayerWeights.z);

    return mix(blend3, layer5Albedo, fragmentLayerWeights.w);
}

/*
*	Returns the normal direction.
*/
vec3 GetNormalDirection()
{
	vec3 layer1NormalDirection = SampleTriPlanar(layer1NormalMapTexture).xyz;
    vec3 layer2NormalDirection = SampleTriPlanar(layer2NormalMapTexture).xyz;
    vec3 layer3NormalDirection = SampleTriPlanar(layer3NormalMapTexture).xyz;
    vec3 layer4NormalDirection = SampleTriPlanar(layer4NormalMapTexture).xyz;
    vec3 layer5NormalDirection = SampleTriPlanar(layer5NormalMapTexture).xyz;

    vec3 blend1 = mix(layer1NormalDirection, layer2NormalDirection, fragmentLayerWeights.x);
    vec3 blend2 = mix(blend1, layer3NormalDirection, fragmentLayerWeights.y);
    vec3 blend3 = mix(blend2, layer4NormalDirection, fragmentLayerWeights.z);

    return mix(blend3, layer5NormalDirection, fragmentLayerWeights.w) * 2.0f - 1.0f;
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

    float blend1 = mix(layer1Roughness, layer2Roughness, fragmentLayerWeights.x);
    float blend2 = mix(blend1, layer3Roughness, fragmentLayerWeights.y);
    float blend3 = mix(blend2, layer4Roughness, fragmentLayerWeights.z);

    return mix(blend3, layer5Roughness, fragmentLayerWeights.w);
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

    float blend1 = mix(layer1Metallic, layer2Metallic, fragmentLayerWeights.x);
    float blend2 = mix(blend1, layer3Metallic, fragmentLayerWeights.y);
    float blend3 = mix(blend2, layer4Metallic, fragmentLayerWeights.z);

    return mix(blend3, layer5Metallic, fragmentLayerWeights.w);
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

    float blend1 = mix(layer1AmbientOcclusion, layer2AmbientOcclusion, fragmentLayerWeights.x);
    float blend2 = mix(blend1, layer3AmbientOcclusion, fragmentLayerWeights.y);
    float blend3 = mix(blend2, layer4AmbientOcclusion, fragmentLayerWeights.z);

    return mix(blend3, layer5AmbientOcclusion, fragmentLayerWeights.w);
}

void main()
{
    //Sample the normal depth texture.
    vec4 normalDepthTextureSampler = texture(normalDepthTexture, fragmentTextureCoordinate);

    //Calculate the fragment world position.
    fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, normalDepthTextureSampler.w);

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
	albedoColor = GetAlbedo();

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