//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLightingInternals.glsl"

/*
*   Calculates the ambient lighting.
*/
vec3 CalculateAmbient(  vec3 surfaceColor,
                        float roughness,
                        float viewAngle,
                        float metallic,
                        vec3 irradiance,
                        vec3 normalDirection,
                        vec3 albedoColor,
                        vec3 viewDirection,
                        vec3 specularIrradiance,
                        float ambientOcclusion)
{
    vec3 specularComponent = CalculateFresnelRoughness(surfaceColor, roughness, viewAngle);
    vec3 diffuseComponent = 1.0f - specularComponent;
    diffuseComponent *= 1.0f - metallic;

    vec3 diffuse = irradiance * albedoColor;

    vec3 reclectionDirection = reflect(-viewDirection, normalDirection);
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

/*
*   Calculates the full lighting for a fragment.
*/
vec3 CalculateLighting( vec3 diffuseIrradiance,
                        vec3 specularIrradiance,
                        float directionalShadowMultiplier,
                        vec3 fragmentWorldPosition,
                        vec3 albedo,
                        vec3 normal,
                        float roughness,
                        float metallic,
                        float ambientOcclusion,
                        float thickness)
{
    //Calculate the view direction.
    vec3 viewDirection = normalize(cameraWorldPosition - fragmentWorldPosition);

    //Calculate the view angle.
    float viewAngle = max(dot(normal, viewDirection), 0.0f);

    //Calculate the surface color.
    vec3 surfaceColor = CalculateSurfaceColor(albedo, metallic);

    //Start off with just the ambient lighting.
    vec3 finalFragment = CalculateAmbient(  surfaceColor,
                                            roughness,
                                            viewAngle,
                                            metallic,
                                            diffuseIrradiance,
                                            normal,
                                            albedo,
                                            viewDirection,
                                            specularIrradiance,
                                            ambientOcclusion);

    //Calculate the directional light.
    finalFragment += CalculateDirectionalLight( albedo,
                                                thickness,
                                                viewDirection,
                                                normal,
                                                roughness,
                                                viewAngle,
                                                surfaceColor,
                                                metallic) * directionalShadowMultiplier * Scale(ambientOcclusion, 0.0f, 1.0f, 0.25f, 1.0f);

    //Calculate all point lights.
    for (int i = 0; i < numberOfPointLights; ++i)
    {
        finalFragment += CalculatePointLight(   i,
                                                fragmentWorldPosition, 
                                                viewAngle,
                                                viewDirection,
                                                albedo,
                                                normal,
                                                roughness,
                                                metallic,
                                                ambientOcclusion,
                                                thickness);
    }

    //Calculate all spot lights.
    for (int i = 0; i < numberOfSpotLights; ++i)
    {
        finalFragment += CalculateSpotLight(    i,
                                                fragmentWorldPosition, 
                                                viewAngle,
                                                viewDirection,
                                                albedo,
                                                normal,
                                                roughness,
                                                metallic,
                                                ambientOcclusion,
                                                thickness);
    }

    //Return the final fragment.
    return finalFragment;
}