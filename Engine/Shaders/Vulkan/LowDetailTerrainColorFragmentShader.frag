//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

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
float RandomFloat(vec3 seed);

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