//Includes.
#include "CatalystShaderCommon.glsl"

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

float CalculateDistribution(    float roughness,
                                vec3 normalDirection,
                                vec3 halfwayDirection);

vec3 CalculateFresnel(  vec3 surfaceColor,
                        float lightViewAngle);

vec3 CalculateFresnelRoughness( vec3 surfaceColor,
                                float roughness,
                                float lightViewAngle);

float CalculateGeometry(    float roughness,
                            float lightAngle,
                            float viewAngle);

vec3 CalculateLight(    vec3 viewDirection,
                        vec3 lightDirection,
                        vec3 normalDirection,
                        float thickness,
                        float roughness,
                        float viewAngle,
                        vec3 surfaceColor,
                        float metallic,
                        vec3 radiance);

/*
*   Calculates the ambient component of this fragment.
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
*   Calculates the distribution.
*/
float CalculateDistribution(    float roughness,
                                vec3 normalDirection,
                                vec3 halfwayDirection)
{
    float roughnessSquared = roughness * roughness;
    float roughnessSquaredSquared = roughnessSquared * roughnessSquared;
    float normalHalfwayAngle = max(dot(normalDirection, halfwayDirection), 0.0f);
    float normalHalfwayAngleSquared = normalHalfwayAngle * normalHalfwayAngle;

    float denominator = normalHalfwayAngleSquared * (roughnessSquaredSquared - 1.0f) + 1.0f;

    return roughnessSquaredSquared / (PI * denominator * denominator);
}

/*
*   Calculates the fresnel.
*/
vec3 CalculateFresnel(  vec3 surfaceColor,
                        float lightViewAngle)
{
    return surfaceColor + (1.0f - surfaceColor) * pow(max(1.0f - lightViewAngle, 0.0f), 5.0f);
}

/*
*   Calculates the fresnel with roughness in mind.
*/
vec3 CalculateFresnelRoughness( vec3 surfaceColor,
                                float roughness,
                                float lightViewAngle)
{
    return surfaceColor + (max(vec3(1.0f - roughness), surfaceColor) - surfaceColor) * pow(max(1.0f - lightViewAngle, 0.0f), 5.0f);
}

/*
*   Calculates the geometry.
*/
float CalculateGeometry(    float roughness,
                            float lightAngle,
                            float viewAngle)
{
    float geometryRoughness = roughness + 1.0f;
    geometryRoughness = (geometryRoughness * geometryRoughness) / 8.0f;

    float lightObstruction = lightAngle / (lightAngle * (1.0f - geometryRoughness) + geometryRoughness);
    float viewObstruction = viewAngle / (viewAngle * (1.0f - geometryRoughness) + geometryRoughness);

    return lightObstruction * viewObstruction;
}

/*
*   Calculates a light.
*/
vec3 CalculateLight(    vec3 viewDirection,
                        vec3 lightDirection,
                        vec3 normalDirection,
                        float thickness,
                        float roughness,
                        float viewAngle,
                        vec3 surfaceColor,
                        float metallic,
                        vec3 albedoColor,
                        vec3 radiance)
{
    vec3 halfwayDirection = normalize(viewDirection + lightDirection);
    float lightViewAngle = clamp(dot(halfwayDirection, viewDirection), 0.0f, 1.0f);
    float lightAngle = mix(1.0f, max(dot(normalDirection, lightDirection), 0.0f), thickness);

    float distribution = CalculateDistribution(roughness, normalDirection, halfwayDirection);
    float geometry = CalculateGeometry(roughness, lightAngle, viewAngle);
    vec3 fresnel = CalculateFresnel(surfaceColor, lightViewAngle);

    vec3 diffuseComponent = vec3(1.0f) - fresnel;
    diffuseComponent *= 1.0f - metallic;

    vec3 nominator = distribution * geometry * fresnel;
    float denominator = 4 * viewAngle * lightAngle + 0.001f;
    vec3 specularComponent = nominator / denominator;

    //Return the combined components.
    return (diffuseComponent * albedoColor / PI + specularComponent) * radiance * lightAngle;
}