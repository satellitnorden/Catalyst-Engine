//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define BLEND_SMOOTHING (1.0f)
#define MATERIAL_TEXTURE_RESOLUTION (64.0f)
#define INVERSE_MATERIAL_TEXTURE_RESOLUTION (0.015625f)
#define HEIGHT_INFLUENCE (10.0f)

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D terrainDataTexture;

//Out parameters.
layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normalDepth;
layout (location = 2) out vec4 materialProperties;

//Globals.
vec2 textureCoordinate;

int terrainMaterials[9];

float materialWeights[9];

vec3 materialAlbedos[9];
vec3 materialNormalMaps[9];
vec4 materialMaterialProperties[9];

/*
*   Blends two terrain values.
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
*   Blends two terrain values.
*/
vec4 Blend(vec4 first, float firstHeight, vec4 second, float secondHeight, float alpha)
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
*   Blends the albedo.
*/
vec3 BlendAlbedo()
{
    return  materialAlbedos[0] * materialWeights[0] + 
            materialAlbedos[1] * materialWeights[1] + 
            materialAlbedos[2] * materialWeights[2] + 
            materialAlbedos[3] * materialWeights[3] + 
            materialAlbedos[4] * materialWeights[4] + 
            materialAlbedos[5] * materialWeights[5] + 
            materialAlbedos[6] * materialWeights[6] + 
            materialAlbedos[7] * materialWeights[7] + 
            materialAlbedos[8] * materialWeights[8];
}

/*
*   Blends the normal map.
*/
vec3 BlendNormalMap()
{
    return  materialNormalMaps[0] * materialWeights[0] + 
            materialNormalMaps[1] * materialWeights[1] + 
            materialNormalMaps[2] * materialWeights[2] + 
            materialNormalMaps[3] * materialWeights[3] + 
            materialNormalMaps[4] * materialWeights[4] + 
            materialNormalMaps[5] * materialWeights[5] + 
            materialNormalMaps[6] * materialWeights[6] + 
            materialNormalMaps[7] * materialWeights[7] + 
            materialNormalMaps[8] * materialWeights[8];
}

/*
*   Blends the material properties.
*/
vec4 BlendMaterialProperties()
{
    return  materialMaterialProperties[0] * materialWeights[0] + 
            materialMaterialProperties[1] * materialWeights[1] + 
            materialMaterialProperties[2] * materialWeights[2] + 
            materialMaterialProperties[3] * materialWeights[3] + 
            materialMaterialProperties[4] * materialWeights[4] + 
            materialMaterialProperties[5] * materialWeights[5] + 
            materialMaterialProperties[6] * materialWeights[6] + 
            materialMaterialProperties[7] * materialWeights[7] + 
            materialMaterialProperties[8] * materialWeights[8];
}

