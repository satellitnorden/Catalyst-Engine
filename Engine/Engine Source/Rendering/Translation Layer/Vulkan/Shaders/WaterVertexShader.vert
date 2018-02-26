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

//Water uniform data.
layout (std140, set = 1, binding = 1) uniform WaterUniformData
{
    float waterHeight;
    float waterSize;
    float waterPadding1;
    float waterPadding2;
    vec3 waterWorldPosition;
};

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTextureCoordinate;

//Out parameters.
layout (location = 0) out vec2 tessellationControlTextureCoordinate;
layout (location = 1) out vec3 tessellationControlPosition;

void main()
{	
	//Pass the texture coordinate to the tesselation control shader.
	tessellationControlTextureCoordinate = vertexTextureCoordinate * (waterSize * 0.25f);

	//Pass the unmodified vertex position to the tesselation control shader.
	tessellationControlPosition = vertexPosition;
}