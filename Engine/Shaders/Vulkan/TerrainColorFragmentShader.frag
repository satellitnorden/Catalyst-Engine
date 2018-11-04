//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define BLEND_SMOOTHING (0.1f)
#define PARALLAX_OCCLUSION_MAPPING_HEIGHT_SCALE (0.2f)
#define PARALLAX_OCCLUSION_MAPPING_SAMPLES (32)
#define PARALLAX_OCCLUSION_MAPPING_SAMPLE_STEP (1.0f / PARALLAX_OCCLUSION_MAPPING_SAMPLES)

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) bool parallaxOcclusionMappingEnabled;
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

vec3 fragmentWorldPosition;
vec3 fragmentWorldNormal;
vec4 fragmentLayerWeights;

vec2 textureCoordinates[5];

//Forward declarations.
vec2 CalculateParallaxOcclusionMapping(vec2 originalTextureCoordinate, sampler2D heightTexture);
void CalculateTextureCoordinates(mat3 tangentSpaceMatrix);

/*
*   Calculates the parallax occlusion mapping at the given texture coordinate.
*/
vec2 CalculateParallaxOcclusionMapping(vec3 tangentSpaceViewDirection, vec2 originalTextureCoordinate, sampler2D heightTexture)
{
    vec2 offset = tangentSpaceViewDirection.xy * PARALLAX_OCCLUSION_MAPPING_HEIGHT_SCALE;
    vec2 offsetStep = offset / PARALLAX_OCCLUSION_MAPPING_SAMPLES;

    vec2 currentTextureCoordinate = originalTextureCoordinate;
    float currentDepth = texture(heightTexture, currentTextureCoordinate).w;
    float accumulatedDepth = 0.0f;

    while(accumulatedDepth < currentDepth)
    {
        currentTextureCoordinate -= offsetStep;

        currentDepth = texture(heightTexture, currentTextureCoordinate).w;  

        accumulatedDepth += PARALLAX_OCCLUSION_MAPPING_SAMPLE_STEP;  
    }

    vec2 previousTextureCoordinate = currentTextureCoordinate + offsetStep;

    float afterDepth  = currentDepth - accumulatedDepth;
    float beforeDepth = texture(heightTexture, previousTextureCoordinate).w - accumulatedDepth + PARALLAX_OCCLUSION_MAPPING_SAMPLE_STEP;
     
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTextureCoordinate = previousTextureCoordinate * weight + currentTextureCoordinate * (1.0 - weight);

    return finalTextureCoordinate;  
}

/*
*   Calculates the texture coordinates for all layers.
*/
void CalculateTextureCoordinates(mat3 tangentSpaceMatrix)
{
    mat3 transposedTangentSpaceMatrix = inverse(tangentSpaceMatrix);
    vec3 tangentSpaceCameraWorldPosition = transposedTangentSpaceMatrix * cameraWorldPosition;
    vec3 tangentSpaceFragmentWorldPosition = transposedTangentSpaceMatrix * fragmentWorldPosition;
    vec3 tangentSpaceViewDirection = normalize(tangentSpaceCameraWorldPosition - tangentSpaceFragmentWorldPosition);

    vec2 originalTextureCoordinate = fragmentWorldPosition.xz * 0.25f;

    for (int i = 0; i < 5; ++i)
    {
        if (parallaxOcclusionMappingEnabled)
        {
            if (i == 0)
            {
                textureCoordinates[i] = CalculateParallaxOcclusionMapping(tangentSpaceViewDirection, originalTextureCoordinate, layer1MaterialPropertiesTexture);
            }

            else if (i == 1)
            {
                textureCoordinates[i] = CalculateParallaxOcclusionMapping(tangentSpaceViewDirection, originalTextureCoordinate, layer2MaterialPropertiesTexture);
            }

            else if (i == 2)
            {
                textureCoordinates[i] = CalculateParallaxOcclusionMapping(tangentSpaceViewDirection, originalTextureCoordinate, layer3MaterialPropertiesTexture);
            }

            else if (i == 3)
            {
                textureCoordinates[i] = CalculateParallaxOcclusionMapping(tangentSpaceViewDirection, originalTextureCoordinate, layer4MaterialPropertiesTexture);
            }

            else
            {
                textureCoordinates[i] = CalculateParallaxOcclusionMapping(tangentSpaceViewDirection, originalTextureCoordinate, layer5MaterialPropertiesTexture);
            }
        }

        else
        {
            textureCoordinates[i] = originalTextureCoordinate;
        }
    }
}

