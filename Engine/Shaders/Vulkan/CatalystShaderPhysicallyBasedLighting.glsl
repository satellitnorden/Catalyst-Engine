//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLightingInternals.glsl"

//Preprocessor defines.
#define CATALYST_PHYSICALLY_BASED_LIGHTING_DIRECTIONAL_LIGHT_AMBIENT_OCCLUSION_SCALE (0.25f)
#define CATALYST_PHYSICALLY_BASED_LIGHTING_POINT_LIGHT_AMBIENT_OCCLUSION_SCALE (0.5f)
#define CATALYST_PHYSICALLY_BASED_LIGHTING_SPOT_LIGHT_AMBIENT_OCCLUSION_SCALE (0.5f)

/*
*   Calculates the ambient lighting.
*
*   albedo - The albedo of the surface at the surface point.
*   diffuseIrradiance - The diffuse irradiance in the reflection direction of the surface at the surface point.
*   normal - The normal of the surface at the surface point.
*   specularIrradiance - The specular irradiance in the reflection direction of the surface at the surface point.
*   viewDirection - Direction vector going from the surface point to the perceiving point.
*   ambientOcclusion - The ambient oclusion of the surface at the surface point.
*   metallic - The metallic of the surface at the surface point.
*   roughness - The roughness of the surface at the surface point.
*/
vec3 CalculateAmbient(  vec3 albedo,
                        vec3 diffuseIrradiance,
                        vec3 normal,
                        vec3 specularIrradiance,
                        vec3 viewDirection,
                        float ambientOcclusion,
                        float metallic,
                        float roughness)
{
    float viewAngle = max(dot(normal, viewDirection), 0.0f);
    vec3 specularComponent = CalculateFresnelRoughness(CalculateSurfaceColor(albedo, metallic), roughness, viewAngle);
    vec3 diffuseComponent = 1.0f - specularComponent;
    diffuseComponent *= 1.0f - metallic;

    vec3 diffuse = diffuseIrradiance * albedo;

    vec3 specular = mix(specularIrradiance, diffuseIrradiance, roughness);

    return (diffuse * diffuseComponent + specular * specularComponent) * ambientOcclusion;
}

/*
*   Calculates a light.
*
*   viewDirection - Direction vector going from the surface point to the perceiving point.
*   lightDirection - Direction vector going from the surface point to the light point.
*   normal - The normal vector of the surface point.
*   thickness - The thickness of the surface at the surface point.
*   roughness - The roughness of the surface at the surface point.
*   metallic - The metallic of the surface at the surface point.
*   albedo - The albedo of the surface at the surface point.
*   radiance - The radiance of the light.
*/
vec3 CalculateLight(vec3 viewDirection,
                    vec3 lightDirection,
                    vec3 normal,
                    float thickness,
                    float roughness,
                    float metallic,
                    vec3 albedo,
                    vec3 radiance)
{
    vec3 halfwayDirection = normalize(viewDirection + lightDirection);
    float lightViewAngle = clamp(dot(halfwayDirection, viewDirection), 0.0f, 1.0f);
    float lightAngle = mix(1.0f, max(dot(normal, lightDirection), 0.0f), thickness);
    float viewAngle = max(dot(normal, viewDirection), 0.0f);

    float distribution = CalculateDistribution(roughness, normal, halfwayDirection);
    float geometry = CalculateGeometry(roughness, lightAngle, viewAngle);
    vec3 fresnel = CalculateFresnel(CalculateSurfaceColor(albedo, metallic), lightViewAngle);

    vec3 diffuseComponent = vec3(1.0f) - fresnel;
    diffuseComponent *= 1.0f - metallic;

    vec3 nominator = distribution * geometry * fresnel;
    float denominator = 4.0f * viewAngle * lightAngle;
    vec3 specularComponent = denominator != 0.0f ? nominator / denominator : vec3(0.0f);

    //Return the combined components.
    return (diffuseComponent * albedo / PI + specularComponent) * radiance * lightAngle;
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
    vec3 finalFragment = CalculateAmbient(  albedo,
                                            diffuseIrradiance,
                                            normal,
                                            specularIrradiance,
                                            viewDirection,
                                            ambientOcclusion,
                                            metallic,
                                            roughness);

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