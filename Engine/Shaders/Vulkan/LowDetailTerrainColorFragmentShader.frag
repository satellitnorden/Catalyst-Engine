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
layout (set = 2, binding = 3) uniform sampler2D layer2AlbedoTexture;
layout (set = 2, binding = 6) uniform sampler2D layer3AlbedoTexture;
layout (set = 2, binding = 9) uniform sampler2D layer4AlbedoTexture;
layout (set = 2, binding = 12) uniform sampler2D layer5AlbedoTexture;

//Out parameters.
layout (location = 0) out vec4 albedoColor;
layout (location = 1) out vec4 materialProperties;

//Globals.
vec2 finalTextureCoordinate;

vec3 fragmentWorldPosition;
vec3 fragmentWorldNormal;
vec4 fragmentLayerWeights;

//Forward declarations.
vec3 CalculateFragmentWorldPosition(vec2 textureCoordinate, float depth);
float RandomFloat(vec3 seed);

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
*   Given a coordinate and a seed, returns a random number.
*/
float RandomFloat(vec3 seed)
{
    return fract(sin(dot(seed.xy * seed.z, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

/*
*	Calculates the tri-planar data.
*/
void CalculateTriPlanarData(float depth)
{
	//Calculate the absolute normal.
	vec3 absoluteNormal = abs(fragmentWorldNormal);

	//Calculate the texture coordinates on the three planes.
	vec2 textureCoordinateYZ = fragmentWorldPosition.yz;
	vec2 textureCoordinateXZ = fragmentWorldPosition.xz;
	vec2 textureCoordinateXY = fragmentWorldPosition.xy;

    //Calculate the random float.
    float randomFloat = RandomFloat(vec3(gl_FragCoord.xy, depth));

    //Pick which plane to sample.
    if (absoluteNormal.x > absoluteNormal.y && absoluteNormal.x > absoluteNormal.z && absoluteNormal.x > randomFloat)
    {
        finalTextureCoordinate = textureCoordinateYZ;
    }

    else if (absoluteNormal.y > absoluteNormal.x && absoluteNormal.y > absoluteNormal.z && absoluteNormal.y > randomFloat)
    {
        finalTextureCoordinate = textureCoordinateXZ;
    }

    else
    {
        finalTextureCoordinate = textureCoordinateXY;
    }

    finalTextureCoordinate *= 0.25f;
}

/*
*	Returns the albedo.
*/
vec4 GetAlbedo()
{
    vec4 layer1Albedo = texture(layer1AlbedoTexture, finalTextureCoordinate);
    vec4 layer2Albedo = texture(layer2AlbedoTexture, finalTextureCoordinate);
    vec4 layer3Albedo = texture(layer3AlbedoTexture, finalTextureCoordinate);
    vec4 layer4Albedo = texture(layer4AlbedoTexture, finalTextureCoordinate);
    vec4 layer5Albedo = texture(layer5AlbedoTexture, finalTextureCoordinate);

    vec4 blend1 = mix(layer1Albedo, layer2Albedo, fragmentLayerWeights.x);
    vec4 blend2 = mix(blend1, layer3Albedo, fragmentLayerWeights.y);
    vec4 blend3 = mix(blend2, layer4Albedo, fragmentLayerWeights.z);

    return mix(blend3, layer5Albedo, fragmentLayerWeights.w);
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
	CalculateTriPlanarData(normalDepthTextureSampler.w);

	//Set the albedo color.
	albedoColor = GetAlbedo();

	//Write the material properties.
    materialProperties = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}