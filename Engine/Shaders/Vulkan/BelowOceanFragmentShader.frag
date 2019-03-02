//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 oceanColor;
    layout (offset = 16) float oceanDeformationWeightDistanceSquared;
    layout (offset = 20) float oceanColorWeightDistanceSquared;
    layout (offset = 24) float oceanFoamWeightDistanceSquared;
    layout (offset = 28) float oceanNormalSmoothingFactor;
    layout (offset = 32) float oceanTextureScaling;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform samplerCube nightTexture;
layout (set = 2, binding = 0) uniform samplerCube dayTexture;
layout (set = 3, binding = 0) uniform sampler2D sceneTexture;
layout (set = 3, binding = 1) uniform sampler2D sceneNormalDepthTexture;
layout (set = 4, binding = 0) uniform sampler2D oceanNormalTexture;
layout (set = 4, binding = 1) uniform sampler2D oceanFoamTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;
layout (location = 1) out vec4 normalDepth;

//Globals.
vec4 normalDepthTextureSampler;
vec4 belowOceanNormalDepth;

//Forward declarations.
vec3 CalculateAboveOceanFragment(vec3 sceneWorldPosition);
vec3 CalculateBelowOceanFragment(vec3 sceneWorldPosition);
float CalculateDeformationWeight(float distanceToEndPointSquared);
vec2 CalculateEndPointTextureCoordinate(vec3 endPoint);
vec3 CalculateIntersectionPoint(vec3 pointOnPlane, vec3 pointOnLine, vec3 normal, vec3 line);
vec3 CalculateNormal(vec2 endPointTextureCoordinate);
vec3 CalculateOceanColor(vec3 reflection);
float CalculateOceanColorWeight(float distanceToEndPointSquared);
vec3 CalculateOceanFragment(vec3 endPoint);
vec3 CalculateSceneColor(vec3 normal, float distanceToIntersectionPointSquared);
vec2 CalculateSceneTextureCoordinate(vec3 normal, float distanceToIntersectionPointSquared);
vec3 CalculateSceneWorldPosition();
float LengthSquared(vec3 vector);

/*
*   Calculates the fragment when the scene world position is above the ocean.
*/
vec3 CalculateAboveOceanFragment(vec3 sceneWorldPosition)
{
    //Calculate the view direction.
    vec3 viewDirection = normalize(perceiverWorldPosition - sceneWorldPosition);

    //Calculate the intersection point.
    vec3 intersectionPoint = CalculateIntersectionPoint(vec3(0.0f, 0.0f, 0.0f), perceiverWorldPosition, vec3(0.0f, -1.0f, 0.0f), viewDirection);

    //Calculate the ocean fragment.
    return CalculateOceanFragment(intersectionPoint);
}

/*
*   Calculates the fragment when the scene world position is below the ocean.
*/
vec3 CalculateBelowOceanFragment(vec3 sceneWorldPosition)
{
    //Calculate the ocean fragment.
    return CalculateOceanFragment(sceneWorldPosition);
}

/*
*   Calculates the deformation weight.
*/
float CalculateDeformationWeight(float distanceToEndPointSquared)
{
    //Calculate the deformation weight.
    return Scale(min(distanceToEndPointSquared / oceanDeformationWeightDistanceSquared, 1.0f), 0.0f, 1.0f, 0.25f, 1.0f);
}

/*
*   Calculates the end point texture coordinate.
*/
vec2 CalculateEndPointTextureCoordinate(vec3 endPoint)
{
    //Calculate the intersection point texture coordinate.
    return endPoint.xz * oceanTextureScaling + vec2(-windDirection.x, -windDirection.z) * windSpeed * totalTime * 0.01f;
}

/*
*   Calculates a line-plane intersection point.
*/
vec3 CalculateIntersectionPoint(vec3 pointOnPlane, vec3 pointOnLine, vec3 normal, vec3 line)
{
    return dot(pointOnPlane - pointOnLine, normal) / dot(line, normal) * line + pointOnLine;
}