/*
*	Blends two terrain values.
*/
vec3 Blend(vec3 first, float firstHeight, vec3 second, float secondHeight, float alpha)
{
	float firstWeight = (1.0f - alpha) * firstHeight;
	float secondWeight = alpha * secondHeight;

    float total = 1.0f / (firstWeight + secondWeight);

    firstWeight *= total;
    secondWeight *= total;

    float difference = abs(firstWeight - secondWeight);
    float newAlpha = mix(0.5f, 1.0f, min(difference / BLEND_SMOOTHING, 1.0f));

    return firstWeight > secondWeight ? mix(second, first, newAlpha) : mix(first, second, newAlpha);
}

/*
*	Blends two terrain values.
*/
float Blend(float first, float firstHeight, float second, float secondHeight, float alpha)
{
	float firstWeight = (1.0f - alpha) * firstHeight;
	float secondWeight = alpha * secondHeight;

    float total = 1.0f / (firstWeight + secondWeight);

    firstWeight *= total;
    secondWeight *= total;

    float difference = abs(firstWeight - secondWeight);
    float newAlpha = mix(0.5f, 1.0f, min(difference / BLEND_SMOOTHING, 1.0f));

    return firstWeight > secondWeight ? mix(second, first, newAlpha) : mix(first, second, newAlpha);
}

/*
*	Returns the albedo.
*/
vec3 GetAlbedo()
{
    vec3 layer1Albedo = texture(layer1AlbedoTexture, textureCoordinates[0]).rgb;
    vec3 layer2Albedo = texture(layer2AlbedoTexture, textureCoordinates[1]).rgb;
    vec3 layer3Albedo = texture(layer3AlbedoTexture, textureCoordinates[2]).rgb;
    vec3 layer4Albedo = texture(layer4AlbedoTexture, textureCoordinates[3]).rgb;
    vec3 layer5Albedo = texture(layer5AlbedoTexture, textureCoordinates[4]).rgb;

    vec3 blend1 = Blend(layer1Albedo, layer1MaterialPropertiesSampler.w, layer2Albedo, layer2MaterialPropertiesSampler.w, fragmentLayerWeights.x);
    vec3 blend2 = Blend(blend1, layer2MaterialPropertiesSampler.w, layer3Albedo, layer3MaterialPropertiesSampler.w, fragmentLayerWeights.y);
    vec3 blend3 = Blend(blend2, layer3MaterialPropertiesSampler.w, layer4Albedo, layer4MaterialPropertiesSampler.w, fragmentLayerWeights.z);

    return Blend(blend3, layer4MaterialPropertiesSampler.w, layer5Albedo, layer5MaterialPropertiesSampler.w, fragmentLayerWeights.w);
}

