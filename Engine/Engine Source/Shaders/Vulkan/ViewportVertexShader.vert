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