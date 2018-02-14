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

//Texture samplers.
layout (binding = 2) uniform sampler2D heightMapTexture;

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
*   Returns the middle point given two points.
*/
vec3 GetMiddlePoint(vec3 point1, vec3 point2)
{
    return (point1 + point2) / 2.0f;
}

/*
*   Returns the middle point given three points.
*/
vec3 GetMiddlePoint(vec3 point1, vec3 point2, vec3 point3)
{
    return (point1 + point2 + point3) / 3.0f;
}

/*
*   Given a distance to the camera squared, return the proper tesselation level.
*/
float GetTesselationLevel(float distanceToCameraSquared)
{
    float tesselationLevel = 1.0f;

    tesselationLevel = distanceToCameraSquared < 300.0f * 300.0f ? 4.0f : tesselationLevel;
    tesselationLevel = distanceToCameraSquared < 200.0f * 200.0f ? 16.0f : tesselationLevel;
    tesselationLevel = distanceToCameraSquared < 100.0f * 100.0f ? 64.0f : tesselationLevel;

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
        vec3 position1 = tessellationControlPosition[0] + vec3(0.0f, texture(heightMapTexture, tessellationEvaluationHeightMapTextureCoordinate[0]).r * terrainHeight, 0.0f);
        vec3 position2 = tessellationControlPosition[1] + vec3(0.0f, texture(heightMapTexture, tessellationEvaluationHeightMapTextureCoordinate[1]).r * terrainHeight, 0.0f);
        vec3 position3 = tessellationControlPosition[2] + vec3(0.0f, texture(heightMapTexture, tessellationEvaluationHeightMapTextureCoordinate[2]).r * terrainHeight, 0.0f);

        vec3 middlePoint1 = GetMiddlePoint(position2, position3);
        vec3 middlePoint2 = GetMiddlePoint(position3, position1);
        vec3 middlePoint3 = GetMiddlePoint(position1, position2);

        vec3 middleOfTriangle = GetMiddlePoint(middlePoint1, middlePoint2, middlePoint3);

        gl_TessLevelInner[0] = GetTesselationLevel(LengthSquared(cameraWorldPosition - middleOfTriangle));
        gl_TessLevelOuter[0] = GetTesselationLevel(LengthSquared(cameraWorldPosition - middlePoint1));
        gl_TessLevelOuter[1] = GetTesselationLevel(LengthSquared(cameraWorldPosition - middlePoint2));
        gl_TessLevelOuter[2] = GetTesselationLevel(LengthSquared(cameraWorldPosition - middlePoint3));
    }
}