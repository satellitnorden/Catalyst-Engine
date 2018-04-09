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
*   Returns the length of a vector squared, ignoring the Y component.
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

    tesselationLevel = distanceToCameraSquared < 250000.0f ? 2.0f : tesselationLevel;   //500 meters.
    tesselationLevel = distanceToCameraSquared < 160000.0f ? 4.0f : tesselationLevel;   //400 meters.
    tesselationLevel = distanceToCameraSquared < 90000.0f ? 8.0f : tesselationLevel;    //300 meters.
    tesselationLevel = distanceToCameraSquared < 40000.0f ? 16.0f : tesselationLevel;   //200 meters.
    tesselationLevel = distanceToCameraSquared < 10000.0f ? 32.0f : tesselationLevel;   //100 meters.
    tesselationLevel = distanceToCameraSquared < 2500.0f ? 64.0f : tesselationLevel;    //50 meters.

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
        vec3 position1 = tessellationControlPosition[0];
        vec3 position2 = tessellationControlPosition[1];
        vec3 position3 = tessellationControlPosition[2];

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