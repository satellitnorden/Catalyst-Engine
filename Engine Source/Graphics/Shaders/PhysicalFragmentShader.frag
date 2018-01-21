//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, binding = 0) uniform DynamicUniformData
{
  //Camera data.
  mat4 originViewMatrix;
  mat4 viewMatrix;
  vec3 cameraWorldPosition;
  float padding1;

  //Point light data.
  int numberOfPointLights;
    float pointLightAttenuationDistances[MaximumNumberOfPointLights];
    float pointLightIntensities[MaximumNumberOfPointLights];
    vec3 pointLightColors[MaximumNumberOfPointLights];
    vec3 pointLightWorldPositions[MaximumNumberOfPointLights];

  //Spot light data.
  int numberOfSpotLights;
  float spotLightAttenuationDistances[MaximumNumberOfSpotLights];
  float spotLightIntensities[MaximumNumberOfSpotLights];
  float spotLightInnerCutoffAngles[MaximumNumberOfSpotLights];
  float spotLightOuterCutoffAngles[MaximumNumberOfSpotLights];
  vec3 spotLightColors[MaximumNumberOfSpotLights];
  vec3 spotLightDirections[MaximumNumberOfSpotLights];
  vec3 spotLightWorldPositions[MaximumNumberOfSpotLights];
};

//Preprocessor defines.
#define PI 3.141592f

//Texture samplers.
layout (binding = 2) uniform samplerCube skyBoxTexture;
layout (binding = 3) uniform sampler2D albedoTexture;
layout (binding = 4) uniform sampler2D normalMapTexture;
layout (binding = 5) uniform sampler2D roughnessTexture;
layout (binding = 6) uniform sampler2D metallicTexture;
layout (binding = 7) uniform sampler2D ambientOcclusionTexture;

layout (location = 0) in vec3 fragmentWorldPosition;
layout (location = 1) in mat3 fragmentTangentSpaceMatrix;
layout (location = 4) in vec2 fragmentTextureCoordinate;

layout (location = 0) out vec4 fragmentColor;

//Globals.
vec3 albedoColor;
vec3 normalDirection;
float roughness;
float metallic;
float ambientOcclusion;
vec3 viewDirection;
float viewAngle;
vec3 surfaceColor;

/*
*   Calculates the distribution.
*/
float CalculateDistribution(vec3 halfwayDirection)
{
    float roughnessSquared = roughness * roughness;
    float roughnessSquaredSquared = roughnessSquared * roughnessSquared;
    float normalHalfwayAngle = max(dot(normalDirection, halfwayDirection), 0.0f);
    float normalHalfwayAngleSquared = normalHalfwayAngle * normalHalfwayAngle;

    float denominator = normalHalfwayAngleSquared * (roughnessSquaredSquared - 1.0f) + 1.0f;

    return roughnessSquaredSquared / (PI * denominator * denominator);
}

/*
*   Calculates the geometry.
*/
float CalculateGeometry(float lightAngle)
{
    float geometryRoughness = roughness + 1.0f;
    geometryRoughness = (geometryRoughness * geometryRoughness) / 8.0f;

    float lightObstruction = lightAngle / (lightAngle * (1.0f - geometryRoughness) + geometryRoughness);
    float viewObstruction = viewAngle / (viewAngle * (1.0f - geometryRoughness) + geometryRoughness);

    return lightObstruction * viewObstruction;
}

/*
*   Calculates the fresnel, with roughness in mind.
*/
vec3 CalculateFresnelRoughness(float lightViewAngle)
{
    return surfaceColor + (max(vec3(1.0f - roughness), surfaceColor) - surfaceColor) * pow(1.0f - lightViewAngle, 5.0f);
}

/*
*   Calculates the fresnel.
*/
vec3 CalculateFresnel(float lightViewAngle)
{
    return surfaceColor + (1.0f - surfaceColor) * pow(max(1.0f - lightViewAngle, 0.0f), 5.0f);
}

/*
*   Calculates the ambient component of this fragment.
*/
vec3 CalculateAmbient()
{
    vec3 specularComponent = mix(vec3(0.0f), CalculateFresnelRoughness(viewAngle), 0.1f);
    vec3 diffuseComponent = 1.0f - specularComponent;
    diffuseComponent *= 1.0f - metallic;

    vec3 irradiance = texture(skyBoxTexture, normalDirection).rgb;
    vec3 diffuse = irradiance * albedoColor;

    vec3 reclectionDirection = reflect(-viewDirection, normalDirection);
    vec3 specularIrradiance = texture(skyBoxTexture, normalDirection).rgb;
    vec3 specular = mix(specularIrradiance, irradiance, roughness);

    return (diffuse * diffuseComponent + specular * specularComponent) * ambientOcclusion;
}

/*
*   Calculates a light.
*/
vec3 CalculateLight(vec3 lightDirection, vec3 radiance, float intensity)
{
    vec3 halfwayDirection = normalize(viewDirection + lightDirection);
    float lightViewAngle = clamp(dot(halfwayDirection, viewDirection), 0.0f, 1.0f);
    float lightAngle = max(dot(normalDirection, lightDirection), 0.0f);

    float distribution = CalculateDistribution(halfwayDirection);
    float geometry = CalculateGeometry(lightAngle);
    vec3 fresnel = CalculateFresnel(lightViewAngle);

    vec3 diffuseComponent = vec3(1.0f) - fresnel;
    diffuseComponent *= 1.0f - metallic;

    vec3 nominator = distribution * geometry * fresnel;
    float denominator = 4 * viewAngle * lightAngle + 0.001f;
    vec3 specularComponent = nominator / denominator;

    //Return the combined components.
    return (diffuseComponent * albedoColor / PI + specularComponent) * radiance * lightAngle * intensity;
}

