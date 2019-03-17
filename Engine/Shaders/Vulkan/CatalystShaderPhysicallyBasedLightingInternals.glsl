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
*   Calculates the surface color of a fragment.
*/
vec3 CalculateSurfaceColor( vec3 albedo,
                            float metallic)
{
    return mix(vec3(0.04f), albedo, metallic);
}