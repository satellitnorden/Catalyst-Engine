//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, binding = 0) uniform DynamicUniformData
{
    //Camera data.
    float cameraFieldOfViewRadians;
    mat4 inverseCameraMatrix;
    mat4 inverseProjectionMatrix;
    mat4 originViewMatrix;
    mat4 viewMatrix;
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
	vec3 terrainPosition;
};

//In parameters.
layout (location = 0) in vec2 fragmentHeightMapTextureCoordinate;
layout (location = 1) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (binding = 4) uniform sampler2D terrainNormalTexture;
layout (binding = 5) uniform sampler2D albedoTexture;
layout (binding = 6) uniform sampler2D normalMapTexture;
layout (binding = 7) uniform sampler2D roughnessTexture;
layout (binding = 8) uniform sampler2D metallicTexture;
layout (binding = 9) uniform sampler2D ambientOcclusionTexture;

//Out parameters.
layout (location = 0) out vec4 albedoColor;
layout (location = 1) out vec4 normalDirectionDepth;
layout (location = 2) out vec4 roughnessMetallicAmbientOcclusion;

void main()
{
	//Set the albedo color.
	albedoColor = texture(albedoTexture, fragmentTextureCoordinate);

	//Calculate the tangent space matrix.
	vec3 normal = texture(terrainNormalTexture, fragmentHeightMapTextureCoordinate).xyz * 2.0f - 1.0f;
	vec3 tangent = cross(vec3(0.0f, 0.0f, 1.0f), normal);
	vec3 bitangent = cross(tangent, normal);

	mat3 tangentSpaceMatrix = mat3(tangent, bitangent, normal);

	//Set the normal.
    vec3 normalDirection = texture(normalMapTexture, fragmentTextureCoordinate).xyz * 2.0f - 1.0f;
    normalDirection = tangentSpaceMatrix * normalDirection;
    normalDirection = normalize(normalDirection);
    normalDirectionDepth = vec4(normalDirection, gl_FragCoord.z);

	//Set the roughness.
    roughnessMetallicAmbientOcclusion.r = texture(roughnessTexture, fragmentTextureCoordinate).r;

    //Set the metallic.
    roughnessMetallicAmbientOcclusion.g = texture(metallicTexture, fragmentTextureCoordinate).r;

    //Set the ambient occlusion.
    roughnessMetallicAmbientOcclusion.b = texture(ambientOcclusionTexture, fragmentTextureCoordinate).r;
}