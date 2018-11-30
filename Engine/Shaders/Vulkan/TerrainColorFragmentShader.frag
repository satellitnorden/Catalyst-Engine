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
vec2 textureCoordinate;

int patchMaterials[4];
vec3 materialAlbedos[4];
vec3 materialNormalMaps[4];
vec4 materialMaterialProperties[4];
float blendAlphas[2];
float verticalDisplacementBlends[2];

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
    vec3 blend1 = Blend(materialAlbedos[0], materialMaterialProperties[0].w, materialAlbedos[1], materialMaterialProperties[1].w, blendAlphas[0]);
    vec3 blend2 = Blend(materialAlbedos[2], materialMaterialProperties[2].w, materialAlbedos[3], materialMaterialProperties[3].w, blendAlphas[0]);

    return Blend(blend1, verticalDisplacementBlends[0], blend2, verticalDisplacementBlends[1], blendAlphas[1]);
}

/*
*	Blends the normal map.
*/
vec3 BlendNormalMap()
{
    vec3 blend1 = Blend(materialNormalMaps[0], materialMaterialProperties[0].w, materialNormalMaps[1], materialMaterialProperties[1].w, blendAlphas[0]);
    vec3 blend2 = Blend(materialNormalMaps[2], materialMaterialProperties[2].w, materialNormalMaps[3], materialMaterialProperties[3].w, blendAlphas[0]);

    return Blend(blend1, verticalDisplacementBlends[0], blend2, verticalDisplacementBlends[1], blendAlphas[1]);
}

/*
*   Blends the material properties.
*/
vec4 BlendMaterialProperties()
{
    vec4 blend1 = Blend(materialMaterialProperties[0], materialMaterialProperties[0].w, materialMaterialProperties[1], materialMaterialProperties[1].w, blendAlphas[0]);
    vec4 blend2 = Blend(materialMaterialProperties[2], materialMaterialProperties[2].w, materialMaterialProperties[3], materialMaterialProperties[3].w, blendAlphas[0]);

    return Blend(blend1, verticalDisplacementBlends[0], blend2, verticalDisplacementBlends[1], blendAlphas[1]);
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

    //Sample the normal maps.
    materialNormalMaps[0] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[0])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[1] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[1])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[2] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[2])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    materialNormalMaps[3] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[3])].normalMapTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;

    //Sample the material properties.
    materialMaterialProperties[0] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[0])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[1] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[1])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[2] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[2])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
    materialMaterialProperties[3] = texture(sampler2D(globalTextures[terrainMaterialData[nonuniformEXT(patchMaterials[3])].materialPropertiesTextureIndex], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
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

    //Sample the patch materials.
    patchMaterials[0] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), patchCoordinates).x * 255.0f);
    patchMaterials[1] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), patchCoordinates + vec2(MATERIAL_TEXTURE_RESOLUTION_STEP, 0.0f)).x * 255.0f);
    patchMaterials[2] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), patchCoordinates + vec2(0.0f, MATERIAL_TEXTURE_RESOLUTION_STEP)).x * 255.0f);
    patchMaterials[3] = int(texture(sampler2D(globalTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], globalSamplers[FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index]), patchCoordinates + vec2(MATERIAL_TEXTURE_RESOLUTION_STEP, MATERIAL_TEXTURE_RESOLUTION_STEP)).x * 255.0f);

    //Sample the materials.
    SampleMaterials();

    //Calculate the blend alphas.
    blendAlphas[0] = fract(patchCoordinates.x * MATERIAL_TEXTURE_RESOLUTION);
    blendAlphas[1] = fract(patchCoordinates.y * MATERIAL_TEXTURE_RESOLUTION);

    //Calculate the vertical displacement blends.
    verticalDisplacementBlends[0] = mix(materialMaterialProperties[0].w, materialMaterialProperties[1].w, blendAlphas[0]);
    verticalDisplacementBlends[1] = mix(materialMaterialProperties[2].w, materialMaterialProperties[3].w, blendAlphas[0]);

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