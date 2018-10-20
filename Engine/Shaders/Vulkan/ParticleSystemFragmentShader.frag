//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in float fragmentFadeFactor;
layout (location = 1) in vec2 fragmentTextureCoordinate;
layout (location = 2) in vec2 fragmentSceneTextureCoordinate;
layout (location = 3) in vec3 fragmentWorldPosition;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D sceneTexture;
layout (set = 1, binding = 1) uniform sampler2D normalDepthTexture;
layout (set = 2, binding = 1) uniform sampler2D albedoTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;
layout (location = 1) out vec4 normalDepth;

/*
*   Calculates the directional light.
*/
vec3 CalculateDirectionalLight()
{
    //Return the directional light color.
    return directionalLightColor * directionalLightIntensity;
}

/*
*   Calculates a single point light.
*/
vec3 CalculatePointLight(int index)
{
    //Start off with just the color of the point light.
    vec3 pointLightColor = pointLightColors[index];

    //Calculate the attenuation.
    float distanceToLightSource = length(fragmentWorldPosition - pointLightWorldPositions[index]);
    float attenuation = clamp(1.0f - distanceToLightSource / pointLightAttenuationDistances[index], 0.0f, 1.0f);
    attenuation *= attenuation;

    pointLightColor *= attenuation;

    //Return the spotlight color.
    return pointLightColor * pointLightIntensities[index];
}

/*
*   Calculates a single spot light.
*/
vec3 CalculateSpotLight(int index)
{
    //Start off with just the color of the spot light.
    vec3 spotLightColor = spotLightColors[index];

    //Get the angle between the normal and the light direction.
    vec3 lightDirection = normalize(fragmentWorldPosition - spotLightWorldPositions[index]);
    float lightAngle = dot(lightDirection, spotLightDirections[index]);

    //Calculate the attenuation.
    float distanceToLightSource = length(fragmentWorldPosition - spotLightWorldPositions[index]);
    float attenuation = clamp(1.0f - distanceToLightSource / spotLightAttenuationDistances[index], 0.0f, 1.0f);
    attenuation *= attenuation;

    spotLightColor *= attenuation;

    //Calculate the inner/outer cone fade out.
    float epsilon = spotLightInnerCutoffAngles[index] - spotLightOuterCutoffAngles[index];
    float intensity = lightAngle > spotLightInnerCutoffAngles[index] ? 1.0f : clamp((lightAngle - spotLightOuterCutoffAngles[index]) / epsilon, 0.0f, 1.0f); 

    spotLightColor *= intensity;

    //Return the spotlight color.
    return spotLightColor * spotLightIntensities[index];
}

void main()
{
    //Sample the albedo texture.
    vec4 albedoTextureSampler = pow(texture(albedoTexture, fragmentTextureCoordinate), vec4(2.2f));

    //Start off the final fragment with just some ambient lighting.
    vec3 finalFragment = albedoTextureSampler.rgb * 0.025f;

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

    //Calculate the blend factor.
    float blendFactor = albedoTextureSampler.a * fragmentFadeFactor;

    //Write the fragment and normal depth.
    vec4 sceneTextureSampler = texture(sceneTexture, fragmentSceneTextureCoordinate);
    vec4 normalDepthTextureSampler = texture(normalDepthTexture, fragmentSceneTextureCoordinate);

    fragment = vec4(mix(sceneTextureSampler.rgb, finalFragment, blendFactor), 1.0f);
    normalDepth = vec4(normalDepthTextureSampler.xyz, mix(normalDepthTextureSampler.a, gl_FragCoord.z, blendFactor));
}