/*
*   Calculates the directional light shadow multiplier.

float CalculateDirectionalLightShadowMultiplier()
{
    //Calculate if this fragment's world position is in shadow of the directional light.
    vec4 directionalLightShadowMapCoordinate = directionalLightMatrix * vec4(fragmentWorldPosition, 1.0f);
    directionalLightShadowMapCoordinate = directionalLightShadowMapCoordinate * 0.5f + 0.5f;
    float fragmentDepth = directionalLightShadowMapCoordinate.z;

    float texelStep = 1.0f / 4096.0f;

    int numberOfShadowTexels = 3;

    float xCoordinate = -texelStep * numberOfShadowTexels;
    float yCoordinate = -texelStep * numberOfShadowTexels;

    float accumulatedShadowValue = 0.0f;

    for (int i = 0; i < numberOfShadowTexels; ++i)
    {
        for (int j = 0; j < numberOfShadowTexels; ++j)
        {
            float nonShadowDepth = texture(directionalLightShadowMapTexture, directionalLightShadowMapCoordinate.xy + vec2(xCoordinate, yCoordinate)).r;

            accumulatedShadowValue += fragmentDepth < nonShadowDepth || directionalLightShadowMapCoordinate.z > 1.0f ? 1.0f : 0.0f;

            yCoordinate += texelStep;
        }

        xCoordinate += texelStep;
        yCoordinate = -texelStep * numberOfShadowTexels;
    }

    return accumulatedShadowValue / (numberOfShadowTexels * numberOfShadowTexels);
}
*/

/*
*   Calculates the directional light.

vec3 CalculateDirectionalLight()
{
    //Calculate the directional light.
    vec3 lightDirection = -directionalLightDirection;
    vec3 radiance = directionalLightColor;

    return CalculateLight(lightDirection, radiance, directionalLightIntensity) * CalculateDirectionalLightShadowMultiplier();
}
*/

/*
*   Calculates a single point light.
*/
vec3 CalculatePointLight(int index)
{
    //Calculate the point light.
    vec3 lightDirection = normalize(pointLightWorldPositions[index] - fragmentWorldPosition);

    float distanceToLightSource = length(fragmentWorldPosition - pointLightWorldPositions[index]);
    float attenuation = clamp(1.0f - distanceToLightSource / pointLightAttenuationDistances[index], 0.0f, 1.0f);
    attenuation *= attenuation;

    vec3 radiance = pointLightColors[index] * attenuation;

    return CalculateLight(lightDirection, radiance, pointLightIntensities[index]);
}

/*
*   Calculates a single spot light.
*/
vec3 CalculateSpotLight(int index)
{
    //Calculate the spot light.
    vec3 lightDirection = normalize(spotLightWorldPositions[index] - fragmentWorldPosition);
    float angle = dot(lightDirection, -spotLightDirections[index]);

    float distanceToLightSource = length(fragmentWorldPosition - spotLightWorldPositions[index]);
    float attenuation = clamp(1.0f - distanceToLightSource / spotLightAttenuationDistances[index], 0.0f, 1.0f);
    attenuation *= attenuation;

    vec3 radiance = spotLightColors[index] * attenuation;

    vec3 calculatedLight = CalculateLight(lightDirection, radiance, spotLightIntensities[index]);

    //Calculate the inner/outer cone fade out.
    float epsilon = spotLightInnerCutoffAngles[index] - spotLightOuterCutoffAngles[index];
    float intensity = angle > spotLightInnerCutoffAngles[index] ? 1.0f : clamp((angle - spotLightOuterCutoffAngles[index]) / epsilon, 0.0f, 1.0f); 

    calculatedLight *= intensity;

    return calculatedLight;
}

void main()
{
    //Sample values from the textures.
    albedoColor = pow(texture(albedoTexture, fragmentTextureCoordinate).rgb, vec3(2.2f));

    //Calculate the normal direction.
    normalDirection = texture(normalMapTexture, fragmentTextureCoordinate).xyz * 2.0f - 1.0f;
    normalDirection = fragmentTangentSpaceMatrix * normalDirection;
    normalDirection = normalize(normalDirection);

    roughness = texture(roughnessTexture, fragmentTextureCoordinate).r;
    metallic = texture(metallicTexture, fragmentTextureCoordinate).r;
    ambientOcclusion = texture(ambientOcclusionTexture, fragmentTextureCoordinate).r;

    //Calculate globals.
    viewDirection = normalize(cameraWorldPosition - fragmentWorldPosition);
    viewAngle = max(dot(normalDirection, viewDirection), 0.0f);
    surfaceColor = mix(vec3(0.04f), albedoColor, metallic);

    //Start off with just the ambient lighting.
    vec3 finalFragment = CalculateAmbient();

    /*
    //Calculate the directional light.
    finalFragment += CalculateDirectionalLight();
    */

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

    //Apply gamma correction.
    finalFragment = pow(finalFragment, vec3(1.0f / 2.2f));

    //Set the final fragment color.
    fragmentColor = vec4(finalFragment, 1.0f);
}