/*
*   Calculates the normal.
*/
vec3 CalculateNormal(vec2 endPointTextureCoordinate)
{
    //Sample the ocean normal.
    vec3 normal = texture(oceanNormalTexture, endPointTextureCoordinate).xzy;
    normal = normal * 2.0f - 1.0f;

    return mix(normal, vec3(0.0f, 1.0f, 0.0f), oceanNormalSmoothingFactor);
}

/*
*   Calculates the ocean color.
*/
vec3 CalculateOceanColor(vec3 reflection)
{
    //Calculate the ocean color.
    return oceanColor * (reflection + directionalLightColor * (directionalLightIntensity * 0.25f));
}

/*
*   Calculates the ocean color weight.
*/
float CalculateOceanColorWeight(float distanceToBottomSquared)
{
    //Calculate the ocean color weight.
    float oceanColorWeight = Scale(min(distanceToBottomSquared / oceanColorWeightDistanceSquared, 1.0f), 0.0f, 1.0f, 0.1f, 1.0f);
    return oceanColorWeight * oceanColorWeight;
}

/*
*   Calculates the ocean fragment.
*/
vec3 CalculateOceanFragment(vec3 endPoint)
{
    //Calculate the distance to the end point squared.
    float distanceToEndPointSquared = LengthSquared(endPoint - perceiverWorldPosition);

    //Calculate the end point texture coordinate.
    vec2 endPointTextureCoordinate = CalculateEndPointTextureCoordinate(endPoint);

    //Calculate the normal.
    vec3 normal = CalculateNormal(endPointTextureCoordinate);

    //Calculate the reflection.
    vec3 reflection = mix(texture(nightTexture, normal).rgb, texture(dayTexture, normal).rgb, environmentBlend);

    //Calculate the ocean color.
    vec3 calculatedOceanColor = CalculateOceanColor(reflection);

    //Calculate the ocean color weight.
    float oceanColorWeight = CalculateOceanColorWeight(distanceToEndPointSquared);

    //Calculate the scene color.
    vec3 sceneColor = CalculateSceneColor(normal, distanceToEndPointSquared);

    //Calculate the depth.
    vec4 screenSpacePosition = viewMatrix * vec4(endPoint, 1.0f);
    float depth = screenSpacePosition.z / screenSpacePosition.w;

    belowOceanNormalDepth = vec4(normal, depth);

    //Calculate the below ocean fragment.
    return mix(sceneColor, calculatedOceanColor, oceanColorWeight);
}

/*
*   Calculates the scene color.
*/
vec3 CalculateSceneColor(vec3 normal, float distanceToEndPointSquared)
{
    //Calculate the scene texture coordinate.
    vec2 sceneTextureCoordinate = CalculateSceneTextureCoordinate(normal, distanceToEndPointSquared);

    //Calculate the above ocean scene color.
    return texture(sceneTexture, sceneTextureCoordinate).rgb;
}

/*
*   Calculates the scene texture coordinate
*/
vec2 CalculateSceneTextureCoordinate(vec3 normal, float distanceToEndPointSquared)
{
    //Calculate the above ocean scene texture coordinate.
    return mix(fragmentTextureCoordinate, fragmentTextureCoordinate + normal.xz, CalculateDeformationWeight(distanceToEndPointSquared));
}

/*
*   Calculates the scene world position.
*/
vec3 CalculateSceneWorldPosition()
{
    //Calculate the scene world position.
    return CalculateFragmentWorldPosition(fragmentTextureCoordinate, texture(sceneNormalDepthTexture, fragmentTextureCoordinate).w);
}

/*
*   Returns the length of a vector squared.
*/
float LengthSquared(vec3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

void main()
{
    //Calculate the scene world position.
    vec3 sceneWorldPosition = CalculateSceneWorldPosition();

    //Based on whether or not the scene world position is above or below the ocean, render accordingly.
    if (sceneWorldPosition.y > 0.0f)
    {
        fragment = vec4(CalculateAboveOceanFragment(sceneWorldPosition), 1.0f);
        normalDepth = belowOceanNormalDepth;
    }

    else
    {
        fragment = vec4(CalculateBelowOceanFragment(sceneWorldPosition), 1.0f);
        normalDepth = belowOceanNormalDepth;
    }
}