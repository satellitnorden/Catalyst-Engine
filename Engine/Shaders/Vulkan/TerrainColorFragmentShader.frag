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
    vec3 patchNormal = texture(terrainMaterialTextures[terrainPatchData[nonuniformEXT(patchIndex)].normalTextureIndex], patchCoordinates).xyz * 2.0f - 1.0f;

    //Sample the patch material.
    int patchMaterial = int(texture(terrainMaterialTextures[terrainPatchData[nonuniformEXT(patchIndex)].materialTextureIndex], patchCoordinates).x * 255.0f);

    //Calculate the texture coordinate.
    vec2 textureCoordinate = CalculateFragmentWorldPosition(fragmentTextureCoordinate, patchDepth).xz * 0.25f;

    //Calculate the tangent space matrix.
    vec3 tangent = cross(vec3(0.0f, 0.0f, 1.0f), patchNormal);
    vec3 bitangent = cross(tangent, patchNormal);

    mat3 tangentSpaceMatrix = mat3(tangent, bitangent, patchNormal);

	//Write the albedo.
	albedo = vec4(texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[patchMaterial].albedoTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).rgb, 1.0f);

	//Write the normal.
    vec3 normal = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[patchMaterial].normalMapTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate).xyz * 2.0f - 1.0f;
    normal = tangentSpaceMatrix * normal;
    normal = normalize(normal);
    normalDepth = vec4(normal, patchDepth);

	//Write the material properties.
    materialProperties = texture(sampler2D(globalTextures[nonuniformEXT(terrainMaterialData[patchMaterial].materialPropertiesTextureIndex)], globalSamplers[FilterLinear_MipmapModeLinear_AddressModeRepeat_Index]), textureCoordinate);
}