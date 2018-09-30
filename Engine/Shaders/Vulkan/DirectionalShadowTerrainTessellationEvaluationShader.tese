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

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    float firstLayerDisplacementHeight;
    float secondLayerDisplacementHeight;
    float thirdLayerDisplacementHeight;
    float fourthLayerDisplacementHeight;
    float fifthLayerDisplacementHeight;
};

//Layout specification.
layout (triangles, equal_spacing, ccw) in;

//In parameters.
layout (location = 0) in vec3 tessellationEvaluationPosition[];
layout (location = 1) in vec3 tessellationEvaluationNormal[];
layout (location = 2) in vec4 tessellationEvaluationLayerWeights[];
layout (location = 3) in vec2 tessellationEvaluationTextureCoordinate[];

//Texture samplers.
layout (set = 1, binding = 2) uniform sampler2D layer1MaterialPropertiesTexture;
layout (set = 1, binding = 5) uniform sampler2D layer2MaterialPropertiesTexture;
layout (set = 1, binding = 8) uniform sampler2D layer3MaterialPropertiesTexture;
layout (set = 1, binding = 11) uniform sampler2D layer4MaterialPropertiesTexture;
layout (set = 1, binding = 14) uniform sampler2D layer5MaterialPropertiesTexture;

//Out parameters.
layout (location = 0) out vec3 fragmentNormal;
layout (location = 1) out vec4 fragmentLayerWeights;
layout (location = 2) out vec2 fragmentTextureCoordinate;

/*
*   Returns the displacement value.
*/
float GetDisplacement()
{
    float blend1 = mix(firstLayerDisplacementHeight, secondLayerDisplacementHeight, fragmentLayerWeights.x);
    float blend2 = mix(blend1, thirdLayerDisplacementHeight, fragmentLayerWeights.y);
    float blend3 = mix(blend2, fourthLayerDisplacementHeight, fragmentLayerWeights.z);

    float terrainDisplacementHeight = mix(blend3, fifthLayerDisplacementHeight, fragmentLayerWeights.w);

    float layer1Displacement = texture(layer1MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer2Displacement = texture(layer2MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer3Displacement = texture(layer3MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer4Displacement = texture(layer4MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer5Displacement = texture(layer5MaterialPropertiesTexture, fragmentTextureCoordinate).w;

    blend1 = mix(layer1Displacement, layer2Displacement, fragmentLayerWeights.x);
    blend2 = mix(blend1, layer3Displacement, fragmentLayerWeights.y);
    blend3 = mix(blend2, layer3Displacement, fragmentLayerWeights.z);

    return (mix(blend3, layer5Displacement, fragmentLayerWeights.w) * terrainDisplacementHeight) - (terrainDisplacementHeight * 0.5f);
}

void main()
{
    //Pass information to the tessellation evaluation shader.
    fragmentNormal = gl_TessCoord.x * tessellationEvaluationNormal[0] + gl_TessCoord.y * tessellationEvaluationNormal[1] + gl_TessCoord.z * tessellationEvaluationNormal[2];
    fragmentLayerWeights = gl_TessCoord.x * tessellationEvaluationLayerWeights[0] + gl_TessCoord.y * tessellationEvaluationLayerWeights[1] + gl_TessCoord.z * tessellationEvaluationLayerWeights[2];
    fragmentTextureCoordinate = gl_TessCoord.x * tessellationEvaluationTextureCoordinate[0] + gl_TessCoord.y * tessellationEvaluationTextureCoordinate[1] + gl_TessCoord.z * tessellationEvaluationTextureCoordinate[2];

    vec3 position = (gl_TessCoord.x * tessellationEvaluationPosition[0] + gl_TessCoord.y * tessellationEvaluationPosition[1] + gl_TessCoord.z * tessellationEvaluationPosition[2]);
    position.y += GetDisplacement();

    gl_Position = directionalLightViewMatrix * vec4(position, 1.0f);
}