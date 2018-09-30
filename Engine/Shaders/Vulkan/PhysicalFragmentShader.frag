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
    layout (offset = 1872) float windSpeed; //Offset; 1872 - Size; 16
    layout (offset = 1888) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1904
};

layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec3 fragmentWorldPosition;
layout (location = 1) in mat3 fragmentTangentSpaceMatrix;
layout (location = 4) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 1) uniform sampler2D albedoTexture;
layout (set = 1, binding = 2) uniform sampler2D normalMapTexture;
layout (set = 1, binding = 3) uniform sampler2D materialPropertiesTexture;

//Out parameters.
layout (location = 0) out vec4 albedoColor;
layout (location = 1) out vec4 normalDirectionDepth;
layout (location = 2) out vec4 materialProperties;

void main()
{
    //Set the albedo color.
    albedoColor = texture(albedoTexture, fragmentTextureCoordinate);

    //Set the normal.
    vec3 normalDirection = texture(normalMapTexture, fragmentTextureCoordinate).xyz * 2.0f - 1.0f;
    normalDirection = fragmentTangentSpaceMatrix * normalDirection;
    normalDirection = normalize(normalDirection);
    normalDirectionDepth = vec4(normalDirection, gl_FragCoord.z);

    //Sample the material properties.
    vec4 materialPropertiesSampler = texture(materialPropertiesTexture, fragmentTextureCoordinate);

    //Set the roughness.
    materialProperties.r = materialPropertiesSampler.r;

    //Set the metallic.
    materialProperties.g = materialPropertiesSampler.g;

    //Set the ambient occlusion.
    materialProperties.b = materialPropertiesSampler.b;

    //Write the thickness.
    materialProperties.a = 1.0f;
}