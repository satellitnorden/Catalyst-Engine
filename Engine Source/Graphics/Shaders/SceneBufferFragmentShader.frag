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

//In parameters.
layout (location = 0) in vec3 fragmentWorldPosition;
layout (location = 1) in mat3 fragmentTangentSpaceMatrix;
layout (location = 4) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (binding = 2) uniform sampler2D albedoTexture;
layout (binding = 3) uniform sampler2D normalMapTexture;
layout (binding = 4) uniform sampler2D roughnessTexture;
layout (binding = 5) uniform sampler2D metallicTexture;
layout (binding = 6) uniform sampler2D ambientOcclusionTexture;

//Out parameters.
layout (location = 0) out vec4 albedoColor;

void main()
{
    //Set the albedo color.
    albedoColor = texture(albedoTexture, fragmentTextureCoordinate);
}