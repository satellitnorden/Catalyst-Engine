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

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D normalDepthTexture;
layout (set = 1, binding = 1) uniform sampler2D layerWeightsTexture;
layout (set = 2, binding = 0) uniform sampler2D layer1AlbedoTexture;
layout (set = 2, binding = 1) uniform sampler2D layer1NormalMapTexture;
layout (set = 2, binding = 2) uniform sampler2D layer1MaterialPropertiesTexture;
layout (set = 2, binding = 3) uniform sampler2D layer2AlbedoTexture;
layout (set = 2, binding = 4) uniform sampler2D layer2NormalMapTexture;
layout (set = 2, binding = 5) uniform sampler2D layer2MaterialPropertiesTexture;
layout (set = 2, binding = 6) uniform sampler2D layer3AlbedoTexture;
layout (set = 2, binding = 7) uniform sampler2D layer3NormalMapTexture;
layout (set = 2, binding = 8) uniform sampler2D layer3MaterialPropertiesTexture;
layout (set = 2, binding = 9) uniform sampler2D layer4AlbedoTexture;
layout (set = 2, binding = 10) uniform sampler2D layer4NormalMapTexture;
layout (set = 2, binding = 11) uniform sampler2D layer4MaterialPropertiesTexture;
layout (set = 2, binding = 12) uniform sampler2D layer5AlbedoTexture;
layout (set = 2, binding = 13) uniform sampler2D layer5NormalMapTexture;
layout (set = 2, binding = 14) uniform sampler2D layer5MaterialPropertiesTexture;

//Out parameters.
layout (location = 0) out vec4 albedoColor;
layout (location = 1) out vec4 normalDirectionDepth;
layout (location = 2) out vec4 materialProperties;

//Globals.
vec4 layer1MaterialPropertiesSampler;
vec4 layer2MaterialPropertiesSampler;
vec4 layer3MaterialPropertiesSampler;
vec4 layer4MaterialPropertiesSampler;
vec4 layer5MaterialPropertiesSampler;
vec3 absoluteNormal;
vec2 textureCoordinateYZ;
vec2 textureCoordinateXZ;
vec2 textureCoordinateXY;

vec3 fragmentWorldPosition;
vec3 fragmentWorldNormal;
vec4 fragmentLayerWeights;

//Forward declarations.
vec3 CalculateFragmentWorldPosition(vec2 textureCoordinate, float depth);

