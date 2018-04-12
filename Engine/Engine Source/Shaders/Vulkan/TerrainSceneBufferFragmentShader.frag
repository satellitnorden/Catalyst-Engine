//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, set = 0, binding = 0) uniform DynamicUniformData
{
    //Camera data.
    layout (offset = 0) float cameraFieldOfViewCosine; //Offset; 0 - Size; 16
    layout (offset = 16) mat4 inverseCameraMatrix; //Offset; 16 - Size; 64
    layout (offset = 80) mat4 inverseProjectionMatrix; //Offset; 80 - Size; 64
    layout (offset = 144) mat4 originViewMatrix; //Offset; 144 - Size; 64
    layout (offset = 208) mat4 viewMatrix; //Offset; 208 - Size; 64
    layout (offset = 272) vec3 cameraForwardVector; //Offset; 272 - Size; 16
    layout (offset = 288) vec3 cameraWorldPosition; //Offset; 288 - Size; 16

    //Directional light data.
    layout (offset = 304) float directionalLightIntensity; //Offset; 304 - Size; 16
    layout (offset = 320) vec3 directionalLightDirection; //Offset; 320 - Size; 16
    layout (offset = 336) vec3 directionalLightColor; //Offset; 336 - Size; 16
    layout (offset = 352) vec3 directionalLightScreenSpacePosition; //Offset; 352 - Size; 16

    //General data.
    layout (offset = 368) float totalGameTime; //Offset; 368 - Size; 4

    //Point light data.
    layout (offset = 372) int numberOfPointLights; //Offset; 372 - Size; 12
    layout (offset = 384) float pointLightAttenuationDistances[MaximumNumberOfPointLights]; //Offset; 384 - Size; 128
    layout (offset = 512) float pointLightIntensities[MaximumNumberOfPointLights]; //Offset; 512 - Size; 128
    layout (offset = 640) vec3 pointLightColors[MaximumNumberOfPointLights]; //Offset; 640 - Size; 128
    layout (offset = 768) vec3 pointLightWorldPositions[MaximumNumberOfPointLights]; //Offset; 768 - Size; 128

    //Spot light data.
    layout (offset = 896) int numberOfSpotLights; //Offset; 896 - Size; 16
    layout (offset = 912) float spotLightAttenuationDistances[MaximumNumberOfSpotLights]; //Offset; 912 - Size; 128
    layout (offset = 1040) float spotLightIntensities[MaximumNumberOfSpotLights]; //Offset; 1040 - Size; 128
    layout (offset = 1168) float spotLightInnerCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1168 - Size; 128
    layout (offset = 1296) float spotLightOuterCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1296 - Size; 128
    layout (offset = 1424) vec3 spotLightColors[MaximumNumberOfSpotLights]; //Offset; 1424 - Size; 128
    layout (offset = 1552) vec3 spotLightDirections[MaximumNumberOfSpotLights]; //Offset; 1552 - Size; 128
    layout (offset = 1680) vec3 spotLightWorldPositions[MaximumNumberOfSpotLights]; //Offset; 1680 - Size; 128

    //Total size; 1808
};

//Terrain uniform buffer.
layout (std140, set = 1, binding = 1) uniform TerrainUniformData
{
    float terrainFirstLayerDisplacementHeight;
    float terrainSecondLayerDisplacementHeight;
    float terrainThirdLayerDisplacementHeight;
    float terrainFourthLayerDisplacementHeight;
    float terrainFifthLayerDisplacementHeight;
    float terrainHeight;
    float terrainSize;
    float terrainTextureTilingFactor;
    vec3 terrainPosition;
};

layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentHeightMapTextureCoordinate;
layout (location = 1) in vec2 fragmentTextureCoordinate;
layout (location = 2) in vec4 layerWeightsSampler;

//Texture samplers.
layout (set = 1, binding = 2) uniform sampler2D terrainPropertiesTexture;
layout (set = 1, binding = 4) uniform sampler2D layer1AlbedoTexture;
layout (set = 1, binding = 5) uniform sampler2D layer1NormalMapTexture;
layout (set = 1, binding = 6) uniform sampler2D layer1MaterialPropertiesTexture;
layout (set = 1, binding = 7) uniform sampler2D layer2AlbedoTexture;
layout (set = 1, binding = 8) uniform sampler2D layer2NormalMapTexture;
layout (set = 1, binding = 9) uniform sampler2D layer2MaterialPropertiesTexture;
layout (set = 1, binding = 10) uniform sampler2D layer3AlbedoTexture;
layout (set = 1, binding = 11) uniform sampler2D layer3NormalMapTexture;
layout (set = 1, binding = 12) uniform sampler2D layer3MaterialPropertiesTexture;
layout (set = 1, binding = 13) uniform sampler2D layer4AlbedoTexture;
layout (set = 1, binding = 14) uniform sampler2D layer4NormalMapTexture;
layout (set = 1, binding = 15) uniform sampler2D layer4MaterialPropertiesTexture;
layout (set = 1, binding = 16) uniform sampler2D layer5AlbedoTexture;
layout (set = 1, binding = 17) uniform sampler2D layer5NormalMapTexture;
layout (set = 1, binding = 18) uniform sampler2D layer5MaterialPropertiesTexture;

//Out parameters.
layout (location = 0) out vec4 albedoColor;
layout (location = 1) out vec4 normalDirectionDepth;
layout (location = 2) out vec4 roughnessMetallicAmbientOcclusion;

