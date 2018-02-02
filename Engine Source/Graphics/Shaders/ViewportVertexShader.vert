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

//The viewport vertices.
vec3 viewportVertices[4] = vec3[]
(
	vec3(-1.0f, -1.0f, 0.0f),
	vec3(-1.0f, 1.0f, 0.0f),
	vec3(1.0f, 1.0f, 0.0f),
 	vec3(1.0f, -1.0f, 0.0f)
);

//The viewport texture coordinates.
vec2 viewportTextureCoordinates[4] = vec2[]
(
	vec2(0.0f, 0.0f),
	vec2(0.0f, 1.0f),
	vec2(1.0f, 1.0f),
	vec2(1.0f, 0.0f)
);

//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;

void main()
{
	//Set the fragment texture coordinate.
    fragmentTextureCoordinate = viewportTextureCoordinates[gl_VertexIndex];

    //Set the vertex position.
    gl_Position = vec4(viewportVertices[gl_VertexIndex], 1.0f);
} 