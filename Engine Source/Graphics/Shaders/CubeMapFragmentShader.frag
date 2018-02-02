//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, binding = 0) uniform DynamicUniformData
{
    //Camera data.
    mat4 inverseCameraMatrix;
    mat4 inverseProjectionMatrix;
    mat4 originViewMatrix;
    mat4 viewMatrix;
    vec3 cameraWorldPosition;
    float padding1;

    //Directional light data.
    float directionalLightIntensity;
    vec3 directionalLightDirection;
    vec3 directionalLightColor;
    float padding2;

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

//In parameters.
layout (location = 0) in vec3 fragmentTextureCoordinate;

//Texture samplers.
layout (binding = 1) uniform samplerCube cubeMapTexture;

//Out parameters.
layout (location = 0) out vec4 fragmentColor;

void main()
{
    //Sample the cube map texture.
    vec3 cubeMapTextureSampler = texture(cubeMapTexture, fragmentTextureCoordinate).rgb;

    //Apply gamma correction.
    cubeMapTextureSampler = pow(cubeMapTextureSampler, vec3(2.2f));

    //Set the fragment color.
    fragmentColor = vec4(cubeMapTextureSampler, 1.0f);
}