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
    float waterSize;
    float waterTextureScalingFactor;
    float waterPadding1;
    float waterPadding2;
    vec3 waterWorldPosition;
};

//The vertex positions.
vec3 vertexPositions[4] = vec3[]
(
  vec3(-0.5f, 0.0f, -0.5f),
  vec3(-0.5f, 0.0f, 0.5f),
  vec3(0.5f, 0.0f, 0.5f),
  vec3(0.5f, 0.0f, -0.5f)
);

//The texture coordinates.
vec2 vertexTextureCoordinates[4] = vec2[]
(
    vec2(0.0f, 1.0f),
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f)
);

//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;
layout (location = 1) out vec3 fragmentWorldPosition;

void main()
{	
	//Pass the texture coordinate to the fragment shader.
	fragmentTextureCoordinate = vertexTextureCoordinates[gl_VertexIndex] * waterTextureScalingFactor;

	//Pass the unmodified vertex position to the tesselation control shader.
    fragmentWorldPosition = vertexPositions[gl_VertexIndex] * waterSize + waterWorldPosition;
	gl_Position = viewMatrix * vec4(fragmentWorldPosition, 1.0f);
}