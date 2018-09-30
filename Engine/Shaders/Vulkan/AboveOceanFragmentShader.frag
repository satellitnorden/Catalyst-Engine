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
float LengthSquared(vec3 vector);
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
    float distanceToBottomSquared = LengthSquared(sceneWorldPosition - intersectionPoint);

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
    return oceanColor * (reflection + directionalLightColor * (directionalLightIntensity * 0.5f));
}

/*
*   Calculates the ocean color weight.
*/
float CalculateOceanColorWeight(float distanceToBottomSquared)
{
    //Calculate the ocean color weight.
    return Scale(min(distanceToBottomSquared / oceanColorWeightDistanceSquared, 1.0f), 0.0f, 1.0f, 0.1f, 1.0f);
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
*   Returns the length of a vector squared.
*/
float LengthSquared(vec3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
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