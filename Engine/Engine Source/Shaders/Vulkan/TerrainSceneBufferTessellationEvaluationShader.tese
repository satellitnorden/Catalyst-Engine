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
    layout (offset = 320) vec3 directionalLightDirection; //Offset; 320 - Size; 16
    layout (offset = 336) vec3 directionalLightColor; //Offset; 336 - Size; 16
    layout (offset = 352) vec3 directionalLightScreenSpacePosition; //Offset; 352 - Size; 16

    //General data.
    layout (offset = 368) float totalGameTime; //Offset; 368 - Size; 4

    //Point light data.
    layout (offset = 372) int numberOfPointLights; //Offset; 372 - Size; 12
    layout (offset = 384) float pointLightAttenuationDistances[MaximumNumberOfPointLights]; //Offset; 384 - Size; 128
    layout (offset = 512) float pointLightIntensities[MaximumNumberOfPointLights]; //Offset; 512 - Size; 128
    layout (offset = 640) vec3 pointLightColors[MaximumNumberOfPointLights]; //Offset; 640 - Size; 128
    layout (offset = 768) vec3 pointLightWorldPositions[MaximumNumberOfPointLights]; //Offset; 768 - Size; 128

    //Spot light data.
    layout (offset = 896) int numberOfSpotLights; //Offset; 896 - Size; 16
    layout (offset = 912) float spotLightAttenuationDistances[MaximumNumberOfSpotLights]; //Offset; 912 - Size; 128
    layout (offset = 1040) float spotLightIntensities[MaximumNumberOfSpotLights]; //Offset; 1040 - Size; 128
    layout (offset = 1168) float spotLightInnerCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1168 - Size; 128
    layout (offset = 1296) float spotLightOuterCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1296 - Size; 128
    layout (offset = 1424) vec3 spotLightColors[MaximumNumberOfSpotLights]; //Offset; 1424 - Size; 128
    layout (offset = 1552) vec3 spotLightDirections[MaximumNumberOfSpotLights]; //Offset; 1552 - Size; 128
    layout (offset = 1680) vec3 spotLightWorldPositions[MaximumNumberOfSpotLights]; //Offset; 1680 - Size; 128

    //Physics data.
    layout (offset = 1808) float windStrength; //Offset; 1808 - Size; 16
    layout (offset = 1824) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1840
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
layout (set = 1, binding = 6) uniform sampler2D layer1MaterialPropertiesTexture;
layout (set = 1, binding = 9) uniform sampler2D layer2MaterialPropertiesTexture;
layout (set = 1, binding = 12) uniform sampler2D layer3MaterialPropertiesTexture;
layout (set = 1, binding = 15) uniform sampler2D layer4MaterialPropertiesTexture;
layout (set = 1, binding = 18) uniform sampler2D layer5MaterialPropertiesTexture;

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