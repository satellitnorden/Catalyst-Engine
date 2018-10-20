//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLightingInternals.glsl"

//Forward declarations.
vec3 CalculateAmbient(  vec3 surfaceColor,
                        float roughness,
                        float viewAngle,
                        float metallic,
                        samplerCube firstDiffseIrradianceTexture,
                        samplerCube secondDiffuseIrradianceTexture,
                        vec3 normalDirection,
                        vec3 albedoColor,
                        vec3 viewDirection,
                        samplerCube firstDiffseTexture,
                        samplerCube secondDiffuseTexture,
                        float ambientOcclusion);

vec3 CalculateDirectionalLight( vec3 albedoColor,
                                float thickness,
                                vec3 viewDirection,
                                vec3 normalDirection,
                                float roughness,
                                float viewAngle,
                                vec3 surfaceColor,
                                float metallic);

vec3 CalculateSpotLight(    int index,
                            vec3 fragmentWorldPosition,
                            float viewAngle,
                            vec3 viewDirection,
                            vec3 albedo,
                            vec3 normal,
                            float roughness,
                            float metallic,
                            float ambientOcclusion,
                            float thickness);

/*
*   Calculates the ambient lighting.
*/
vec3 CalculateAmbient(  vec3 surfaceColor,
                        float roughness,
                        float viewAngle,
                        float metallic,
                        samplerCube firstDiffseIrradianceTexture,
                        samplerCube secondDiffuseIrradianceTexture,
                        vec3 normalDirection,
                        vec3 albedoColor,
                        vec3 viewDirection,
                        samplerCube firstDiffseTexture,
                        samplerCube secondDiffuseTexture,
                        float ambientOcclusion)
{
    vec3 specularComponent = CalculateFresnelRoughness(surfaceColor, roughness, viewAngle);
    vec3 diffuseComponent = 1.0f - specularComponent;
    diffuseComponent *= 1.0f - metallic;

    vec3 irradiance = mix(texture(firstDiffseIrradianceTexture, normalDirection).rgb, texture(secondDiffuseIrradianceTexture, normalDirection).rgb, environmentBlend);
    vec3 diffuse = irradiance * albedoColor;

    vec3 reclectionDirection = reflect(-viewDirection, normalDirection);
    vec3 specularIrradiance = mix(texture(firstDiffseTexture, normalDirection).rgb, texture(secondDiffuseTexture, normalDirection).rgb, environmentBlend);
    vec3 specular = mix(specularIrradiance, irradiance, roughness);

    return (diffuse * diffuseComponent + specular * specularComponent) * ambientOcclusion;
}

/*
*   Calculates the directional light.
*/
vec3 CalculateDirectionalLight( vec3 albedoColor,
                                float thickness,
                                vec3 viewDirection,
                                vec3 normalDirection,
                                float roughness,
                                float viewAngle,
                                vec3 surfaceColor,
                                float metallic)
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
                            radiance);
}

/*
*   Calculates a single point light.
*/
vec3 CalculatePointLight(   int index,
                            vec3 fragmentWorldPosition,
                            float viewAngle,
                            vec3 viewDirection,
                            vec3 albedo,
                            vec3 normal,
                            float roughness,
                            float metallic,
                            float ambientOcclusion,
                            float thickness)
{
    //Calculate the point light.
    vec3 lightDirection = normalize(pointLightWorldPositions[index] - fragmentWorldPosition);

    float distanceToLightSource = length(fragmentWorldPosition - pointLightWorldPositions[index]);
    float attenuation = pow(clamp(1.0f - distanceToLightSource / pointLightAttenuationDistances[index], 0.0f, 1.0f), 2.0f);

    vec3 radiance = mix(albedo, pointLightColors[index], thickness) * pointLightIntensities[index] * attenuation;

    return CalculateLight(  viewDirection,
                            lightDirection,
                            normal,
                            thickness,
                            roughness,
                            viewAngle,
                            CalculateSurfaceColor(albedo, metallic),
                            metallic,
                            albedo,
                            radiance);
}

/*
*   Calculates a single spot light.
*/
vec3 CalculateSpotLight(    int index,
                            vec3 fragmentWorldPosition,
                            float viewAngle,
                            vec3 viewDirection,
                            vec3 albedo,
                            vec3 normal,
                            float roughness,
                            float metallic,
                            float ambientOcclusion,
                            float thickness)
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

    vec3 radiance = mix(albedo, spotLightColors[index], thickness) * spotLightIntensities[index] * intensity * attenuation;

    return CalculateLight(  viewDirection,
                            lightDirection,
                            normal,
                            thickness,
                            roughness,
                            viewAngle,
                            CalculateSurfaceColor(albedo, metallic),
                            metallic,
                            albedo,
                            radiance);
}