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

//Layout specification.
layout (vertices = 3) out;

//In parameters.
layout (location = 0) in vec2 tessellationControlHeightMapTextureCoordinate[];
layout (location = 1) in vec2 tessellationControlTextureCoordinate[];
layout (location = 2) in vec3 tessellationControlPosition[];

//Out parameters.
layout (location = 0) out vec2 tessellationEvaluationHeightMapTextureCoordinate[];
layout (location = 1) out vec2 tessellationEvaluationTextureCoordinate[];
layout (location = 2) out vec3 tessellationEvaluationPosition[];

/*
*   Returns the length of a vector squared.
*/
float LengthSquared(vec3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

/*
*   Given a distance to the camera squared, return the proper tesselation level.
*/
float GetTesselationLevel()
{
    //Determine the distance from the camera of this triangle, squared.
    vec3 middleOfTriangle = (tessellationEvaluationPosition[0] + tessellationEvaluationPosition[1] + tessellationEvaluationPosition[2]) / 3.0f;
    float distanceToCameraSquared = LengthSquared(middleOfTriangle - cameraWorldPosition);

    float tesselationLevel = 1.0f;

    tesselationLevel = distanceToCameraSquared < 512.0f * 512.0f ? 2.0f : tesselationLevel;
    tesselationLevel = distanceToCameraSquared < 256.0f * 256.0f ? 4.0f : tesselationLevel;
    tesselationLevel = distanceToCameraSquared < 128.0f * 128.0f ? 8.0f : tesselationLevel;
    tesselationLevel = distanceToCameraSquared < 64.0f * 64.0f ? 16.0f : tesselationLevel;
    tesselationLevel = distanceToCameraSquared < 32.0f * 32.0f ? 32.0f : tesselationLevel;

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
        gl_TessLevelInner[0] = GetTesselationLevel();
        gl_TessLevelOuter[0] = 32.0f;
        gl_TessLevelOuter[1] = 32.0f;
        gl_TessLevelOuter[2] = 32.0f; 
    }
}