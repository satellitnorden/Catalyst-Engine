//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, set = 0, binding = 0) uniform DynamicUniformData
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

    //General data.
    float totalGameTime;

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