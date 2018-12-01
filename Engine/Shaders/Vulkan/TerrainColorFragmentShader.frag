//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define BLEND_SMOOTHING (0.1f)
#define MATERIAL_TEXTURE_RESOLUTION (65.0f)
#define INVERSE_MATERIAL_TEXTURE_RESOLUTION (0.0153846153846152f)
#define HALF_INVERSE_MATERIAL_TEXTURE_RESOLUTION (0.0076923076923077f)

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

int patchMaterials[9];
vec3 materialAlbedos[9];
vec3 materialNormalMaps[9];
vec4 materialMaterialProperties[9];
float materialWeights[9];

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
*	Blends the albedo.
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
*	Blends the normal map.
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
*   Samples the materials.
*/
void SampleMaterials()
{
    //Sample the albedos.
    materialAlbedos[0] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[0])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[1] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[1])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[2] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[2])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[3] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[3])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[4] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[4])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[5] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[5])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[6] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[6])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[7] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[7])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;
    materialAlbedos[8] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[8])].albedoTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb;

    //Sample the normal maps.
    materialNormalMaps[0] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[0])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[1] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[1])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[2] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[2])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[3] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[3])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[4] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[4])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[5] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[5])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[6] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[6])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[7] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[7])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[8] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[8])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;

    //Sample the material properties.
    materialMaterialProperties[0] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[0])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[1] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[1])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[2] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[2])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[3] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[3])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[4] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[4])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[5] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[5])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[6] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[6])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[7] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[7])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[8] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[8])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);

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

    //Calculate the texture coordinate.
    textureCoordinate = CalculateFragmentWorldPosition(fragmentTextureCoordinate, patchDepth).xz * 0.25f;

    //Calculate the sample points.
    vec2 realPatchCoordinates = patchCoordinates * MATERIAL_TEXTURE_RESOLUTION;

    vec2 samplePoints[9];

    samplePoints[0] = (realPatchCoordinates - fract(realPatchCoordinates)) * INVERSE_MATERIAL_TEXTURE_RESOLUTION;

    samplePoints[1] = samplePoints[0] + vec2(-INVERSE_MATERIAL_TEXTURE_RESOLUTION, -INVERSE_MATERIAL_TEXTURE_RESOLUTION);
    samplePoints[2] = samplePoints[0] + vec2(-INVERSE_MATERIAL_TEXTURE_RESOLUTION, 0.0f);
    samplePoints[3] = samplePoints[0] + vec2(-INVERSE_MATERIAL_TEXTURE_RESOLUTION, INVERSE_MATERIAL_TEXTURE_RESOLUTION);

    samplePoints[4] = samplePoints[0] + vec2(0.0f, -INVERSE_MATERIAL_TEXTURE_RESOLUTION);
    samplePoints[5] = samplePoints[0] + vec2(0.0f, INVERSE_MATERIAL_TEXTURE_RESOLUTION);

    samplePoints[6] = samplePoints[0] + vec2(INVERSE_MATERIAL_TEXTURE_RESOLUTION, -INVERSE_MATERIAL_TEXTURE_RESOLUTION);
    samplePoints[7] = samplePoints[0] + vec2(INVERSE_MATERIAL_TEXTURE_RESOLUTION, 0.0f);
    samplePoints[8] = samplePoints[0] + vec2(INVERSE_MATERIAL_TEXTURE_RESOLUTION, INVERSE_MATERIAL_TEXTURE_RESOLUTION);

    //Sample the patch materials.
    patchMaterials[0] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), samplePoints[0]).x * 255.0f);
    patchMaterials[1] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), samplePoints[1]).x * 255.0f);
    patchMaterials[2] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), samplePoints[2]).x * 255.0f);
    patchMaterials[3] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), samplePoints[3]).x * 255.0f);
    patchMaterials[4] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), samplePoints[4]).x * 255.0f);
    patchMaterials[5] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), samplePoints[5]).x * 255.0f);
    patchMaterials[6] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), samplePoints[6]).x * 255.0f);
    patchMaterials[7] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), samplePoints[7]).x * 255.0f);
    patchMaterials[8] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), samplePoints[8]).x * 255.0f);

    //Sample the materials.
    SampleMaterials();

    //Calculate the material weights.
    materialWeights[0] = (1.0f - min(length(patchCoordinates - samplePoints[0]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * materialMaterialProperties[0].w;
    materialWeights[1] = (1.0f - min(length(patchCoordinates - samplePoints[1]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * materialMaterialProperties[1].w;
    materialWeights[2] = (1.0f - min(length(patchCoordinates - samplePoints[2]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * materialMaterialProperties[2].w;
    materialWeights[3] = (1.0f - min(length(patchCoordinates - samplePoints[3]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * materialMaterialProperties[3].w;
    materialWeights[4] = (1.0f - min(length(patchCoordinates - samplePoints[4]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * materialMaterialProperties[4].w;
    materialWeights[5] = (1.0f - min(length(patchCoordinates - samplePoints[5]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * materialMaterialProperties[5].w;
    materialWeights[6] = (1.0f - min(length(patchCoordinates - samplePoints[6]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * materialMaterialProperties[6].w;
    materialWeights[7] = (1.0f - min(length(patchCoordinates - samplePoints[7]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * materialMaterialProperties[7].w;
    materialWeights[8] = (1.0f - min(length(patchCoordinates - samplePoints[8]) * MATERIAL_TEXTURE_RESOLUTION, 1.0f)) * materialMaterialProperties[8].w;

    float blendNormalization = 1.0f / (materialWeights[0] + materialWeights[1] + materialWeights[2] + materialWeights[3] + materialWeights[4] + materialWeights[5] + materialWeights[6] + materialWeights[7] + materialWeights[8]);

    materialWeights[0] *= blendNormalization;
    materialWeights[1] *= blendNormalization;
    materialWeights[2] *= blendNormalization;
    materialWeights[3] *= blendNormalization;
    materialWeights[4] *= blendNormalization;
    materialWeights[5] *= blendNormalization;
    materialWeights[6] *= blendNormalization;
    materialWeights[7] *= blendNormalization;
    materialWeights[8] *= blendNormalization;

    //Sample the patch normal.
    vec3 patchNormal = texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].normalTextureIndex], globalSamplers[FilterLinear_MipmapModeNearest_AddressModeClampToEdge_Index]), patchCoordinates).xyz * 2.0f - 1.0f;

    //Calculate the tangent space matrix.
    vec3 tangent = cross(vec3(0.0f, 0.0f, 1.0f), patchNormal);
    vec3 bitangent = cross(tangent, patchNormal);

    mat3 tangentSpaceMatrix = mat3(tangent, bitangent, patchNormal);

	//Write the albedo.
	albedo = vec4(BlendAlbedo(), 1.0f);

	//Write the normal.
    vec3 normal = BlendNormalMap();
    normal = tangentSpaceMatrix * normal;
    normal = normalize(normal);
    normalDepth = vec4(normal, patchDepth);

	//Write the material properties.
    materialProperties = BlendMaterialProperties();
}