/*
*   Samples all materials.
*/
void SampleMaterials()
{
    //Sample the albedos.
    materialAlbedos[0] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[0]].albedoTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[1] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[1]].albedoTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[2] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[2]].albedoTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[3] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[3]].albedoTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[4] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[4]].albedoTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[5] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[5]].albedoTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[6] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[6]].albedoTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[7] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[7]].albedoTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[8] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[8]].albedoTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;

    //Sample the normal maps.
    materialNormalMaps[0] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[0]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[1] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[1]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[2] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[2]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[3] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[3]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[4] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[4]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[5] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[5]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[6] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[6]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[7] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[7]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[8] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[8]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;

    //Sample the material properties.
    materialMaterialProperties[0] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[0]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[1] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[1]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[2] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[2]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[3] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[3]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[4] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[4]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[5] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[5]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[6] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[6]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[7] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[7]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[8] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[8]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
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

    //Calculate all corners.
    vec2 corners[9];

    corners[0] = patchCoordinates - vec2(fract(patchCoordinates.x * MATERIAL_TEXTURE_RESOLUTION) * INVERSE_MATERIAL_TEXTURE_RESOLUTION, fract(patchCoordinates.y * MATERIAL_TEXTURE_RESOLUTION) * INVERSE_MATERIAL_TEXTURE_RESOLUTION);

    corners[1] = corners[0] + vec2(-INVERSE_MATERIAL_TEXTURE_RESOLUTION, -INVERSE_MATERIAL_TEXTURE_RESOLUTION);
    corners[2] = corners[0] + vec2(-INVERSE_MATERIAL_TEXTURE_RESOLUTION, 0.0f);
    corners[3] = corners[0] + vec2(-INVERSE_MATERIAL_TEXTURE_RESOLUTION, INVERSE_MATERIAL_TEXTURE_RESOLUTION);

    corners[4] = corners[0] + vec2(0.0f, -INVERSE_MATERIAL_TEXTURE_RESOLUTION);
    corners[5] = corners[0] + vec2(0.0f, INVERSE_MATERIAL_TEXTURE_RESOLUTION);

    corners[6] = corners[0] + vec2(INVERSE_MATERIAL_TEXTURE_RESOLUTION, -INVERSE_MATERIAL_TEXTURE_RESOLUTION);
    corners[7] = corners[0] + vec2(INVERSE_MATERIAL_TEXTURE_RESOLUTION, 0.0f);
    corners[8] = corners[0] + vec2(INVERSE_MATERIAL_TEXTURE_RESOLUTION, INVERSE_MATERIAL_TEXTURE_RESOLUTION);

    //Sample the terrain materials.
    terrainMaterials[0] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[0]).x * 255.0f);
    terrainMaterials[1] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[1]).x * 255.0f);
    terrainMaterials[2] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[2]).x * 255.0f);
    terrainMaterials[3] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[3]).x * 255.0f);
    terrainMaterials[4] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[4]).x * 255.0f);
    terrainMaterials[5] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[5]).x * 255.0f);
    terrainMaterials[6] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[6]).x * 255.0f);
    terrainMaterials[7] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[7]).x * 255.0f);
    terrainMaterials[8] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[8]).x * 255.0f);

    //Calculate the fragment world position.
    vec3 fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, patchDepth);

    //Calculate the texture coordinate.
    textureCoordinate = fragmentWorldPosition.xz * 0.2f;

    //Sample all materials.
    SampleMaterials();

    //Calculate the material weights.
    materialWeights[0] = SmoothStep(1.0f - min(length(patchCoordinates - corners[0]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * pow(materialMaterialProperties[0].w, HEIGHT_INFLUENCE);
    materialWeights[1] = SmoothStep(1.0f - min(length(patchCoordinates - corners[1]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * pow(materialMaterialProperties[1].w, HEIGHT_INFLUENCE);
    materialWeights[2] = SmoothStep(1.0f - min(length(patchCoordinates - corners[2]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * pow(materialMaterialProperties[2].w, HEIGHT_INFLUENCE);
    materialWeights[3] = SmoothStep(1.0f - min(length(patchCoordinates - corners[3]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * pow(materialMaterialProperties[3].w, HEIGHT_INFLUENCE);
    materialWeights[4] = SmoothStep(1.0f - min(length(patchCoordinates - corners[4]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * pow(materialMaterialProperties[4].w, HEIGHT_INFLUENCE);
    materialWeights[5] = SmoothStep(1.0f - min(length(patchCoordinates - corners[5]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * pow(materialMaterialProperties[5].w, HEIGHT_INFLUENCE);
    materialWeights[6] = SmoothStep(1.0f - min(length(patchCoordinates - corners[6]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * pow(materialMaterialProperties[6].w, HEIGHT_INFLUENCE);
    materialWeights[7] = SmoothStep(1.0f - min(length(patchCoordinates - corners[7]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * pow(materialMaterialProperties[7].w, HEIGHT_INFLUENCE);
    materialWeights[8] = SmoothStep(1.0f - min(length(patchCoordinates - corners[8]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * pow(materialMaterialProperties[8].w, HEIGHT_INFLUENCE);

    //Normalize the material weights.
    float materialWeightsTotal = 1.0f / (materialWeights[0] + materialWeights[1] + materialWeights[2] + materialWeights[3] + materialWeights[4] + materialWeights[5] + materialWeights[6] + materialWeights[7] + materialWeights[8]);

    materialWeights[0] *= materialWeightsTotal;
    materialWeights[1] *= materialWeightsTotal;
    materialWeights[2] *= materialWeightsTotal;
    materialWeights[3] *= materialWeightsTotal;
    materialWeights[4] *= materialWeightsTotal;
    materialWeights[5] *= materialWeightsTotal;
    materialWeights[6] *= materialWeightsTotal;
    materialWeights[7] *= materialWeightsTotal;
    materialWeights[8] *= materialWeightsTotal;

	//Write the albedo.
	albedo = vec4(BlendAlbedo(), 1.0f);

	//Write the normal/depth.
    vec3 terrainNormal = texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].normalTextureIndex)], globalSamplers[FilterLinear_MipmapModeNearest_AddressModeClampToEdge_Index]), patchCoordinates).xyz * 2.0f - 1.0f;

    vec3 tangent = cross(vec3(0.0f, 0.0f, 1.0f), terrainNormal);
    vec3 bitangent = cross(tangent, terrainNormal);
    mat3 tangentSpaceMatrix = mat3(tangent, bitangent, terrainNormal);

    vec3 normalMap = BlendNormalMap();
    vec3 normal = normalize(tangentSpaceMatrix * normalMap);
    normalDepth = vec4(normal, patchDepth);

	//Write the material properties.
    materialProperties = BlendMaterialProperties();
}