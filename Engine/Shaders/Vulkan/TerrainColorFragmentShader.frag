//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define BLEND_SMOOTHING (0.1f)
#define MATERIAL_TEXTURE_RESOLUTION (65.0f)
#define MATERIAL_TEXTURE_RESOLUTION_STEP (1.0f / MATERIAL_TEXTURE_RESOLUTION)
#define PARALLAX_OCCLUSION_MAPPING_HEIGHT_SCALE (0.2f)
#define PARALLAX_OCCLUSION_MAPPING_SAMPLES (32)
#define PARALLAX_OCCLUSION_MAPPING_SAMPLE_STEP (1.0f / PARALLAX_OCCLUSION_MAPPING_SAMPLES)

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D terrainDataTexture;

//Out parameters.
layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normalDepth;
layout (location = 2) out vec4 materialProperties;

//Globals.
vec3 fragmentWorldPosition;

vec2 textureCoordinate;
int patchMaterials[4];
float blendAlphas[2];

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
    vec3 albedo1 = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[0])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    vec3 albedo2 = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[1])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    vec3 albedo3 = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[2])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    vec3 albedo4 = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[3])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;

    vec3 blend1 = mix(albedo2, albedo1, blendAlphas[0]);
    vec3 blend2 = mix(albedo3, albedo2, blendAlphas[0]);

    return mix(blend1, blend2, blendAlphas[1]);
}

/*
*	Returns the normal.
*/
vec3 GetNormal(int patchMaterial)
{
    return texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterial)].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
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

    //Sample the patch materials.
    patchMaterials[0] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), patchCoordinates).x * 255.0f);
    patchMaterials[1] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), patchCoordinates + vec2(MATERIAL_TEXTURE_RESOLUTION_STEP, 0.0f)).x * 255.0f);
    patchMaterials[2] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), patchCoordinates + vec2(0.0f, MATERIAL_TEXTURE_RESOLUTION_STEP)).x * 255.0f);
    patchMaterials[3] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), patchCoordinates + vec2(MATERIAL_TEXTURE_RESOLUTION_STEP, MATERIAL_TEXTURE_RESOLUTION_STEP)).x * 255.0f);

    //Calculate the blend alphas.
    blendAlphas[0] = fract(patchCoordinates.x * MATERIAL_TEXTURE_RESOLUTION);
    blendAlphas[1] = fract(patchCoordinates.y * MATERIAL_TEXTURE_RESOLUTION);

    //Calculate the fragment world position.
    fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, patchDepth);

    //Calculate the tangent space matrix.
    vec3 tangent = cross(vec3(0.0f, 0.0f, 1.0f), patchNormal);
    vec3 bitangent = cross(tangent, patchNormal);

    mat3 tangentSpaceMatrix = mat3(tangent, bitangent, patchNormal);

	//Calculate the texture coordinates.
	CalculateTextureCoordinates(tangentSpaceMatrix);

	//Write the albedo.
	albedo = vec4(GetAlbedo(), 1.0f);

	//Set the normal.
    vec3 normal = GetNormal(patchMaterials[0]);
    normal = tangentSpaceMatrix * normal;
    normal = normalize(normal);
    normalDepth = vec4(normal, patchDepth);

	//Write the material properties.
    materialProperties = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[0])].materialPropertiesIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
}