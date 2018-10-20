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

//Forward declarations.
vec3 CalculateAboveOceanFragment();
vec3 CalculateBelowOceanFragment();
float CalculateDeformationWeight(float distanceToBottomSquared);
vec4 CalculateFoamColor(vec2 intersectionPointTextureCoordinate, vec3 reflection);
float CalculateFoamWeight(float distanceToBottomSquared);
vec3 CalculateDirectionalLight(vec3 reflectionDirection);
vec3 CalculateIntersectionPoint(vec3 pointOnPlane, vec3 pointOnLine, vec3 normal, vec3 line);
vec2 CalculateIntersectionPointTextureCoordinate(vec3 intersectionPoint);
vec3 CalculateNormal(vec2 intersectionPointTextureCoordinate);
vec3 CalculateOceanColor(vec3 reflection);
float CalculateOceanColorWeight(float distanceToBottomSquared);
vec3 CalculateSceneColor(vec3 normal, float distanceToBottomSquared);
vec2 CalculateSceneTextureCoordinate(vec3 normal, float distanceToBottomSquared);
vec3 CalculateSceneWorldPosition();
vec3 CalculateWorldPosition(vec2 textureCoordinate, float depth);
float Scale(float value, float originalMinimum, float originalMaximum, float newMinimum, float newMaximum);

/*
*   Calculates the fragment when the scene world position is above the ocean.
*/
vec3 CalculateAboveOceanFragment()
{
    //The fragment should simply be the scene texture.
    return texture(sceneTexture, fragmentTextureCoordinate).rgb;
}

/*
*   Calculates the fragment when the scene world position is below the ocean.
*/
vec3 CalculateBelowOceanFragment(vec3 sceneWorldPosition)
{
    //Calculate the view direction.
    vec3 viewDirection = normalize(sceneWorldPosition - cameraWorldPosition);

    //Calculate the intersection point.
    vec3 intersectionPoint = CalculateIntersectionPoint(vec3(0.0f, 0.0f, 0.0f), cameraWorldPosition, vec3(0.0f, -1.0f, 0.0f), viewDirection);

    //Calculate the distance to the bottom squared.
    float distanceToBottomSquared = LengthSquared3(sceneWorldPosition - intersectionPoint);

    //Calculate the intersection point texture coordinate.
    vec2 intersectionPointTextureCoordinate = CalculateIntersectionPointTextureCoordinate(intersectionPoint);

    //Calculate the normal.
    vec3 normal = CalculateNormal(intersectionPointTextureCoordinate);

    //Calculate the scene color.
    vec3 sceneColor = CalculateSceneColor(normal, distanceToBottomSquared);

    //Calculate the reflection direction.
    vec3 reflectionDirection = reflect(viewDirection, normal);

    //Calculate the reflection.
    vec3 reflection = mix(texture(nightTexture, reflectionDirection).rgb, texture(dayTexture, reflectionDirection).rgb, environmentBlend);

    //Calculate the ocean color.
    vec3 calculatedOceanColor = CalculateOceanColor(mix(texture(nightTexture, normal).rgb, texture(dayTexture, normal).rgb, environmentBlend));

    //Calculate the ocean color weight.
    float oceanColorWeight = CalculateOceanColorWeight(distanceToBottomSquared);

    //Calculate the underwater color.
    vec3 underwaterColor = mix(sceneColor, calculatedOceanColor, oceanColorWeight);

    //Calculate the transparency.
    float transparency = max(dot(normal, -viewDirection), 0.0f);

    //Calculate the final ocean color.
    vec3 finalOceanColor = mix(reflection, underwaterColor, transparency);

    //Calculate the foam weight.
    float foamWeight = CalculateFoamWeight(distanceToBottomSquared);

    //Calculate the foam color.
    vec4 foamColor = CalculateFoamColor(intersectionPointTextureCoordinate, reflection);

    //Mix in the foam color with the final ocean color.
    finalOceanColor = mix(finalOceanColor, foamColor.rgb, foamWeight * foamColor.a);

    //Apply directional lighting.
    finalOceanColor += CalculateDirectionalLight(reflectionDirection);

    return finalOceanColor;
}

/*
*   Calculates the deformation weight.
*/
float CalculateDeformationWeight(float distanceToBottomSquared)
{
    //Calculate the deformation weight.
    return Scale(min(distanceToBottomSquared / oceanDeformationWeightDistanceSquared, 1.0f), 0.0f, 1.0f, 0.25f, 1.0f);
}