/*
*   Calculates the fragment world position.
*/
vec3 CalculateFragmentWorldPosition(vec2 textureCoordinate, float depth)
{
    vec2 nearPlaneCoordinate = textureCoordinate * 2.0f - 1.0f;
    vec3 fragmentScreenSpacePosition = vec3(nearPlaneCoordinate, depth);
    vec4 viewSpacePosition = inverseProjectionMatrix * vec4(fragmentScreenSpacePosition, 1.0f);
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseCameraMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

/*
*	Calculates the tri-planar data.
*/
void CalculateTriPlanarData()
{
	//Calculate the absolute normal.
	absoluteNormal = abs(fragmentWorldNormal);
	absoluteNormal /= absoluteNormal.x + absoluteNormal.y + absoluteNormal.z;

	//Calculate the texture coordinates on the three planes.
	textureCoordinateYZ = fragmentWorldPosition.yz * 0.25f;
	textureCoordinateXZ = fragmentWorldPosition.xz * 0.25f;
	textureCoordinateXY = fragmentWorldPosition.xy * 0.25f;
}

/*
*	Samples a texture using tri-planar mapping.
*/
vec4 SampleTriPlanar(sampler2D textureSampler)
{
	vec4 xSample = texture(textureSampler, textureCoordinateYZ) * absoluteNormal.x;
	vec4 ySample = texture(textureSampler, textureCoordinateXZ) * absoluteNormal.y;
	vec4 zSample = texture(textureSampler, textureCoordinateXY) * absoluteNormal.z;

	return xSample + ySample + zSample;
}

/*
*	Returns the albedo.
*/
vec4 GetAlbedo()
{
    vec4 layer1Albedo = SampleTriPlanar(layer1AlbedoTexture);
    vec4 layer2Albedo = SampleTriPlanar(layer2AlbedoTexture);
    vec4 layer3Albedo = SampleTriPlanar(layer3AlbedoTexture);
    vec4 layer4Albedo = SampleTriPlanar(layer4AlbedoTexture);
    vec4 layer5Albedo = SampleTriPlanar(layer5AlbedoTexture);

    vec4 blend1 = mix(layer1Albedo, layer2Albedo, fragmentLayerWeights.x);
    vec4 blend2 = mix(blend1, layer3Albedo, fragmentLayerWeights.y);
    vec4 blend3 = mix(blend2, layer4Albedo, fragmentLayerWeights.z);

    return mix(blend3, layer5Albedo, fragmentLayerWeights.w);
}

/*
*	Returns the normal direction.
*/
vec3 GetNormalDirection()
{
	vec3 layer1NormalDirection = SampleTriPlanar(layer1NormalMapTexture).xyz;
    vec3 layer2NormalDirection = SampleTriPlanar(layer2NormalMapTexture).xyz;
    vec3 layer3NormalDirection = SampleTriPlanar(layer3NormalMapTexture).xyz;
    vec3 layer4NormalDirection = SampleTriPlanar(layer4NormalMapTexture).xyz;
    vec3 layer5NormalDirection = SampleTriPlanar(layer5NormalMapTexture).xyz;

    vec3 blend1 = mix(layer1NormalDirection, layer2NormalDirection, fragmentLayerWeights.x);
    vec3 blend2 = mix(blend1, layer3NormalDirection, fragmentLayerWeights.y);
    vec3 blend3 = mix(blend2, layer4NormalDirection, fragmentLayerWeights.z);

    return mix(blend3, layer5NormalDirection, fragmentLayerWeights.w) * 2.0f - 1.0f;
}

/*
*	Returns the roughness.
*/
float GetRoughness()
{
	float layer1Roughness = layer1MaterialPropertiesSampler.r;
    float layer2Roughness = layer2MaterialPropertiesSampler.r;
    float layer3Roughness = layer3MaterialPropertiesSampler.r;
    float layer4Roughness = layer4MaterialPropertiesSampler.r;
    float layer5Roughness = layer5MaterialPropertiesSampler.r;

    float blend1 = mix(layer1Roughness, layer2Roughness, fragmentLayerWeights.x);
    float blend2 = mix(blend1, layer3Roughness, fragmentLayerWeights.y);
    float blend3 = mix(blend2, layer4Roughness, fragmentLayerWeights.z);

    return mix(blend3, layer5Roughness, fragmentLayerWeights.w);
}

/*
*	Returns the metallic.
*/
float GetMetallic()
{
	float layer1Metallic = layer1MaterialPropertiesSampler.y;
    float layer2Metallic = layer2MaterialPropertiesSampler.y;
    float layer3Metallic = layer3MaterialPropertiesSampler.y;
    float layer4Metallic = layer4MaterialPropertiesSampler.y;
    float layer5Metallic = layer5MaterialPropertiesSampler.y;

    float blend1 = mix(layer1Metallic, layer2Metallic, fragmentLayerWeights.x);
    float blend2 = mix(blend1, layer3Metallic, fragmentLayerWeights.y);
    float blend3 = mix(blend2, layer4Metallic, fragmentLayerWeights.z);

    return mix(blend3, layer5Metallic, fragmentLayerWeights.w);
}

/*
*	Returns the ambient occlusion.
*/
float GetAmbientOcclusion()
{
	float layer1AmbientOcclusion = layer1MaterialPropertiesSampler.z;
    float layer2AmbientOcclusion = layer2MaterialPropertiesSampler.z;
    float layer3AmbientOcclusion = layer3MaterialPropertiesSampler.z;
    float layer4AmbientOcclusion = layer4MaterialPropertiesSampler.z;
    float layer5AmbientOcclusion = layer5MaterialPropertiesSampler.z;

    float blend1 = mix(layer1AmbientOcclusion, layer2AmbientOcclusion, fragmentLayerWeights.x);
    float blend2 = mix(blend1, layer3AmbientOcclusion, fragmentLayerWeights.y);
    float blend3 = mix(blend2, layer4AmbientOcclusion, fragmentLayerWeights.z);

    return mix(blend3, layer5AmbientOcclusion, fragmentLayerWeights.w);
}

void main()
{
    //Sample the normal depth texture.
    vec4 normalDepthTextureSampler = texture(normalDepthTexture, fragmentTextureCoordinate);

    //Calculate the fragment world position.
    fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, normalDepthTextureSampler.w);

    //Set the fragment world normal.
    fragmentWorldNormal = normalDepthTextureSampler.xyz;

    //Set the fragment layer weights.
    fragmentLayerWeights = texture(layerWeightsTexture, fragmentTextureCoordinate);

	//Calculate the tri-planar data.
	CalculateTriPlanarData();

    //Sampler the material properties.
    layer1MaterialPropertiesSampler = SampleTriPlanar(layer1MaterialPropertiesTexture);
    layer2MaterialPropertiesSampler = SampleTriPlanar(layer2MaterialPropertiesTexture);
    layer3MaterialPropertiesSampler = SampleTriPlanar(layer3MaterialPropertiesTexture);
    layer4MaterialPropertiesSampler = SampleTriPlanar(layer4MaterialPropertiesTexture);
    layer5MaterialPropertiesSampler = SampleTriPlanar(layer5MaterialPropertiesTexture);

	//Set the albedo color.
	albedoColor = GetAlbedo();

	//Calculate the tangent space matrix.
	vec3 normal = fragmentWorldNormal;
	vec3 tangent = cross(vec3(0.0f, 0.0f, 1.0f), normal);
	vec3 bitangent = cross(tangent, normal);

	mat3 tangentSpaceMatrix = mat3(tangent, bitangent, normal);

	//Set the normal.
    vec3 normalDirection = GetNormalDirection();
    normalDirection = tangentSpaceMatrix * normalDirection;
    normalDirection = normalize(normalDirection);
    normalDirectionDepth = vec4(normalDirection, normalDepthTextureSampler.w);

	//Set the roughness.
    materialProperties.r = GetRoughness();

    //Set the metallic.
    materialProperties.g = GetMetallic();

    //Set the ambient occlusion.
    materialProperties.b = GetAmbientOcclusion();

    //Write the thickness.
    materialProperties.a = 1.0f;
}