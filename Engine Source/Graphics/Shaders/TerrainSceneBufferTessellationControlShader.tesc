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
	float terrainDisplacementHeight;
	float terrainHeight;
    float terrainSize;
	vec3 terrainPosition;
};

//Layout specification.
layout (vertices = 3) out;

//In parameters.
layout (location = 0) in float tesselationControlDistanceToCameraSquared[];
layout (location = 1) in vec2 tessellationControlHeightMapTextureCoordinate[];
layout (location = 2) in vec2 tessellationControlTextureCoordinate[];
layout (location = 3) in vec3 tessellationControlPosition[];

//Out parameters.
layout (location = 0) out vec2 tessellationEvaluationHeightMapTextureCoordinate[];
layout (location = 1) out vec2 tessellationEvaluationTextureCoordinate[];
layout (location = 2) out vec3 tessellationEvaluationPosition[];

/*
*   Given a distance to the camera squared, return the proper tesselation level.
*/
float GetTesselationLevel(float distanceSquared)
{
    float tesselationLevel = 1.0f;

    tesselationLevel = distanceSquared < 256.0f * 256.0f ? 2.0f : tesselationLevel;
    tesselationLevel = distanceSquared < 128.0f * 128.0f ? 4.0f : tesselationLevel;
    tesselationLevel = distanceSquared < 64.0f * 64.0f ? 16.0f : tesselationLevel;
    tesselationLevel = distanceSquared < 32.0f * 32.0f ? 64.0f : tesselationLevel;

    return tesselationLevel;
}

void main()
{   
    //Pass along the values to the tessellation evaluation shader.
    tessellationEvaluationHeightMapTextureCoordinate[gl_InvocationID] = tessellationControlHeightMapTextureCoordinate[gl_InvocationID];
    tessellationEvaluationTextureCoordinate[gl_InvocationID] = tessellationControlTextureCoordinate[gl_InvocationID];
    tessellationEvaluationPosition[gl_InvocationID] = tessellationControlPosition[gl_InvocationID];

    //Calculate tht tessellation levels.
    if (gl_InvocationID == 0)
    {
        float averageDistanceToCameraSquared = (tesselationControlDistanceToCameraSquared[0] + tesselationControlDistanceToCameraSquared[1] + tesselationControlDistanceToCameraSquared[2]) / 3.0f;
        float tesselationLevel = GetTesselationLevel(averageDistanceToCameraSquared);

        gl_TessLevelInner[0] = tesselationLevel;
        gl_TessLevelOuter[0] = 32;
        gl_TessLevelOuter[1] = 32;
        gl_TessLevelOuter[2] = 32;
    }
}