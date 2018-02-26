//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, set = 0, binding = 0) uniform DynamicUniformData
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

    //General data.
    float totalGameTime;

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

//Water uniform data.
layout (std140, set = 1, binding = 1) uniform WaterUniformData
{
    float waterHeight;
    float waterSize;
    float waterPadding1;
    float waterPadding2;
    vec3 waterWorldPosition;
};

//Layout specification.
layout (triangles, equal_spacing, ccw) in;

//In parameters.
layout (location = 0) in vec2 tessellationEvaluationTextureCoordinate[];
layout (location = 1) in vec3 tessellationEvaluationPosition[];

//Texture samplers.
layout (set = 1, binding = 2) uniform sampler2D normalMapTexture;

//Out parameters.
layout (location = 0) out vec2 fragmentTextureCoordinate;

void main()
{
	//Pass the values along to the fragment shader.
	fragmentTextureCoordinate = gl_TessCoord.x * tessellationEvaluationTextureCoordinate[0] + gl_TessCoord.y * tessellationEvaluationTextureCoordinate[1] + gl_TessCoord.z * tessellationEvaluationTextureCoordinate[2];
	vec3 position = (gl_TessCoord.x * tessellationEvaluationPosition[0] + gl_TessCoord.y * tessellationEvaluationPosition[1] + gl_TessCoord.z * tessellationEvaluationPosition[2]);

	//Construct the water normal.
    vec4 normalMapSampler = texture(normalMapTexture, fragmentTextureCoordinate);
    vec3 normal = normalMapSampler.xzy;

    //Modify the height based on the time and normal.
	position.y += dot(normal, vec3(0.0f, 1.0f, 0.0f)) * waterHeight * sin(position.x + position.z + totalGameTime);

    //Set the final position.
	gl_Position = viewMatrix * vec4(position, 1.0f);
}