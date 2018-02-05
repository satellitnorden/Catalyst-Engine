//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, binding = 0) uniform DynamicUniformData
{
    //Camera data.
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
    float terrainSize;
	vec3 terrainPosition;
};

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTextureCoordinate;

//Out parameters.
layout (location = 0) out float tesselationControlDistanceToCameraSquared;
layout (location = 1) out vec2 tessellationControlHeightMapTextureCoordinate;
layout (location = 2) out vec2 tessellationControlTextureCoordinate;
layout (location = 3) out vec3 tessellationControlPosition;

/*
*	Returns the squared 2D length of a vector, not taking height into account.
*/
float Squared2DLength(vec3 vector)
{
	return vector.x * vector.x + vector.z * vector.z;
}

void main()
{	
	//Pass the texture coordinate to the tesselation control shader.
	tessellationControlHeightMapTextureCoordinate = vertexTextureCoordinate;
	tessellationControlTextureCoordinate = tessellationControlHeightMapTextureCoordinate * (terrainSize * 0.5f);

	vec3 finalVertexPosition = vertexPosition * (terrainSize * 0.5f) + terrainPosition;

	tesselationControlDistanceToCameraSquared = Squared2DLength(cameraWorldPosition - finalVertexPosition);

	//Pass the unmodified vertex position to the tesselation control shader.
	tessellationControlPosition = finalVertexPosition;
}