//Globals.
vec4 layer1MaterialPropertiesSampler;
vec4 layer2MaterialPropertiesSampler;
vec4 layer3MaterialPropertiesSampler;
vec4 layer4MaterialPropertiesSampler;
vec4 layer5MaterialPropertiesSampler;

/*
*	Returns the albedo.
*/
vec4 GetAlbedo()
{
    vec4 layer1Albedo = texture(layer1AlbedoTexture, fragmentTextureCoordinate);
    vec4 layer2Albedo = texture(layer2AlbedoTexture, fragmentTextureCoordinate);
    vec4 layer3Albedo = texture(layer3AlbedoTexture, fragmentTextureCoordinate);
    vec4 layer4Albedo = texture(layer4AlbedoTexture, fragmentTextureCoordinate);
    vec4 layer5Albedo = texture(layer5AlbedoTexture, fragmentTextureCoordinate);

    vec4 blend1 = mix(layer1Albedo, layer2Albedo, layerWeightsSampler.x);
    vec4 blend2 = mix(blend1, layer3Albedo, layerWeightsSampler.y);
    vec4 blend3 = mix(blend2, layer4Albedo, layerWeightsSampler.z);

    return mix(blend3, layer5Albedo, layerWeightsSampler.w);
}

/*
*	Returns the normal direction.
*/
vec3 GetNormalDirection()
{
	vec3 layer1NormalDirection = texture(layer1NormalMapTexture, fragmentTextureCoordinate).xyz;
    vec3 layer2NormalDirection = texture(layer2NormalMapTexture, fragmentTextureCoordinate).xyz;
    vec3 layer3NormalDirection = texture(layer3NormalMapTexture, fragmentTextureCoordinate).xyz;
    vec3 layer4NormalDirection = texture(layer4NormalMapTexture, fragmentTextureCoordinate).xyz;
    vec3 layer5NormalDirection = texture(layer5NormalMapTexture, fragmentTextureCoordinate).xyz;

    vec3 blend1 = mix(layer1NormalDirection, layer2NormalDirection, layerWeightsSampler.x);
    vec3 blend2 = mix(blend1, layer3NormalDirection, layerWeightsSampler.y);
    vec3 blend3 = mix(blend2, layer4NormalDirection, layerWeightsSampler.z);

    return mix(blend3, layer5NormalDirection, layerWeightsSampler.w) * 2.0f - 1.0f;
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

    float blend1 = mix(layer1Roughness, layer2Roughness, layerWeightsSampler.x);
    float blend2 = mix(blend1, layer3Roughness, layerWeightsSampler.x);
    float blend3 = mix(blend2, layer4Roughness, layerWeightsSampler.x);

    return mix(blend3, layer5Roughness, layerWeightsSampler.y);
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

    float blend1 = mix(layer1Metallic, layer2Metallic, layerWeightsSampler.x);
    float blend2 = mix(blend1, layer3Metallic, layerWeightsSampler.y);
    float blend3 = mix(blend2, layer4Metallic, layerWeightsSampler.z);

    return mix(blend3, layer5Metallic, layerWeightsSampler.w);
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

    float blend1 = mix(layer1AmbientOcclusion, layer2AmbientOcclusion, layerWeightsSampler.x);
    float blend2 = mix(blend1, layer3AmbientOcclusion, layerWeightsSampler.y);
    float blend3 = mix(blend2, layer4AmbientOcclusion, layerWeightsSampler.z);

    return mix(blend3, layer5AmbientOcclusion, layerWeightsSampler.w);
}

void main()
{
    //Sampler the material properties.
    layer1MaterialPropertiesSampler = texture(layer1MaterialPropertiesTexture, fragmentTextureCoordinate);
    layer2MaterialPropertiesSampler = texture(layer2MaterialPropertiesTexture, fragmentTextureCoordinate);
    layer3MaterialPropertiesSampler = texture(layer3MaterialPropertiesTexture, fragmentTextureCoordinate);
    layer4MaterialPropertiesSampler = texture(layer4MaterialPropertiesTexture, fragmentTextureCoordinate);
    layer5MaterialPropertiesSampler = texture(layer5MaterialPropertiesTexture, fragmentTextureCoordinate);

	//Set the albedo color.
	albedoColor = GetAlbedo();

	//Calculate the tangent space matrix.
	vec3 normal = texture(terrainPropertiesTexture, fragmentHeightMapTextureCoordinate).xyz * 2.0f - 1.0f;
	vec3 tangent = cross(vec3(0.0f, 0.0f, 1.0f), normal);
	vec3 bitangent = cross(tangent, normal);

	mat3 tangentSpaceMatrix = mat3(tangent, bitangent, normal);

	//Set the normal.
    vec3 normalDirection = GetNormalDirection();
    normalDirection = tangentSpaceMatrix * normalDirection;
    normalDirection = normalize(normalDirection);
    normalDirectionDepth = vec4(normalDirection, gl_FragCoord.z);

	//Set the roughness.
    roughnessMetallicAmbientOcclusion.r = GetRoughness();

    //Set the metallic.
    roughnessMetallicAmbientOcclusion.g = GetMetallic();

    //Set the ambient occlusion.
    roughnessMetallicAmbientOcclusion.b = GetAmbientOcclusion();

    //Write the thinness.
    roughnessMetallicAmbientOcclusion.a = 0.0f;
}