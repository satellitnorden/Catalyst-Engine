//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, binding = 0) uniform DynamicUniformData
{
  //Camera data.
  mat4 originViewMatrix;
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

//The cube map vertices.
vec3 cubeMapVertices[36] = vec3[]
(
  vec3(-1.0f, 1.0f, -1.0f),
  vec3(-1.0f, -1.0f, -1.0f),
  vec3(1.0f, -1.0f, -1.0f),
  vec3(1.0f, -1.0f, -1.0f),
  vec3(1.0f, 1.0f, -1.0f),
  vec3(-1.0f, 1.0f, -1.0f),

  vec3(-1.0f, -1.0f, 1.0f),
  vec3(-1.0f, -1.0f, -1.0f),
  vec3(-1.0f, 1.0f, -1.0f),
  vec3(-1.0f, 1.0f, -1.0f),
  vec3(-1.0f, 1.0f, 1.0f),
  vec3(-1.0f, -1.0f, 1.0f),

  vec3(1.0f, -1.0f, -1.0f),
  vec3(1.0f, -1.0f, 1.0f),
  vec3(1.0f, 1.0f, 1.0f),
  vec3(1.0f, 1.0f, 1.0f),
  vec3(1.0f, 1.0f, -1.0f),
  vec3(1.0f, -1.0f, -1.0f),

  vec3(-1.0f, -1.0f, 1.0f),
  vec3(-1.0f, 1.0f, 1.0f),
  vec3(1.0f, 1.0f, 1.0f),
  vec3(1.0f, 1.0f, 1.0f),
  vec3(1.0f, -1.0f, 1.0f),
  vec3(-1.0f, -1.0f, 1.0f),

  vec3(-1.0f, 1.0f, -1.0f),
  vec3(1.0f, 1.0f, -1.0f),
  vec3(1.0f, 1.0f, 1.0f),
  vec3(1.0f, 1.0f, 1.0f),
  vec3(-1.0f, 1.0f, 1.0f),
  vec3(-1.0f, 1.0f, -1.0f),

  vec3(-1.0f, -1.0f, -1.0f),
  vec3(-1.0f, -1.0f, 1.0f),
  vec3(1.0f, -1.0f, -1.0f),
  vec3(1.0f, -1.0f, -1.0f),
  vec3(-1.0f, -1.0f, 1.0f),
  vec3(1.0f, -1.0f, 1.0f)
);

//Model uniform buffer.
layout (binding = 1) uniform ModelData
{
    mat4 modelMatrix;
};

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexTextureCoordinate;

//Out parameters.
layout (location = 0) out vec3 fragmentWorldPosition;
layout (location = 1) out mat3 fragmentTangentSpaceMatrix;
layout (location = 4) out vec2 fragmentTextureCoordinate;

void main()
{
  fragmentWorldPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
    
  vec3 tangent = normalize(vec3(modelMatrix * vec4(vertexTangent, 0.0f)));
  vec3 bitangent = normalize(vec3(modelMatrix * vec4(cross(vertexNormal, vertexTangent), 0.0f)));
  vec3 normal = normalize(vec3(modelMatrix * vec4(vertexNormal, 0.0f)));

  fragmentTangentSpaceMatrix = mat3(tangent, bitangent, normal);

  fragmentTextureCoordinate = vertexTextureCoordinate;

  gl_Position = viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}