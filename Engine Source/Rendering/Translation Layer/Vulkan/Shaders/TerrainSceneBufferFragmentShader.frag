//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, binding = 0) uniform DynamicUniformData
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
layout (std140, binding = 1) uniform TerrainUniformData
{
    float terrainDisplacementHeight;
    float terrainHeight;
    float terrainSize;
    float terrainTextureTilingFactor;
    vec3 terrainPosition;
};

//In parameters.
layout (location = 0) in float fragmentLayer1Weight;
layout (location = 1) in float fragmentLayer2Weight;
layout (location = 2) in vec2 fragmentHeightMapTextureCoordinate;
layout (location = 3) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (binding = 3) uniform sampler2D terrainNormalTexture;
layout (binding = 5) uniform sampler2D layer1AlbedoTexture;
layout (binding = 6) uniform sampler2D layer1NormalMapTexture;
layout (binding = 7) uniform sampler2D layer1RoughnessTexture;
layout (binding = 8) uniform sampler2D layer1MetallicTexture;
layout (binding = 9) uniform sampler2D layer1AmbientOcclusionTexture;
layout (binding = 12) uniform sampler2D layer2AlbedoTexture;
layout (binding = 13) uniform sampler2D layer2NormalMapTexture;
layout (binding = 14) uniform sampler2D layer2RoughnessTexture;
layout (binding = 15) uniform sampler2D layer2MetallicTexture;
layout (binding = 16) uniform sampler2D layer2AmbientOcclusionTexture;
layout (binding = 19) uniform sampler2D layer3AlbedoTexture;
layout (binding = 20) uniform sampler2D layer3NormalMapTexture;
layout (binding = 21) uniform sampler2D layer3RoughnessTexture;
layout (binding = 22) uniform sampler2D layer3MetallicTexture;
layout (binding = 23) uniform sampler2D layer3AmbientOcclusionTexture;

//Out parameters.
layout (location = 0) out vec4 albedoColor;
layout (location = 1) out vec4 normalDirectionDepth;
layout (location = 2) out vec4 roughnessMetallicAmbientOcclusion;

/*
*	Returns the albedo.
*/
vec4 GetAlbedo()
{
    vec4 layer1Albedo = texture(layer1AlbedoTexture, fragmentTextureCoordinate);
    vec4 layer2Albedo = texture(layer2AlbedoTexture, fragmentTextureCoordinate);
    vec4 layer3Albedo = texture(layer3AlbedoTexture, fragmentTextureCoordinate);

    vec4 blend1 = mix(layer2Albedo, layer1Albedo, fragmentLayer1Weight);

    return mix(layer3Albedo, blend1, fragmentLayer2Weight);
}

/*
*	Returns the normal direction.
*/
vec3 GetNormalDirection()
{
	vec3 layer1NormalDirection = texture(layer1NormalMapTexture, fragmentTextureCoordinate).xyz;
    vec3 layer2NormalDirection = texture(layer2NormalMapTexture, fragmentTextureCoordinate).xyz;
    vec3 layer3NormalDirection = texture(layer3NormalMapTexture, fragmentTextureCoordinate).xyz;

    vec3 blend1 = mix(layer2NormalDirection, layer1NormalDirection, fragmentLayer1Weight);

    return mix(layer3NormalDirection, blend1, fragmentLayer2Weight) * 2.0f - 1.0f;
}

/*
*	Returns the roughness.
*/
float GetRoughness()
{
	float layer1Roughness = texture(layer1RoughnessTexture, fragmentTextureCoordinate).r;
    float layer2Roughness = texture(layer2RoughnessTexture, fragmentTextureCoordinate).r;
    float layer3Roughness = texture(layer3RoughnessTexture, fragmentTextureCoordinate).r;

    float blend1 = mix(layer2Roughness, layer1Roughness, fragmentLayer1Weight);

    return mix(layer3Roughness, blend1, fragmentLayer2Weight);
}

/*
*	Returns the metallic.
*/
float GetMetallic()
{
	float layer1Metallic = texture(layer1MetallicTexture, fragmentTextureCoordinate).r;
    float layer2Metallic = texture(layer2MetallicTexture, fragmentTextureCoordinate).r;
    float layer3Metallic = texture(layer3MetallicTexture, fragmentTextureCoordinate).r;

    float blend1 = mix(layer2Metallic, layer1Metallic, fragmentLayer1Weight);

    return mix(layer3Metallic, blend1, fragmentLayer2Weight);
}

/*
*	Returns the ambient occlusion.
*/
float GetAmbientOcclusion()
{
	float layer1AmbientOcclusion = texture(layer1AmbientOcclusionTexture, fragmentTextureCoordinate).r;
    float layer2AmbientOcclusion = texture(layer2AmbientOcclusionTexture, fragmentTextureCoordinate).r;
    float layer3AmbientOcclusion = texture(layer3AmbientOcclusionTexture, fragmentTextureCoordinate).r;

    float blend1 = mix(layer2AmbientOcclusion, layer1AmbientOcclusion, fragmentLayer1Weight);

    return mix(layer3AmbientOcclusion, blend1, fragmentLayer2Weight);
}

void main()
{
	//Set the albedo color.
	albedoColor = GetAlbedo();

	//Calculate the tangent space matrix.
	vec3 normal = texture(terrainNormalTexture, fragmentHeightMapTextureCoordinate).xyz * 2.0f - 1.0f;
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