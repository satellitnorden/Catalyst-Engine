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
	vec3 terrainPosition;
};

//Layout specification.
layout (triangles, equal_spacing, ccw) in;

//In parameters.
layout (location = 0) in vec2 tessellationEvaluationHeightMapTextureCoordinate[];
layout (location = 1) in vec2 tessellationEvaluationTextureCoordinate[];
layout (location = 2) in vec3 tessellationEvaluationPosition[];

//Texture samplers.
layout (binding = 2) uniform sampler2D displacementTexture;
layout (binding = 3) uniform sampler2D heightMapTexture;

//Out parameters.
layout (location = 0) out vec2 fragmentHeightMapTextureCoordinate;
layout (location = 1) out vec2 fragmentTextureCoordinate;

void main()
{
	//Pass the values along to the fragment shader.
	fragmentHeightMapTextureCoordinate = gl_TessCoord.x * tessellationEvaluationHeightMapTextureCoordinate[0] + gl_TessCoord.y * tessellationEvaluationHeightMapTextureCoordinate[1] + gl_TessCoord.z * tessellationEvaluationHeightMapTextureCoordinate[2];
	fragmentTextureCoordinate = gl_TessCoord.x * tessellationEvaluationTextureCoordinate[0] + gl_TessCoord.y * tessellationEvaluationTextureCoordinate[1] + gl_TessCoord.z * tessellationEvaluationTextureCoordinate[2];
	vec3 position = (gl_TessCoord.x * tessellationEvaluationPosition[0] + gl_TessCoord.y * tessellationEvaluationPosition[1] + gl_TessCoord.z * tessellationEvaluationPosition[2]);

	float height = texture(heightMapTexture, fragmentHeightMapTextureCoordinate).r * terrainHeight;
	float displacement = (texture(displacementTexture, fragmentTextureCoordinate).r * terrainDisplacementHeight) - (terrainDisplacementHeight * 0.5f);

	position.y += height + displacement;

	gl_Position = viewMatrix * vec4(position, 1.0f);
}