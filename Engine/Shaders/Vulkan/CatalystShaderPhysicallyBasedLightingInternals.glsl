//Forward declarations.
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
                        vec3 albedoColor,
                        vec3 radiance);

vec3 CalculateSurfaceColor( vec3 albedo,
                            float metallic);

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

/*
*   Calculates the surface color of a fragment.
*/
vec3 CalculateSurfaceColor( vec3 albedo,
                            float metallic)
{
    return mix(vec3(0.04f), albedo, metallic);
}