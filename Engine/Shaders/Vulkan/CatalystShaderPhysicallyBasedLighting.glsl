//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLightingInternals.glsl"

//Preprocessor defines.
#define CATALYST_PHYSICALLY_BASED_LIGHTING_DIRECTIONAL_LIGHT_AMBIENT_OCCLUSION_SCALE (0.25f)
#define CATALYST_PHYSICALLY_BASED_LIGHTING_POINT_LIGHT_AMBIENT_OCCLUSION_SCALE (0.5f)
#define CATALYST_PHYSICALLY_BASED_LIGHTING_SPOT_LIGHT_AMBIENT_OCCLUSION_SCALE (0.5f)

/*
*   Calculates an indirect light.
*
*   viewDirection - Direction vector going from the surface point to the perceiving point.
*   lightDirection - Direction vector going from the surface point to the light point.
*   albedo - The albedo of the surface at the surface point.
*   normal - The normal of the surface at the surface point.
*   roughness - The roughness of the surface at the surface point.
*   ambientOcclusion - The ambient oclusion of the surface at the surface point.
*   metallic - The metallic of the surface at the surface point.
*   irradiance - The irradiance in the reflection direction of the surface at the surface point.
*/
vec3 CalculateIndirectLight(vec3 viewDirection,
                            vec3 lightDirection,
                            vec3 albedo,
                            vec3 normal,
                            float roughness,
                            float metallic,
                            float ambientOcclusion,
                            vec3 irradiance)
{
    vec3 halfwayDirection = normalize(viewDirection + lightDirection);
    float lightViewAngle = clamp(dot(halfwayDirection, viewDirection), 0.0f, 1.0f);
    float lightAngle = max(dot(normal, lightDirection), 0.0f);
    float viewAngle = max(dot(normal, viewDirection), 0.0f);

    float distribution = CalculateDistribution(roughness, normal, halfwayDirection);
    float geometry = CalculateGeometry(roughness, lightAngle, viewAngle);
    vec3 fresnel = CalculateFresnelRoughness(CalculateSurfaceColor(albedo, metallic), lightViewAngle, roughness);

    vec3 diffuseComponent = vec3(1.0f) - fresnel;
    diffuseComponent *= 1.0f - metallic;

    vec3 nominator = distribution * geometry * fresnel;
    float denominator = 4.0f * viewAngle * lightAngle;
    vec3 specularComponent = denominator != 0.0f ? nominator / denominator : vec3(0.0f);

    //Return the combined components.
    return (diffuseComponent * albedo / PI + specularComponent) * irradiance * lightAngle;
}

/*
*   Calculates a direct light.
*
*   viewDirection - Direction vector going from the surface point to the perceiving point.
*   lightDirection - Direction vector going from the surface point to the light point.
*   albedo - The albedo of the surface at the surface point.
*   normal - The normal vector of the surface point.
*   thickness - The thickness of the surface at the surface point.
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
                            vec3 radiance)
{
    vec3 halfwayDirection = normalize(viewDirection + lightDirection);
    float lightViewAngle = clamp(dot(halfwayDirection, viewDirection), 0.0f, 1.0f);
    float lightAngle = max(dot(normal, lightDirection), 0.0f);
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