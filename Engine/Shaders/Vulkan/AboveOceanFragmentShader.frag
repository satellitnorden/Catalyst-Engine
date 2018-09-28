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
    layout (offset = 1872) float windStrength; //Offset; 1872 - Size; 16
    layout (offset = 1888) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1904
};

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    float oceanScaling;
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

//Globals.
vec3 normalDirection;
vec3 sceneWorldPosition;

//Forward declarations.
vec3 CalculateAboveOceanFragment();
float CalculateAverage(vec3 fragment);
vec3 CalculateBelowOceanFragment();
vec3 CalculateDirectionalLight(vec3 reflectionDirection);
vec3 CalculateIntersectionPoint(vec3 pointOnPlane, vec3 pointOnLine, vec3 normal, vec3 line);
void CalculateNormalDirection(vec3 intersectionPoint);
vec3 CalculateReflection(vec3 reflectionDirection);
void CalculateSceneWorldPosition();
vec3 CalculateWorldPosition(vec2 textureCoordinate, float depth);

/*
*   Calculates the fragment when the scene world position is above the ocean.
*/
vec3 CalculateAboveOceanFragment()
{
    //The fragment should simply be the scene texture.
    return texture(sceneTexture, fragmentTextureCoordinate).rgb;
}

/*
*   Calculates the average of a fragment.
*/
float CalculateAverage(vec3 fragment)
{
    return (fragment.r + fragment.b + fragment.b) * 0.33333333f;
}

/*
*   Calculates the fragment when the scene world position is below the ocean.
*/
vec3 CalculateBelowOceanFragment()
{
    //Calculate the intersection point.
    vec3 viewDirection = normalize(sceneWorldPosition - cameraWorldPosition);
    vec3 intersectionPoint = CalculateIntersectionPoint(vec3(0.0f, 0.0f, 0.0f), cameraWorldPosition, vec3(0.0f, 1.0f, 0.0f), viewDirection);

    //Calculate the normal direction.
    CalculateNormalDirection(intersectionPoint);

    //Calculate the reflection direction.
    vec3 reflectionDirection = reflect(viewDirection, normalDirection);

    //Calculate the reflection.
    vec3 reflection = CalculateReflection(reflectionDirection);

    //Sample the scene texture.
    float distanceToBottom = length(sceneWorldPosition - intersectionPoint);
    float deformationWeight = clamp(distanceToBottom / 25.0f, 0.0f, 1.0f);
    vec2 sceneTextureCoordinate = fragmentTextureCoordinate + (normalDirection.xz * deformationWeight);

    vec4 sceneTextureSampler = texture(sceneTexture, sceneTextureCoordinate);

    //Calculate the underwater weight.
    float underwaterWeight = clamp(length(sceneWorldPosition - intersectionPoint) / 250.0f, 0.0f, 1.0f);

    //Calculate the underwater color.
    vec3 underwaterColor = vec3(0.0f, 0.375f, 0.5f) * (CalculateAverage(reflection) + directionalLightIntensity * 0.5f);

    //Calculate the transparency.
    float transparency = 1.0f - clamp(dot(normalDirection, normalize(cameraWorldPosition - sceneWorldPosition)), 0.0f, 1.0f);

    //Calculate the final ocean color.
    vec3 finalOceanColor = mix(mix(sceneTextureSampler.rgb, underwaterColor, underwaterWeight), reflection, transparency);

    //Apply foam.
    finalOceanColor = mix(finalOceanColor, texture(oceanFoamTexture, intersectionPoint.xz + vec2(totalGameTime * windDirection.x, totalGameTime * windDirection.z) * windStrength * 0.1f).rgb, clamp(1.0f - distanceToBottom, 0.0f, 1.0f));

    //Apply the directional light.
    finalOceanColor += CalculateDirectionalLight(reflectionDirection);

    return finalOceanColor;
}

/*
*   Calculates the directional light.
*/
vec3 CalculateDirectionalLight(vec3 reflectionDirection)
{
    return directionalLightColor * directionalLightIntensity * pow(max(dot(-directionalLightDirection, reflectionDirection), 0.0f), 1024.0f);
}

/*
*   Calculates a line-plane intersection point.
*/
vec3 CalculateIntersectionPoint(vec3 pointOnPlane, vec3 pointOnLine, vec3 normal, vec3 line)
{
    return dot(pointOnPlane - pointOnLine, normal) / dot(line, normal) * line + pointOnLine;
}

/*
*   Calculates the normal direction.
*/
void CalculateNormalDirection(vec3 intersectionPoint)
{
    normalDirection = texture(oceanNormalTexture, (intersectionPoint.xz * oceanScaling) - (vec2(totalGameTime * windDirection.x, totalGameTime * windDirection.z) * windStrength * 0.1f)).xzy * 2.0f - 1.0f;
    normalDirection = mix(normalDirection, vec3(0.0f, 1.0f, 0.0f), 0.9f);
}

/*
*   Calculates the reflection.
*/
vec3 CalculateReflection(vec3 reflectionDirection)
{
    return mix(texture(nightTexture, reflectionDirection).rgb, texture(dayTexture, reflectionDirection).rgb, environmentBlend);
}

/*
*   Calculates the scene world position.
*/
void CalculateSceneWorldPosition()
{
    //Sample the depth of the scene at this point.
    float sceneDepth = texture(sceneNormalDepthTexture, fragmentTextureCoordinate).w;

    //Calculate the scene world position.
    sceneWorldPosition = CalculateWorldPosition(fragmentTextureCoordinate, sceneDepth);
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

void main()
{
    //Calculate the scene world position.
    CalculateSceneWorldPosition();

    //Based on whether or not the scene world position is above or below the ocean, render accoordingly.
    if (sceneWorldPosition.y > 0.0f)
    {
        fragment = vec4(CalculateAboveOceanFragment(), 1.0f);
    }

    else
    {
        fragment = vec4(CalculateBelowOceanFragment(), 1.0f);
    }
}