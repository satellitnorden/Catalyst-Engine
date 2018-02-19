//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, binding = 0) uniform DynamicUniformData
{
    //Camera data.
    float cameraFieldOfViewCosine;
    mat4 inverseCameraMatrix;
    mat4 inverseProjectionMatrix;
    mat4 originViewMatrix;
    mat4 viewMatrix;
    vec3 cameraForwardVector;
    vec3 cameraWorldPosition;
    float padding1;

    //Directional light data.
    float directionalLightIntensity;
    vec3 directionalLightDirection;
    vec3 directionalLightColor;
    vec3 directionalLightScreenSpacePosition;
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

//Terrain uniform buffer.
layout (std140, binding = 1) uniform TerrainUniformData
{
    float terrainDisplacementHeight;
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
layout (binding = 2) uniform sampler2D terrainPropertiesTexture;
layout (binding = 3) uniform sampler2D layerWeightsTexture;
layout (binding = 6) uniform sampler2D layer1MaterialPropertiesTexture;
layout (binding = 9) uniform sampler2D layer2MaterialPropertiesTexture;
layout (binding = 12) uniform sampler2D layer3MaterialPropertiesTexture;
layout (binding = 15) uniform sampler2D layer4MaterialPropertiesTexture;
layout (binding = 18) uniform sampler2D layer5MaterialPropertiesTexture;

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
    float layer1Displacement = texture(layer1MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer2Displacement = texture(layer2MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer3Displacement = texture(layer3MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer4Displacement = texture(layer4MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer5Displacement = texture(layer5MaterialPropertiesTexture, fragmentTextureCoordinate).w;

    float blend1 = mix(layer1Displacement, layer2Displacement, layerWeightsSampler.x);
    float blend2 = mix(blend1, layer3Displacement, layerWeightsSampler.y);
    float blend3 = mix(blend2, layer3Displacement, layerWeightsSampler.z);

    return (mix(blend3, layer5Displacement, layerWeightsSampler.w) * terrainDisplacementHeight) - (terrainDisplacementHeight * 0.5f);
}

void main()
{
	//Pass the values along to the fragment shader.
	fragmentHeightMapTextureCoordinate = gl_TessCoord.x * tessellationEvaluationHeightMapTextureCoordinate[0] + gl_TessCoord.y * tessellationEvaluationHeightMapTextureCoordinate[1] + gl_TessCoord.z * tessellationEvaluationHeightMapTextureCoordinate[2];
	fragmentTextureCoordinate = gl_TessCoord.x * tessellationEvaluationTextureCoordinate[0] + gl_TessCoord.y * tessellationEvaluationTextureCoordinate[1] + gl_TessCoord.z * tessellationEvaluationTextureCoordinate[2];
	vec3 position = (gl_TessCoord.x * tessellationEvaluationPosition[0] + gl_TessCoord.y * tessellationEvaluationPosition[1] + gl_TessCoord.z * tessellationEvaluationPosition[2]);

	float height = texture(terrainPropertiesTexture, fragmentHeightMapTextureCoordinate).w * terrainHeight;

	position.y = height + GetDisplacement();

	gl_Position = viewMatrix * vec4(position, 1.0f);
}