//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLightingInternals.glsl"

//Preprocessor defines.
#define CATALYST_PHYSICALLY_BASED_LIGHTING_DIRECTIONAL_LIGHT_AMBIENT_OCCLUSION_SCALE (0.25f)
#define CATALYST_PHYSICALLY_BASED_LIGHTING_POINT_LIGHT_AMBIENT_OCCLUSION_SCALE (0.5f)
#define CATALYST_PHYSICALLY_BASED_LIGHTING_SPOT_LIGHT_AMBIENT_OCCLUSION_SCALE (0.5f)

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
                                float metallic,
                                float ambientOcclusion)
{
    //Calculate the directional light.
    vec3 lightDirection = -directionalLightDirection;
    vec3 radiance = directionalLightColor * directionalLightIntensity;

    return CalculateLight(  viewDirection,
                            lightDirection,
                            normalDirection,
                            thickness,
                            roughness,
                            viewAngle,
                            surfaceColor,
                            metallic,
                            albedoColor,
                            radiance) * Scale(ambientOcclusion, 0.0f, 1.0f, CATALYST_PHYSICALLY_BASED_LIGHTING_DIRECTIONAL_LIGHT_AMBIENT_OCCLUSION_SCALE, 1.0f);
}

/*
*   Calculates a single point light.
*/
vec3 CalculatePointLight(   vec3 pointLightWorldPosition,
                            float pointLightAttenuationDistance,
                            vec3 pointLightColor,
                            float pointLightIntensity,
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
    vec3 lightDirection = normalize(pointLightWorldPosition - fragmentWorldPosition);

    float distanceToLightSource = length(fragmentWorldPosition - pointLightWorldPosition);
    float attenuation = pow(clamp(1.0f - distanceToLightSource / pointLightAttenuationDistance, 0.0f, 1.0f), 2.0f);

    vec3 radiance = pointLightColor * pointLightIntensity * attenuation;

    return CalculateLight(  viewDirection,
                            lightDirection,
                            normal,
                            thickness,
                            roughness,
                            viewAngle,
                            CalculateSurfaceColor(albedo, metallic),
                            metallic,
                            albedo,
                            radiance) * Scale(ambientOcclusion, 0.0f, 1.0f, CATALYST_PHYSICALLY_BASED_LIGHTING_POINT_LIGHT_AMBIENT_OCCLUSION_SCALE, 1.0f);
}

/*
*   Calculates a single spot light.
*/
vec3 CalculateSpotLight(    vec3 spotLightWorldPosition,
                            vec3 spotLightDirection,
                            float spotLightAttenuationDistance,
                            float spotLightInnerCutoffAngle,
                            float spotLightOuterCutoffAngle,
                            vec3 spotLightColor,
                            float spotLightIntensity,
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
    vec3 lightDirection = normalize(spotLightWorldPosition - fragmentWorldPosition);
    float angle = dot(lightDirection, -spotLightDirection);

    float distanceToLightSource = length(fragmentWorldPosition - spotLightWorldPosition);
    float attenuation = clamp(1.0f - distanceToLightSource / spotLightAttenuationDistance, 0.0f, 1.0f);
    attenuation *= attenuation;

    //Calculate the inner/outer cone fade out.
    float epsilon = spotLightInnerCutoffAngle - spotLightOuterCutoffAngle;
    float intensity = angle > spotLightInnerCutoffAngle ? 1.0f : clamp((angle - spotLightOuterCutoffAngle) / epsilon, 0.0f, 1.0f); 

    vec3 radiance = spotLightColor * spotLightIntensity * intensity * attenuation;

    return CalculateLight(  viewDirection,
                            lightDirection,
                            normal,
                            thickness,
                            roughness,
                            viewAngle,
                            CalculateSurfaceColor(albedo, metallic),
                            metallic,
                            albedo,
                            radiance) * Scale(ambientOcclusion, 0.0f, 1.0f, CATALYST_PHYSICALLY_BASED_LIGHTING_SPOT_LIGHT_AMBIENT_OCCLUSION_SCALE, 1.0f);
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
    vec3 viewDirection = normalize(perceiverWorldPosition - fragmentWorldPosition);

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
                                                metallic,
                                                ambientOcclusion) * directionalShadowMultiplier;

    //Return the final fragment.
    return finalFragment;
}