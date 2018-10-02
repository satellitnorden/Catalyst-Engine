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
    layout (offset = 320) mat4 directionalLightViewMatrix; //Offset; 320 - Size; 64
    layout (offset = 384) vec3 directionalLightDirection; //Offset; 384 - Size; 16
    layout (offset = 400) vec3 directionalLightColor; //Offset; 400 - Size; 16
    layout (offset = 416) vec3 directionalLightScreenSpacePosition; //Offset; 416 - Size; 16

    //Environment data.
    layout (offset = 432) float environmentBlend; //Offset; 432 - Size; 4

    //General data.
    layout (offset = 436) float deltaTime; //Offset; 436 - Size; 4
    layout (offset = 440) float totalGameTime; //Offset; 440 - Size; 4

    //Point light data.
    layout (offset = 444) int numberOfPointLights; //Offset; 444 - Size; 4
    layout (offset = 448) float pointLightAttenuationDistances[MaximumNumberOfPointLights]; //Offset; 448 - Size; 128
    layout (offset = 576) float pointLightIntensities[MaximumNumberOfPointLights]; //Offset; 576 - Size; 128
    layout (offset = 704) vec3 pointLightColors[MaximumNumberOfPointLights]; //Offset; 704 - Size; 128
    layout (offset = 832) vec3 pointLightWorldPositions[MaximumNumberOfPointLights]; //Offset; 832 - Size; 128

    //Spot light data.
    layout (offset = 960) int numberOfSpotLights; //Offset; 960 - Size; 16
    layout (offset = 976) float spotLightAttenuationDistances[MaximumNumberOfSpotLights]; //Offset; 976 - Size; 128
    layout (offset = 1104) float spotLightIntensities[MaximumNumberOfSpotLights]; //Offset; 1104 - Size; 128
    layout (offset = 1232) float spotLightInnerCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1232 - Size; 128
    layout (offset = 1360) float spotLightOuterCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1360 - Size; 128
    layout (offset = 1488) vec3 spotLightColors[MaximumNumberOfSpotLights]; //Offset; 1488 - Size; 128
    layout (offset = 1616) vec3 spotLightDirections[MaximumNumberOfSpotLights]; //Offset; 1616 - Size; 128
    layout (offset = 1744) vec3 spotLightWorldPositions[MaximumNumberOfSpotLights]; //Offset; 1744 - Size; 128

    //Physics data.
    layout (offset = 1872) float windSpeed; //Offset; 1872 - Size; 16
    layout (offset = 1888) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1904
};

//Preprocessor defines.
#define MAX_TESSELLATION_LEVEL 32.0f

//Layout specification.
layout (vertices = 3) out;

//In parameters.
layout (location = 0) in vec3 tessellationControlPosition[];
layout (location = 1) in vec3 tessellationControlNormal[];
layout (location = 2) in vec4 tessellationControlLayerWeights[];
layout (location = 3) in vec2 tessellationControlTextureCoordinate[];

//Out parameters.
layout (location = 0) out vec3 tessellationEvaluationPosition[];
layout (location = 1) out vec3 tessellationEvaluationNormal[];
layout (location = 2) out vec4 tessellationEvaluationLayerWeights[];
layout (location = 3) out vec2 tessellationEvaluationTextureCoordinate[];

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
float GetTesselationLevel(float distanceToCamera)
{
    return max(MAX_TESSELLATION_LEVEL - distanceToCamera, 1.0f);
}

void main()
{   
    //Pass information to the tessellation evaluation shader.
    tessellationEvaluationPosition[gl_InvocationID] = tessellationControlPosition[gl_InvocationID];
    tessellationEvaluationNormal[gl_InvocationID] = tessellationControlNormal[gl_InvocationID];
    tessellationEvaluationLayerWeights[gl_InvocationID] = tessellationControlLayerWeights[gl_InvocationID];
    tessellationEvaluationTextureCoordinate[gl_InvocationID] = tessellationControlTextureCoordinate[gl_InvocationID];

    //Calculate tht tessellation levels.
    if (gl_InvocationID == 0)
    {
    	/*
        vec3 position1 = tessellationControlPosition[0];
        vec3 position2 = tessellationControlPosition[1];
        vec3 position3 = tessellationControlPosition[2];

        vec3 middlePoint1 = GetMiddlePoint(position2, position3);
        vec3 middlePoint2 = GetMiddlePoint(position3, position1);
        vec3 middlePoint3 = GetMiddlePoint(position1, position2);

        vec3 middleOfTriangle = GetMiddlePoint(middlePoint1, middlePoint2, middlePoint3);

        gl_TessLevelInner[0] = GetTesselationLevel(length(cameraWorldPosition - middleOfTriangle));
        gl_TessLevelOuter[0] = GetTesselationLevel(length(cameraWorldPosition - middlePoint1));
        gl_TessLevelOuter[1] = GetTesselationLevel(length(cameraWorldPosition - middlePoint2));
        gl_TessLevelOuter[2] = GetTesselationLevel(length(cameraWorldPosition - middlePoint3));
        */

        gl_TessLevelInner[0] = 1.0f;
        gl_TessLevelOuter[0] = 1.0f;
        gl_TessLevelOuter[1] = 1.0f;
        gl_TessLevelOuter[2] = 1.0f;
    }
}