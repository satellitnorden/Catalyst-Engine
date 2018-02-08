//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, binding = 0) uniform DynamicUniformData
{
    //Camera data.
    mat4 inverseCameraMatrix;
    mat4 inverseProjectionMatrix;
    mat4 originViewMatrix;
    mat4 viewMatrix;
    vec3 cameraWorldPosition;
    float padding1;

    //Directional light data.
    float directionalLightIntensity;
    vec3 directionalLightDirection;
    vec3 directionalLightColor;
    vec3 directionalLightScreenSpacePosition;
    float padding2;

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
#define DIRECTIONAL_LIGHT_SCREEN_SHADE_SHADOWS_SAMPLES 250
#define PI 3.141592f

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (binding = 1) uniform samplerCube skyBoxTexture;
layout (binding = 2) uniform sampler2D albedoTexture;
layout (binding = 3) uniform sampler2D normalDirectionDepthTexture;
layout (binding = 4) uniform sampler2D roughnessMetallicAmbientOcclusionTexture;

//Out parameters.
layout (location = 0) out vec4 fragmentColor;

//Globals.
float fragmentDepth;
float roughness;
float metallic;
float ambientOcclusion;
float viewAngle;
vec3 viewDirection;
vec3 surfaceColor;
vec3 fragmentScreenSpacePosition;
vec3 fragmentWorldPosition;
vec3 albedoColor;
vec3 normalDirection;


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
    return surfaceColor + (max(vec3(1.0f - roughness), surfaceColor) - surfaceColor) * pow(max(1.0f - lightViewAngle, 0.0f), 5.0f);
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
    vec3 specularComponent = vec3(0.0f);
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
*   Returns the directional light screen space shadow multiplier.
*/
float CalculateDirectionalLightScreenSpaceShadowMultiplier()
{
    return 1.0f;

    vec3 rayDirection = directionalLightScreenSpacePosition - vec3((fragmentScreenSpacePosition.xy) + 1.0f / 2.0f, fragmentScreenSpacePosition.z);
    vec3 rayStep = rayDirection / DIRECTIONAL_LIGHT_SCREEN_SHADE_SHADOWS_SAMPLES;
    vec3 currentScreenSpacePosition = fragmentScreenSpacePosition;
    currentScreenSpacePosition.xy = (currentScreenSpacePosition.xy) + 1.0f / 2.0f;

    for (int i = 0; i < DIRECTIONAL_LIGHT_SCREEN_SHADE_SHADOWS_SAMPLES; ++i)
    {
        //Calculate the texture coordinates.
        vec2 sampleTextureCoordinates = vec2(currentScreenSpacePosition.xy);

        //Don't sample outside of the screen quad. ):
        if (sampleTextureCoordinates.x < 0.0f || sampleTextureCoordinates.x > 1.0f || sampleTextureCoordinates.y < 0.0f || sampleTextureCoordinates.y > 1.0f)
        {
            return 1.0f;
        }

        //Sample the depth at this point.
        float sampleDepth = texture(normalDirectionDepthTexture, sampleTextureCoordinates).a;

        //If the sampled depth is lower than the depth of the current screen space position, an occlusion has been found.
        if (sampleDepth < currentScreenSpacePosition.z)
        {
            return 0.0f;
        }

        //Else advance the current screen space position.
        else
        {
            currentScreenSpacePosition += rayStep;
        }
    }

    //No occlusion was found.
    return 1.0f;   
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
*/
vec3 CalculateDirectionalLight()
{
    //Calculate the directional light.
    vec3 lightDirection = -directionalLightDirection;
    vec3 radiance = directionalLightColor;

    return CalculateLight(lightDirection, radiance, directionalLightIntensity) * CalculateDirectionalLightScreenSpaceShadowMultiplier();
}

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
    vec4 normalDirectionDepthSampler = texture(normalDirectionDepthTexture, fragmentTextureCoordinate);
    vec4 roughnessMetallicAmbientOcclusionSampler = texture(roughnessMetallicAmbientOcclusionTexture, fragmentTextureCoordinate);

    //Set the normal direction.
    normalDirection = normalDirectionDepthSampler.xyz;

    //Set the fragment depth.
    fragmentDepth = normalDirectionDepthSampler.a;

    //Calculate the world position of this fragment.
    vec2 nearPlaneCoordinate = fragmentTextureCoordinate * 2.0f - 1.0f;
    fragmentScreenSpacePosition = vec3(nearPlaneCoordinate, fragmentDepth);
    vec4 viewSpacePosition = inverseProjectionMatrix * vec4(fragmentScreenSpacePosition, 1.0f);
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseCameraMatrix * viewSpacePosition;

    fragmentWorldPosition = worldSpacePosition.xyz;

    //Set the roughness.
    roughness = roughnessMetallicAmbientOcclusionSampler.r;

    //Set the metallic.
    metallic = roughnessMetallicAmbientOcclusionSampler.g;

    //Set the ambient occlusion.
    ambientOcclusion = roughnessMetallicAmbientOcclusionSampler.b;

    //Calculate globals.
    viewDirection = normalize(cameraWorldPosition - fragmentWorldPosition);
    viewAngle = max(dot(normalDirection, viewDirection), 0.0f);
    surfaceColor = mix(vec3(0.04f), albedoColor, metallic);

    //Start off with just the ambient lighting.
    vec3 finalFragment = CalculateAmbient();

    //Calculate the directional light.
    finalFragment += CalculateDirectionalLight();

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

    //Set the final fragment color.
    fragmentColor = vec4(finalFragment, 1.0f);
}