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

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D terrainDataTexture;

//Out parameters.
layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normalDepth;
layout (location = 2) out vec4 materialProperties;

//Globals.
float blendedHeights[2];
float horizontalBlend;
float verticalBlend;

int terrainMaterials[4];

vec2 textureCoordinate;

vec3 materialAlbedos[4];
vec3 materialNormalMaps[4];
vec4 materialMaterialProperties[4];

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
    vec3 blend1 = Blend(materialAlbedos[0], materialMaterialProperties[0].w, materialAlbedos[1], materialMaterialProperties[1].w, horizontalBlend);
    vec3 blend2 = Blend(materialAlbedos[2], materialMaterialProperties[2].w, materialAlbedos[3], materialMaterialProperties[3].w, horizontalBlend);
    
    return Blend(blend1, blendedHeights[0], blend2, blendedHeights[1], verticalBlend);
}

/*
*   Blends the normal map.
*/
vec3 BlendNormalMap()
{
    vec3 blend1 = Blend(materialNormalMaps[0], materialMaterialProperties[0].w, materialNormalMaps[1], materialMaterialProperties[1].w, horizontalBlend);
    vec3 blend2 = Blend(materialNormalMaps[2], materialMaterialProperties[2].w, materialNormalMaps[3], materialMaterialProperties[3].w, horizontalBlend);
    
    return Blend(blend1, blendedHeights[0], blend2, blendedHeights[1], verticalBlend);
}

/*
*   Blends the material properties.
*/
vec4 BlendMaterialProperties()
{
    vec4 blend1 = Blend(materialMaterialProperties[0], materialMaterialProperties[0].w, materialMaterialProperties[1], materialMaterialProperties[1].w, horizontalBlend);
    vec4 blend2 = Blend(materialMaterialProperties[2], materialMaterialProperties[2].w, materialMaterialProperties[3], materialMaterialProperties[3].w, horizontalBlend);
    
    return Blend(blend1, blendedHeights[0], blend2, blendedHeights[1], verticalBlend);
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

    //Sample the normal maps.
    materialNormalMaps[0] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[0]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[1] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[1]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[2] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[2]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[3] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[3]].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;

    //Sample the material properties.
    materialMaterialProperties[0] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[0]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[1] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[1]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[2] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[2]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[3] = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[terrainMaterials[3]].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
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
    vec2 corners[4];

    corners[0] = patchCoordinates;
    corners[1] = patchCoordinates + vec2(INVERSE_MATERIAL_TEXTURE_RESOLUTION, 0.0f);
    corners[2] = patchCoordinates + vec2(0.0f, INVERSE_MATERIAL_TEXTURE_RESOLUTION);
    corners[3] = patchCoordinates + vec2(INVERSE_MATERIAL_TEXTURE_RESOLUTION, INVERSE_MATERIAL_TEXTURE_RESOLUTION);

    //Sample the terrain materials.
    terrainMaterials[0] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[0]).x * 255.0f);
    terrainMaterials[1] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[1]).x * 255.0f);
    terrainMaterials[2] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[2]).x * 255.0f);
    terrainMaterials[3] = int(texture(sampler2D(globalTextures[nonuniformEXT(terrainPatchData[patchIndex].materialTextureIndex)], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), corners[3]).x * 255.0f);

    //Calculate the texture coordinate.
    textureCoordinate = CalculateFragmentWorldPosition(fragmentTextureCoordinate, patchDepth).xz * 0.25f;

    //Sample all materials.
    SampleMaterials();

    //Calculate the horizontal blend.
    horizontalBlend = fract(patchCoordinates.x * MATERIAL_TEXTURE_RESOLUTION);
    verticalBlend = fract(patchCoordinates.y * MATERIAL_TEXTURE_RESOLUTION);

    //Calculate the blended heights.
    blendedHeights[0] = Blend(materialMaterialProperties[0].w, materialMaterialProperties[0].w, materialMaterialProperties[1].w, materialMaterialProperties[1].w, horizontalBlend);
    blendedHeights[1] = Blend(materialMaterialProperties[2].w, materialMaterialProperties[2].w, materialMaterialProperties[3].w, materialMaterialProperties[3].w, horizontalBlend);

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