//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//Preprocessor defines.
#define DIRECTIONAL_LIGHT_SCREEN_SHADE_SHADOWS_SAMPLES 250

//Layout specification.
layout (early_fragment_tests) in;

//Post processing data.
layout (push_constant) uniform LightingData
{
    bool screenSpaceAmbientOcclusionEnabled;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform samplerCube nightDiffuseTexture;
layout (set = 1, binding = 1) uniform samplerCube nightDiffuseIrradianceTexture;
layout (set = 2, binding = 0) uniform samplerCube dayDiffuseTexture;
layout (set = 2, binding = 1) uniform samplerCube dayDiffuseIrradianceTexture;
layout (set = 3, binding = 0) uniform sampler2D albedoTexture;
layout (set = 3, binding = 1) uniform sampler2D normalDirectionDepthTexture;
layout (set = 3, binding = 2) uniform sampler2D roughnessMetallicAmbientOcclusionTexture;
layout (set = 3, binding = 3) uniform sampler2D directionalShadow;

//Out parameters.
layout (location = 0) out vec4 fragmentColor;

//Globals.
float ambientOcclusion;
float fragmentDepth;
float metallic;
float roughness;
float screenSpaceAmbientOcclusion;
float thickness;
float viewAngle;
vec3 viewDirection;
vec3 surfaceColor;
vec3 fragmentWorldPosition;
vec3 albedoColor;
vec3 normalDirection;

/*
*   Linear step.
*/
float LinearStep(float low, float high, float value)
{
    return clamp((value - low) / (high - low), 0.0f, 1.0f);
}

/*
*   Calculates the directional light shadow multiplier.
*/
float CalculateDirectionalLightShadowMultiplier()
{
    return texture(directionalShadow, fragmentTextureCoordinate.xy).r;
}

/*
*   Calculates the directional light.
*/
vec3 CalculateDirectionalLight()
{
    //Calculate the directional light.
    vec3 lightDirection = -directionalLightDirection;
    vec3 radiance = mix(albedoColor, directionalLightColor, thickness) * directionalLightIntensity;

    return CalculateLight(  viewDirection,
                            lightDirection,
                            normalDirection,
                            thickness,
                            roughness,
                            viewAngle,
                            surfaceColor,
                            metallic,
                            albedoColor,
                            radiance) * CalculateDirectionalLightShadowMultiplier() * screenSpaceAmbientOcclusion;
}

/*
*   Calculates a single point light.
*/
vec3 CalculatePointLight(int index)
{
    //Calculate the point light.
    vec3 lightDirection = normalize(pointLightWorldPositions[index] - fragmentWorldPosition);

    float distanceToLightSource = length(fragmentWorldPosition - pointLightWorldPositions[index]);
    float attenuation = clamp(1.0f - distanceToLightSource / pointLightAttenuationDistances[index], 0.0f, 1.0f);
    attenuation *= attenuation;

    vec3 radiance = mix(albedoColor, pointLightColors[index], thickness) * pointLightIntensities[index] * attenuation;

    return CalculateLight(  viewDirection,
                            lightDirection,
                            normalDirection,
                            thickness,
                            roughness,
                            viewAngle,
                            surfaceColor,
                            metallic,
                            albedoColor,
                            radiance);
}

/*
*   Calculates the screen space ambient occlusion.
*/
void CalculateScreenSpaceAmbientOcclusion()
{
    screenSpaceAmbientOcclusion = 1.0f;
}

/*
*   Calculates a single spot light.
*/
vec3 CalculateSpotLight(int index)
{
    //Calculate the spot light.
    vec3 lightDirection = normalize(spotLightWorldPositions[index] - fragmentWorldPosition);
    float angle = dot(lightDirection, -spotLightDirections[index]);

    float distanceToLightSource = length(fragmentWorldPosition - spotLightWorldPositions[index]);
    float attenuation = clamp(1.0f - distanceToLightSource / spotLightAttenuationDistances[index], 0.0f, 1.0f);
    attenuation *= attenuation;

    //Calculate the inner/outer cone fade out.
    float epsilon = spotLightInnerCutoffAngles[index] - spotLightOuterCutoffAngles[index];
    float intensity = angle > spotLightInnerCutoffAngles[index] ? 1.0f : clamp((angle - spotLightOuterCutoffAngles[index]) / epsilon, 0.0f, 1.0f); 

    vec3 radiance = mix(albedoColor, spotLightColors[index], thickness) * spotLightIntensities[index] * intensity * attenuation;

    return CalculateLight(  viewDirection,
                            lightDirection,
                            normalDirection,
                            thickness,
                            roughness,
                            viewAngle,
                            surfaceColor,
                            metallic,
                            albedoColor,
                            radiance);
}

void main()
{
    //Sample values from the textures.
    albedoColor = pow(texture(albedoTexture, fragmentTextureCoordinate).rgb, vec3(2.2f));
    vec4 normalDirectionDepthSampler = texture(normalDirectionDepthTexture, fragmentTextureCoordinate);
    vec4 roughnessMetallicAmbientOcclusionSampler = texture(roughnessMetallicAmbientOcclusionTexture, fragmentTextureCoordinate);

    //Set the normal direction.
    normalDirection = normalDirectionDepthSampler.xyz;

    //Set the fragment depth.
    fragmentDepth = normalDirectionDepthSampler.a;

    //Calculate the world position of this fragment.
    fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, fragmentDepth);

    //Set the roughness.
    roughness = roughnessMetallicAmbientOcclusionSampler.r;

    //Set the metallic.
    metallic = roughnessMetallicAmbientOcclusionSampler.g;

    //Set the ambient occlusion.
    ambientOcclusion = roughnessMetallicAmbientOcclusionSampler.b;

    //Set the thickness.
    thickness = roughnessMetallicAmbientOcclusionSampler.a;

    //Calculate globals.
    viewDirection = normalize(cameraWorldPosition - fragmentWorldPosition);
    viewAngle = max(dot(normalDirection, viewDirection), 0.0f);
    surfaceColor = mix(vec3(0.04f), albedoColor, metallic);

    //Calculate the screen space ambient occlusion.
    CalculateScreenSpaceAmbientOcclusion();

    //Start off with just the ambient lighting.
    vec3 finalFragment = CalculateAmbient(  surfaceColor,
                                            roughness,
                                            viewAngle,
                                            metallic,
                                            nightDiffuseIrradianceTexture,
                                            dayDiffuseIrradianceTexture,
                                            normalDirection,
                                            albedoColor,
                                            viewDirection,
                                            nightDiffuseTexture,
                                            dayDiffuseTexture,
                                            ambientOcclusion);

    //Calculate the directional light.
    finalFragment += CalculateDirectionalLight();

    //Calculate all point lights.
    for (int i = 0; i < numberOfPointLights; ++i)
    {
        finalFragment += CalculatePointLight(i);
    }

    //Calculate all spot lights.
    for (int i = 0; i < numberOfSpotLights; ++i)
    {
        finalFragment += CalculateSpotLight(i);
    }

    //Set the final fragment color.
    fragmentColor = vec4(finalFragment, 1.0f);
}