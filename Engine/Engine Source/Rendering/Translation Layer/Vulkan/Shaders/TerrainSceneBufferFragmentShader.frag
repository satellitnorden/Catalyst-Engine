//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, set = 0, binding = 0) uniform DynamicUniformData
{
    //Camera data.
    float cameraFieldOfViewCosine;
    mat4 inverseCameraMatrix;
    mat4 inverseProjectionMatrix;
    mat4 originViewMatrix;
    mat4 viewMatrix;
    vec3 cameraForwardVector;
    vec3 cameraWorldPosition;
    float padding1;

    //Directional light data.
    float directionalLightIntensity;
    vec3 directionalLightDirection;
    vec3 directionalLightColor;
    vec3 directionalLightScreenSpacePosition;
    float padding2;

    //Point light data.
    int numberOfPointLights;
    float pointLightAttenuationDistances[MaximumNumberOfPointLights];
    float pointLightIntensities[MaximumNumberOfPointLights];
    vec3 pointLightColors[MaximumNumberOfPointLights];
    vec3 pointLightWorldPositions[MaximumNumberOfPointLights];

    //Spot light data.
    int numberOfSpotLights;
    float spotLightAttenuationDistances[MaximumNumberOfSpotLights];
    float spotLightIntensities[MaximumNumberOfSpotLights];
    float spotLightInnerCutoffAngles[MaximumNumberOfSpotLights];
    float spotLightOuterCutoffAngles[MaximumNumberOfSpotLights];
    vec3 spotLightColors[MaximumNumberOfSpotLights];
    vec3 spotLightDirections[MaximumNumberOfSpotLights];
    vec3 spotLightWorldPositions[MaximumNumberOfSpotLights];
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
}