/*
*	Returns the normal direction.
*/
vec3 GetNormalDirection()
{
	vec3 layer1NormalDirection = texture(layer1NormalMapTexture, textureCoordinates[0]).rgb;
    vec3 layer2NormalDirection = texture(layer2NormalMapTexture, textureCoordinates[1]).rgb;
    vec3 layer3NormalDirection = texture(layer3NormalMapTexture, textureCoordinates[2]).rgb;
    vec3 layer4NormalDirection = texture(layer4NormalMapTexture, textureCoordinates[3]).rgb;
    vec3 layer5NormalDirection = texture(layer5NormalMapTexture, textureCoordinates[4]).rgb;

    vec3 blend1 = Blend(layer1NormalDirection, layer1MaterialPropertiesSampler.w, layer2NormalDirection, layer2MaterialPropertiesSampler.w, fragmentLayerWeights.x);
    vec3 blend2 = Blend(blend1, layer2MaterialPropertiesSampler.w, layer3NormalDirection, layer3MaterialPropertiesSampler.w, fragmentLayerWeights.y);
    vec3 blend3 = Blend(blend2, layer3MaterialPropertiesSampler.w, layer4NormalDirection, layer4MaterialPropertiesSampler.w, fragmentLayerWeights.z);

    return Blend(blend3, layer4MaterialPropertiesSampler.w, layer5NormalDirection, layer5MaterialPropertiesSampler.w, fragmentLayerWeights.w) * 2.0f - 1.0f;
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

    float blend1 = Blend(layer1Roughness, layer1MaterialPropertiesSampler.w, layer2Roughness, layer2MaterialPropertiesSampler.w, fragmentLayerWeights.x);
    float blend2 = Blend(blend1, layer2MaterialPropertiesSampler.w, layer3Roughness, layer3MaterialPropertiesSampler.w, fragmentLayerWeights.y);
    float blend3 = Blend(blend2, layer3MaterialPropertiesSampler.w, layer4Roughness, layer4MaterialPropertiesSampler.w, fragmentLayerWeights.z);

    return Blend(blend3, layer4MaterialPropertiesSampler.w, layer5Roughness, layer5MaterialPropertiesSampler.w, fragmentLayerWeights.w);
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

    float blend1 = Blend(layer1Metallic, layer1MaterialPropertiesSampler.w, layer2Metallic, layer2MaterialPropertiesSampler.w, fragmentLayerWeights.x);
    float blend2 = Blend(blend1, layer2MaterialPropertiesSampler.w, layer3Metallic, layer3MaterialPropertiesSampler.w, fragmentLayerWeights.y);
    float blend3 = Blend(blend2, layer3MaterialPropertiesSampler.w, layer4Metallic, layer4MaterialPropertiesSampler.w, fragmentLayerWeights.z);

    return Blend(blend3, layer4MaterialPropertiesSampler.w, layer5Metallic, layer5MaterialPropertiesSampler.w, fragmentLayerWeights.w);
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

    float blend1 = Blend(layer1AmbientOcclusion, layer1MaterialPropertiesSampler.w, layer2AmbientOcclusion, layer2MaterialPropertiesSampler.w, fragmentLayerWeights.x);
    float blend2 = Blend(blend1, layer2MaterialPropertiesSampler.w, layer3AmbientOcclusion, layer3MaterialPropertiesSampler.w, fragmentLayerWeights.y);
    float blend3 = Blend(blend2, layer3MaterialPropertiesSampler.w, layer4AmbientOcclusion, layer4MaterialPropertiesSampler.w, fragmentLayerWeights.z);

    return Blend(blend3, layer4MaterialPropertiesSampler.w, layer5AmbientOcclusion, layer5MaterialPropertiesSampler.w, fragmentLayerWeights.w);
}

void main()
{
    //Sample the normal depth texture.
    vec4 normalDepthTextureSampler = texture(normalDepthTexture, fragmentTextureCoordinate);

    //Calculate the fragment world position.
    fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, normalDepthTextureSampler.w);

    //Set the fragment world normal.
    fragmentWorldNormal = normalDepthTextureSampler.xyz;

    //Calculate the tangent space matrix.
    vec3 normal = fragmentWorldNormal;
    vec3 tangent = cross(vec3(0.0f, 0.0f, 1.0f), normal);
    vec3 bitangent = cross(tangent, normal);

    mat3 tangentSpaceMatrix = mat3(tangent, bitangent, normal);

    //Set the fragment layer weights.
    fragmentLayerWeights = texture(layerWeightsTexture, fragmentTextureCoordinate);

	//Calculate the texture coordinates.
	CalculateTextureCoordinates(tangentSpaceMatrix);

    //Sampler the material properties.
    layer1MaterialPropertiesSampler = texture(layer1MaterialPropertiesTexture, textureCoordinates[0]);
    layer2MaterialPropertiesSampler = texture(layer2MaterialPropertiesTexture, textureCoordinates[1]);
    layer3MaterialPropertiesSampler = texture(layer3MaterialPropertiesTexture, textureCoordinates[2]);
    layer4MaterialPropertiesSampler = texture(layer4MaterialPropertiesTexture, textureCoordinates[3]);
    layer5MaterialPropertiesSampler = texture(layer5MaterialPropertiesTexture, textureCoordinates[4]);

	//Set the albedo color.
	albedoColor = vec4(GetAlbedo(), 1.0f);

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