//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define MAX_TESSELLATION_LEVEL 16.0f

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