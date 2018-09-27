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

//Terrain uniform buffer.
layout (std140, set = 1, binding = 1) uniform TerrainUniformData
{
    float terrainFirstLayerDisplacementHeight;
    float terrainSecondLayerDisplacementHeight;
    float terrainThirdLayerDisplacementHeight;
    float terrainFourthLayerDisplacementHeight;
    float terrainFifthLayerDisplacementHeight;
    float terrainHeight;
    float terrainSize;
    float terrainTextureTilingFactor;
    vec3 terrainPosition;
};

//Layout specification.
layout (triangles, equal_spacing, ccw) in;

//In parameters.
layout (location = 0) in vec2 tessellationEvaluationHeightMapTextureCoordinate[];
layout (location = 1) in vec2 tessellationEvaluationTextureCoordinate[];
layout (location = 2) in vec3 tessellationEvaluationPosition[];

//Texture samplers.
layout (set = 1, binding = 2) uniform sampler2D terrainPropertiesTexture;
layout (set = 1, binding = 3) uniform sampler2D layerWeightsTexture;
layout (set = 2, binding = 2) uniform sampler2D layer1MaterialPropertiesTexture;
layout (set = 2, binding = 5) uniform sampler2D layer2MaterialPropertiesTexture;
layout (set = 2, binding = 8) uniform sampler2D layer3MaterialPropertiesTexture;
layout (set = 2, binding = 11) uniform sampler2D layer4MaterialPropertiesTexture;
layout (set = 2, binding = 14) uniform sampler2D layer5MaterialPropertiesTexture;

//Out parameters.
layout (location = 0) out vec2 fragmentHeightMapTextureCoordinate;
layout (location = 1) out vec2 fragmentTextureCoordinate;
layout (location = 2) out vec4 layerWeightsSampler;

/*
*   Returns the displacement value.
*/
float GetDisplacement()
{
    layerWeightsSampler = texture(layerWeightsTexture, fragmentHeightMapTextureCoordinate);

    float blend1 = mix(terrainFirstLayerDisplacementHeight, terrainSecondLayerDisplacementHeight, layerWeightsSampler.x);
    float blend2 = mix(blend1, terrainThirdLayerDisplacementHeight, layerWeightsSampler.y);
    float blend3 = mix(blend2, terrainFourthLayerDisplacementHeight, layerWeightsSampler.z);

    float terrainDisplacementHeight = mix(blend3, terrainFifthLayerDisplacementHeight, layerWeightsSampler.w);

    float layer1Displacement = texture(layer1MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer2Displacement = texture(layer2MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer3Displacement = texture(layer3MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer4Displacement = texture(layer4MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer5Displacement = texture(layer5MaterialPropertiesTexture, fragmentTextureCoordinate).w;

    blend1 = mix(layer1Displacement, layer2Displacement, layerWeightsSampler.x);
    blend2 = mix(blend1, layer3Displacement, layerWeightsSampler.y);
    blend3 = mix(blend2, layer3Displacement, layerWeightsSampler.z);

    return (mix(blend3, layer5Displacement, layerWeightsSampler.w) * terrainDisplacementHeight) - (terrainDisplacementHeight * 0.5f);
}

void main()
{
	//Pass the values along to the fragment shader.
	fragmentHeightMapTextureCoordinate = gl_TessCoord.x * tessellationEvaluationHeightMapTextureCoordinate[0] + gl_TessCoord.y * tessellationEvaluationHeightMapTextureCoordinate[1] + gl_TessCoord.z * tessellationEvaluationHeightMapTextureCoordinate[2];
	fragmentTextureCoordinate = gl_TessCoord.x * tessellationEvaluationTextureCoordinate[0] + gl_TessCoord.y * tessellationEvaluationTextureCoordinate[1] + gl_TessCoord.z * tessellationEvaluationTextureCoordinate[2];
	vec3 position = (gl_TessCoord.x * tessellationEvaluationPosition[0] + gl_TessCoord.y * tessellationEvaluationPosition[1] + gl_TessCoord.z * tessellationEvaluationPosition[2]);

	float height = texture(terrainPropertiesTexture, fragmentHeightMapTextureCoordinate).w * terrainHeight;

	position.y = height + GetDisplacement() + terrainPosition.y;

	gl_Position = viewMatrix * vec4(position, 1.0f);
}