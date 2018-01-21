//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, binding = 0) uniform DynamicUniformData
{
	//Camera data.
  	mat4 viewMatrix;
  	vec3 cameraWorldPosition;
  	float padding1;

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

//Screen coordinates.
vec2 vertexScreenCoordinates[4] = vec2[]
(
  	vec2(-1.0f, -1.0f),
  	vec2(1.0, -1.0f),
  	vec2(1.0, 1.0f),
  	vec2(-1.0f, 1.0f)
);

//Texture coordinates.
vec2 vertexTextureCoordinates[4] = vec2[]
(
	vec2(0.0f, 0.0f),
  	vec2(1.0f, 0.0f),
  	vec2(1.0f, 1.0f),
  	vec2(0.0f, 1.0f)
);

//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;

void main()
{
  	//Set the texture coordinate.
  	fragmentTextureCoordinate = vertexTextureCoordinates[gl_VertexIndex];

  	//Set the position.
    gl_Position = vec4(vertexTextureCoordinates[gl_VertexIndex], 0.0f, 1.0f);
} 