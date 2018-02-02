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

//Out parameters.
layout (location = 0) out vec3 fragmentTextureCoordinate;

void main()
{
  //Just pass along the texture coordinate to the fragment shader.
  vec3 vertexPosition = cubeMapVertices[gl_VertexIndex];

  fragmentTextureCoordinate = vertexPosition;

  //Vertices should already arrive in normalized screen coordinates, so there's no need to do anything with them.
  vec4 skyBoxPosition = originViewMatrix * vec4(vertexPosition, 1.0f);
  gl_Position = skyBoxPosition.xyww;
}