/*
*	Calculates the foam color.
*/
vec4 CalculateFoamColor(vec2 intersectionPointTextureCoordinate, vec3 reflection)
{
    //Sample the foam texture.
    vec4 oceanFoamTextureSampler = texture(oceanFoamTexture, intersectionPointTextureCoordinate);

    return vec4(oceanFoamTextureSampler.rgb * (reflection + directionalLightColor * min(directionalLightIntensity, 1.0f)), oceanFoamTextureSampler.a);
}

/*
*   Calculates the foam weight.
*/
float CalculateFoamWeight(float distanceToBottomSquared)
{
	//Calculate the foam weight.
    return 1.0f - min(distanceToBottomSquared / oceanFoamWeightDistanceSquared, 1.0f);
}

/*
*   Calculates the directional light.
*/
vec3 CalculateDirectionalLight(vec3 reflectionDirection)
{
    return directionalLightColor * directionalLightIntensity * pow(max(dot(-directionalLightDirection, reflectionDirection), 0.0f), 256.0f);
}

/*
*   Calculates a line-plane intersection point.
*/
vec3 CalculateIntersectionPoint(vec3 pointOnPlane, vec3 pointOnLine, vec3 normal, vec3 line)
{
    return dot(pointOnPlane - pointOnLine, normal) / dot(line, normal) * line + pointOnLine;
}

/*
*	Calculates the intersection point texture coordinate.
*/
vec2 CalculateIntersectionPointTextureCoordinate(vec3 intersectionPoint)
{
	//Calculate the intersection point texture coordinate.
	return intersectionPoint.xz * oceanTextureScaling + vec2(-windDirection.x, -windDirection.z) * windSpeed * totalGameTime * 0.01f;
}

/*
*	Calculates the normal.
*/
vec3 CalculateNormal(vec2 intersectionPointTextureCoordinate)
{
    //Sample the ocean normal.
    vec3 normal = texture(oceanNormalTexture, intersectionPointTextureCoordinate).xzy;
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
*   Calculates the scene color.
*/
vec3 CalculateSceneColor(vec3 normal, float distanceToBottomSquared)
{
    //Calculate the scene texture coordinate.
    vec2 sceneTextureCoordinate = CalculateSceneTextureCoordinate(normal, distanceToBottomSquared);

    //Calculate the above ocean scene color.
    return texture(sceneTexture, sceneTextureCoordinate).rgb;
}

/*
*   Calculates the scene texture coordinate
*/
vec2 CalculateSceneTextureCoordinate(vec3 normal, float distanceToBottomSquared)
{
    //Calculate the above ocean scene texture coordinate.
    return mix(fragmentTextureCoordinate, fragmentTextureCoordinate + normal.xz, CalculateDeformationWeight(distanceToBottomSquared));
}

/*
*   Calculates the scene world position.
*/
vec3 CalculateSceneWorldPosition()
{
    //Sample the depth of the scene at this point.
    float sceneDepth = texture(sceneNormalDepthTexture, fragmentTextureCoordinate).w;

    //Calculate the scene world position.
    return CalculateWorldPosition(fragmentTextureCoordinate, sceneDepth);
}

/*
*   Calculates the world position.
*/
vec3 CalculateWorldPosition(vec2 textureCoordinate, float depth)
{
    vec2 nearPlaneCoordinate = textureCoordinate * 2.0f - 1.0f;
    vec3 fragmentScreenSpacePosition = vec3(nearPlaneCoordinate, depth);
    vec4 viewSpacePosition = inverseProjectionMatrix * vec4(fragmentScreenSpacePosition, 1.0f);
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseCameraMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

/*
*   Scales a value from one range to another.
*/
float Scale(float value, float originalMinimum, float originalMaximum, float newMinimum, float newMaximum)
{
    return (((value - originalMinimum) * (newMaximum - newMinimum)) / (originalMaximum - originalMinimum)) + newMinimum;
}

void main()
{
    //Calculate the scene world position.
    vec3 sceneWorldPosition = CalculateSceneWorldPosition();

    //Based on whether or not the scene world position is above or below the ocean, render accoordingly.
    if (sceneWorldPosition.y > 0.0f)
    {
        fragment = vec4(CalculateAboveOceanFragment(), 1.0f);
    }

    else
    {
        fragment = vec4(CalculateBelowOceanFragment(sceneWorldPosition), 1.0f);
    }
}