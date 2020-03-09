//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLightingInternals.glsl"

/*
*   Calculates the diffuse component of a surface.
*/
float CalculateDiffuseComponent(float roughness, float metallic)
{
    return roughness * (1.0f - metallic);
}

/*
*   Calculates the indirect lighting.
*
*   viewDirection - Direction vector going from the surface point to the perceiving point.
*   albedo - The albedo of the surface at the surface point.
*   normal - The normal of the surface at the surface point.
*   roughness - The roughness of the surface at the surface point.
*   ambient_occlusion - The ambient oclusion of the surface at the surface point.
*   metallic - The metallic of the surface at the surface point.
*   irradiance - The irradiance in the hemisphere of the surface at the surface point.
*/
vec3 CalculateIndirectLighting( vec3 viewDirection,
                                vec3 albedo,
                                vec3 normal,
                                float roughness,
                                float metallic,
                                float ambient_occlusion,
                                vec3 irradiance)
{
    float viewAngle = max(dot(normal, viewDirection), 0.0f);
    vec3 specularComponent = CalculateFresnelRoughness(CalculateSurfaceColor(albedo, metallic), roughness, viewAngle);
    vec3 diffuseComponent = 1.0f - specularComponent;
    diffuseComponent *= 1.0f - metallic;

    vec3 diffuse_irradiance = irradiance * albedo;
    vec3 specular_irradiance = irradiance;

    return (diffuse_irradiance * diffuseComponent + specular_irradiance * specularComponent) * ambient_occlusion;
}

/*
*   Calculates a direct light.
*
*   viewDirection - Direction vector going from the surface point to the perceiving point.
*   lightDirection - Direction vector going from the surface point to the light point.
*   albedo - The albedo of the surface at the surface point.
*   normal - The normal vector of the surface point.
*   roughness - The roughness of the surface at the surface point.
*   metallic - The metallic of the surface at the surface point.
*   radiance - The radiance of the light.
*/
vec3 CalculateDirectLight(  vec3 viewDirection,
                            vec3 lightDirection,
                            vec3 albedo,
                            vec3 normal,
                            float roughness,
                            float metallic,
                            float ambient_occlusion,
                            float thickness,
                            vec3 radiance)
{
    vec3 halfwayDirection = normalize(viewDirection + lightDirection);
    float lightViewAngle = clamp(dot(halfwayDirection, viewDirection), 0.0f, 1.0f);
    float lightAngle = max(dot(normal, lightDirection), 0.0f);
    float viewAngle = max(dot(normal, viewDirection), 0.0f);

    //Apply thickness.
    lightAngle = mix(1.0f, lightAngle, thickness);

    float distribution = CalculateDistribution(roughness, normal, halfwayDirection);
    float geometry = CalculateGeometry(roughness, lightAngle, viewAngle);
    vec3 fresnel = CalculateFresnel(CalculateSurfaceColor(albedo, metallic), lightViewAngle);

    vec3 diffuseComponent = vec3(1.0f) - fresnel;
    diffuseComponent *= 1.0f - metallic;

    vec3 nominator = distribution * geometry * fresnel;
    float denominator = 4.0f * viewAngle * lightAngle;
    vec3 specularComponent = denominator != 0.0f ? nominator / denominator : vec3(0.0f);

    //Return the combined components.
    return (diffuseComponent * albedo / PI + specularComponent) * radiance * lightAngle * (0.25f + ambient_occlusion * 0.75f) * (1.0f - ((1.0f - thickness) * (1.0f - thickness)));
}