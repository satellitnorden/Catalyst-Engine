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

    //Physics data.
    layout (offset = 1808) float windStrength; //Offset; 1808 - Size; 16
    layout (offset = 1824) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1840
};

layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;
layout (location = 1) in vec3 fragmentWorldPosition;

//Texture samplers.
layout (set = 1, binding = 2) uniform sampler2D sceneTexture;
layout (set = 1, binding = 3) uniform sampler2D normalMapTexture;
layout (set = 1, binding = 4) uniform samplerCube skyTexture;

//Out parameters.
layout (location = 0) out vec4 albedoColor;

//Globals.
vec3 normalDirection;

/*
*   Calculates the reflection.
*/
vec3 CalculateReflection()
{
    vec3 viewDirection = fragmentWorldPosition - cameraWorldPosition;
    normalDirection = texture(normalMapTexture, fragmentTextureCoordinate + (vec2(totalGameTime, totalGameTime) * 0.025f)).xzy * 2.0f - 1.0f;
    vec3 compareVector = cameraWorldPosition.y >= 0.0f ? vec3(0.0f, 1.0f, 0.0f) : vec3(0.0f, -1.0f, 0.0f);
    normalDirection = mix(normalDirection, compareVector, 0.75f);

    vec3 reflectionDirection = reflect(viewDirection, normalDirection);

    return texture(skyTexture, reflectionDirection).rgb;
}

void main()
{
    //Calculate the reflection.
    vec3 reflection = CalculateReflection();

    //Calculate the transparency.
    float transparency = 1.0f - clamp(dot(normalDirection, normalize(cameraWorldPosition - fragmentWorldPosition)), 0.0f, 1.0f);

    //Calculate the screen coordinate.
    vec2 screenCoordinate = vec2(gl_FragCoord.x / 1920.0f, gl_FragCoord.y / 1061.0f);

    screenCoordinate += vec2(normalDirection.x, normalDirection.z);
    
    vec3 underwaterColor = texture(sceneTexture, screenCoordinate).rgb;

    //Calculate the final color.
    vec3 finalWaterColor = mix(underwaterColor, reflection, transparency);

    //Set the final color.
    albedoColor = vec4(finalWaterColor, 1.0f);
}