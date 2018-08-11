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

//Preprocessor defines.
#define NUMBER_OF_SCREEN_SPACE_REFLECTION_ITERATIONS 100

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    float oceanScaling;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform samplerCube nightTexture;
layout (set = 1, binding = 2) uniform samplerCube dayTexture;
layout (set = 2, binding = 0) uniform sampler2D sceneTexture;
layout (set = 2, binding = 1) uniform sampler2D sceneNormalDepthTexture;
layout (set = 2, binding = 2) uniform sampler2D oceanNormalTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

//Globals.
float depth;
float screenSpaceReflectionWeight;
vec3 intersectionPoint;
vec3 normalDirection;
vec3 reflectionDirection;
vec3 sceneWorldPosition;
vec3 screenSpaceReflection;
vec3 viewDirection;

/*
*   Calculates the normal direction.
*/
void CalculateNormalDirection()
{
    normalDirection = texture(oceanNormalTexture, (intersectionPoint.xz * oceanScaling) - (vec2(totalGameTime * windDirection.x, totalGameTime * windDirection.z) * windStrength * 0.025f)).xzy * 2.0f - 1.0f;
}

/*
*   Calculates the reflection direction.
*/
void CalculateReflectionDirection()
{
    vec3 inverseViewDirection = sceneWorldPosition - cameraWorldPosition;
    reflectionDirection = normalize(reflect(inverseViewDirection, normalDirection));
}

/*
*   Calculates the screen space reflection.
*/
void CalculateScreenSpaceReflection()
{
    //Disabled for now.
    screenSpaceReflectionWeight = 0.0f;

    /*
    //Set the preliminary screen space reflection weight.
    screenSpaceReflectionWeight = 0.0f;

    //Iterate N times and see if there's a hit.
    vec3 currentPosition = intersectionPoint + reflectionDirection;

    for (int i = 0; i < NUMBER_OF_SCREEN_SPACE_REFLECTION_ITERATIONS; ++i)
    {
        vec4 screenSpacePosition = viewMatrix * vec4(currentPosition, 1.0f);
        screenSpacePosition.xyz /= screenSpacePosition.w;
        screenSpacePosition.xy = screenSpacePosition.xy * 0.5f + 0.5f;

        float sceneDepth = texture(sceneNormalDepthTexture, screenSpacePosition.xy).a;

        if (sceneDepth < screenSpacePosition.z)
        {
            screenSpaceReflectionWeight = 1.0f;
            screenSpaceReflection = texture(sceneTexture, screenSpacePosition.xy).rgb;

            break;
        }

        currentPosition += reflectionDirection;
    }
    */
}

/*
*   Calculates the reflection.
*/
vec3 CalculateReflection()
{
    return mix(mix(texture(nightTexture, reflectionDirection).rgb, texture(dayTexture, reflectionDirection).rgb, environmentBlend), screenSpaceReflection, screenSpaceReflectionWeight);
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
*   Calculates the directional light.
*/
vec3 CalculateDirectionalLight()
{
    vec3 halfwayDirection = normalize(-directionalLightDirection + viewDirection);
    return directionalLightColor * directionalLightIntensity * pow(max(dot(normalDirection, halfwayDirection), 0.0f), 512) * 0.1f;
}

/*
*   Returns the length of a vector squared, ignoring the Y component.
*/
float LengthSquared(vec3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

/*
*	Calculates the average of a fragment.
*/
float CalculateAverage(vec3 fragment)
{
	return (fragment.r + fragment.b + fragment.b) * 0.33333333f;
}

void main()
{
	 //Sample the depth of the scene at this point.
    float sceneDepth = texture(sceneNormalDepthTexture, fragmentTextureCoordinate).w;

    //Calculate the scene world position.
    sceneWorldPosition = CalculateWorldPosition(fragmentTextureCoordinate, sceneDepth);

    //Calculate the intersection point.
    viewDirection = normalize(cameraWorldPosition - sceneWorldPosition);
    intersectionPoint = (dot(-sceneWorldPosition, vec3(0.0f, 1.0f, 0.0f)) / dot (viewDirection, vec3(0.0f, 1.0f, 0.0f))) * viewDirection + sceneWorldPosition;

    //Calculate the normal direction.
    CalculateNormalDirection();

    //Calculate the reflection direction.
    CalculateReflectionDirection();

    //Calculate the screen space reflection.
    CalculateScreenSpaceReflection();

	//Calculate the reflection.
    vec3 reflection = CalculateReflection();

    //Sample the scene texture.
    float deformationWeight = clamp(length(sceneWorldPosition - intersectionPoint) / 25.0f, 0.0f, 1.0f);
    vec2 sceneTextureCoordinate = sceneWorldPosition.y > 0.0f || cameraWorldPosition.y < 0.0f ? fragmentTextureCoordinate : fragmentTextureCoordinate + (normalDirection.xz * deformationWeight);
    float suggestedSceneDepth = texture(sceneNormalDepthTexture, sceneTextureCoordinate).w;
	vec3 suggestedSceneWorldPosition = CalculateWorldPosition(sceneTextureCoordinate, suggestedSceneDepth);

	//Calculate the underwater weight.
    float underwaterWeight = clamp(length(suggestedSceneWorldPosition - intersectionPoint) / 25.0f, 0.0f, 1.0f);

	sceneTextureCoordinate = suggestedSceneWorldPosition.y > 0.0f ? fragmentTextureCoordinate : sceneTextureCoordinate;
	sceneTextureCoordinate = mix(fragmentTextureCoordinate, sceneTextureCoordinate, underwaterWeight);
    vec4 sceneTextureSampler = texture(sceneTexture, sceneTextureCoordinate);

    

    //Calculate the underwater color.
    float underwaterColorWeight = suggestedSceneWorldPosition.y > 0.0f ? 0.0f : clamp(-suggestedSceneWorldPosition.y / 25.0f, 0.0f, 1.0f);
    vec3 underwaterColor = mix(vec3(0.0f, 0.75f, 1.0f), vec3(0.0f, 0.25f, 0.5f), underwaterColorWeight) * (CalculateAverage(reflection) + directionalLightIntensity * 0.1f);

    //Calculate the transparency.
    float transparency = 1.0f - clamp(dot(normalDirection, normalize(cameraWorldPosition - sceneWorldPosition)), 0.0f, 1.0f);

    //Calculate the final ocean color.
    vec3 finalOceanColor = sceneWorldPosition.y > 0.0f || cameraWorldPosition.y < 0.0f ? sceneTextureSampler.rgb : mix(mix(sceneTextureSampler.rgb, underwaterColor, underwaterWeight), reflection, transparency);

    //Apply the directional light.
    if (sceneWorldPosition.y < 0.0f && cameraWorldPosition.y > 0.0f)
    {
        finalOceanColor += CalculateDirectionalLight();
    }

    //Calculate the depth.
    vec4 projectedPosition = viewMatrix * vec4(intersectionPoint, 1.0f);
    projectedPosition.xyz /= projectedPosition.w;
    float depth = projectedPosition.z;

    //Write the fragment
    fragment = vec4(finalOceanColor, 1.0f);
}