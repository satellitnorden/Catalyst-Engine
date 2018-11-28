//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define BLEND_SMOOTHING (0.1f)
#define PARALLAX_OCCLUSION_MAPPING_HEIGHT_SCALE (0.2f)
#define PARALLAX_OCCLUSION_MAPPING_SAMPLES (32)
#define PARALLAX_OCCLUSION_MAPPING_SAMPLE_STEP (1.0f / PARALLAX_OCCLUSION_MAPPING_SAMPLES)

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D terrainDataTexture;

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

vec3 fragmentWorldPosition;

vec2 textureCoordinate;

//Forward declarations.
vec2 CalculateParallaxOcclusionMapping(vec2 originalTextureCoordinate, sampler2D heightTexture);
void CalculateTextureCoordinates(mat3 tangentSpaceMatrix);

/*
*   Calculates the parallax occlusion mapping at the given texture coordinate.
*/
vec2 CalculateParallaxOcclusionMapping(vec3 tangentSpaceViewDirection, vec2 originalTextureCoordinate, sampler2D heightTexture)
{
    vec2 offset = tangentSpaceViewDirection.xy * PARALLAX_OCCLUSION_MAPPING_HEIGHT_SCALE;
    vec2 offsetStep = offset / PARALLAX_OCCLUSION_MAPPING_SAMPLES;

    vec2 currentTextureCoordinate = originalTextureCoordinate;
    float currentDepth = 1.0f - texture(heightTexture, currentTextureCoordinate).w;
    float accumulatedDepth = 0.0f;

    while(accumulatedDepth < currentDepth)
    {
        currentTextureCoordinate -= offsetStep;

        currentDepth = 1.0f - texture(heightTexture, currentTextureCoordinate).w;  

        accumulatedDepth += PARALLAX_OCCLUSION_MAPPING_SAMPLE_STEP;  
    }

    vec2 previousTextureCoordinate = currentTextureCoordinate + offsetStep;

    float afterDepth  = currentDepth - accumulatedDepth;
    float beforeDepth = 1.0f - texture(heightTexture, previousTextureCoordinate).w - accumulatedDepth + PARALLAX_OCCLUSION_MAPPING_SAMPLE_STEP;
     
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTextureCoordinate = previousTextureCoordinate * weight + currentTextureCoordinate * (1.0 - weight);

    return finalTextureCoordinate;  
}

/*
*   Calculates the texture coordinates for all layers.
*/
void CalculateTextureCoordinates(mat3 tangentSpaceMatrix)
{
    mat3 transposedTangentSpaceMatrix = inverse(tangentSpaceMatrix);
    vec3 tangentSpaceCameraWorldPosition = transposedTangentSpaceMatrix * cameraWorldPosition;
    vec3 tangentSpaceFragmentWorldPosition = transposedTangentSpaceMatrix * fragmentWorldPosition;
    vec3 tangentSpaceViewDirection = normalize(tangentSpaceCameraWorldPosition - tangentSpaceFragmentWorldPosition);

    textureCoordinate = fragmentWorldPosition.xz * 0.25f;
}

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

    float difference = abs(firstWeight - secondWeight);
    float newAlpha = mix(0.5f, 1.0f, min(difference / BLEND_SMOOTHING, 1.0f));

    return firstWeight > secondWeight ? mix(second, first, newAlpha) : mix(first, second, newAlpha);
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

    float difference = abs(firstWeight - secondWeight);
    float newAlpha = mix(0.5f, 1.0f, min(difference / BLEND_SMOOTHING, 1.0f));

    return firstWeight > secondWeight ? mix(second, first, newAlpha) : mix(first, second, newAlpha);
}

/*
*	Returns the albedo.
*/
vec3 GetAlbedo()
{
    return texture(sampler2D(globalTextures[terrainMaterialData[0].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
}

/*
*	Returns the normal direction.
*/
vec3 GetNormalDirection()
{
    return texture(sampler2D(globalTextures[terrainMaterialData[0].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
}

void main()
{
    //Sample the terrain data texture.
    vec4 terrainDataTextureSampler = texture(terrainDataTexture, fragmentTextureCoordinate);

    //Retrieve the patch index.
    int patchIndex = int(terrainDataTextureSampler.x);

    //Retrieve the patch coordinates.
    vec2 patchCoordinates = terrainDataTextureSampler.yz;

    //Retrieve the patch depth.
    float patchDepth = terrainDataTextureSampler.w; 

    //Sample the patch normal.
    vec3 patchNormal = texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].normalTextureIndex], globalSamplers[FilterLinear_MipmapModeNearest_AddressModeClampToEdge_Index]), patchCoordinates).xyz * 2.0f - 1.0f;

    //Sample the patch layer weights.
    vec4 patchLayerWeights = texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].layerWeightsTextureIndex], globalSamplers[FilterLinear_MipmapModeNearest_AddressModeClampToEdge_Index]), patchCoordinates);

    //Calculate the fragment world position.
    fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, patchDepth);

    //Calculate the tangent space matrix.
    vec3 normal = patchNormal;
    vec3 tangent = cross(vec3(0.0f, 0.0f, 1.0f), normal);
    vec3 bitangent = cross(tangent, normal);

    mat3 tangentSpaceMatrix = mat3(tangent, bitangent, normal);

	//Calculate the texture coordinates.
	CalculateTextureCoordinates(tangentSpaceMatrix);

	//Set the albedo color.
	albedoColor = vec4(GetAlbedo(), 1.0f);

	//Set the normal.
    vec3 normalDirection = GetNormalDirection();
    normalDirection = tangentSpaceMatrix * normalDirection;
    normalDirection = normalize(normalDirection);
    normalDirectionDepth = vec4(normalDirection, patchDepth);

	//Write the material properties.
    materialProperties = texture(sampler2D(globalTextures[terrainMaterialData[0].materialPropertiesIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
}