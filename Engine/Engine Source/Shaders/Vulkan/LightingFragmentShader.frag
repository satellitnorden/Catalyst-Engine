//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, set = 0, binding = 0) uniform DynamicUniformData
{
    //Camera data.
    layout (offset = 0) float cameraFieldOfViewCosine; //Offset; 0 - Size; 16
    layout (offset = 16) mat4 inverseCameraMatrix; //Offset; 16 - Size; 64
    layout (offset = 80) mat4 inverseProjectionMatrix; //Offset; 80 - Size; 64
    layout (offset = 144) mat4 originViewMatrix; //Offset; 144 - Size; 64
    layout (offset = 208) mat4 viewMatrix; //Offset; 208 - Size; 64
    layout (offset = 272) vec3 cameraForwardVector; //Offset; 272 - Size; 16
    layout (offset = 288) vec3 cameraWorldPosition; //Offset; 288 - Size; 16

    //Directional light data.
    layout (offset = 304) float directionalLightIntensity; //Offset; 304 - Size; 16
    layout (offset = 320) vec3 directionalLightDirection; //Offset; 320 - Size; 16
    layout (offset = 336) vec3 directionalLightColor; //Offset; 336 - Size; 16
    layout (offset = 352) vec3 directionalLightScreenSpacePosition; //Offset; 352 - Size; 16

    //General data.
    layout (offset = 368) float deltaTime; //Offset; 368 - Size; 4
    layout (offset = 372) float randomSeed; //Offset; 372 - Size; 4
    layout (offset = 376) float totalGameTime; //Offset; 376 - Size; 4

    //Point light data.
    layout (offset = 380) int numberOfPointLights; //Offset; 380 - Size; 4
    layout (offset = 384) float pointLightAttenuationDistances[MaximumNumberOfPointLights]; //Offset; 384 - Size; 128
    layout (offset = 512) float pointLightIntensities[MaximumNumberOfPointLights]; //Offset; 512 - Size; 128
    layout (offset = 640) vec3 pointLightColors[MaximumNumberOfPointLights]; //Offset; 640 - Size; 128
    layout (offset = 768) vec3 pointLightWorldPositions[MaximumNumberOfPointLights]; //Offset; 768 - Size; 128

    //Spot light data.
    layout (offset = 896) int numberOfSpotLights; //Offset; 896 - Size; 16
    layout (offset = 912) float spotLightAttenuationDistances[MaximumNumberOfSpotLights]; //Offset; 912 - Size; 128
    layout (offset = 1040) float spotLightIntensities[MaximumNumberOfSpotLights]; //Offset; 1040 - Size; 128
    layout (offset = 1168) float spotLightInnerCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1168 - Size; 128
    layout (offset = 1296) float spotLightOuterCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1296 - Size; 128
    layout (offset = 1424) vec3 spotLightColors[MaximumNumberOfSpotLights]; //Offset; 1424 - Size; 128
    layout (offset = 1552) vec3 spotLightDirections[MaximumNumberOfSpotLights]; //Offset; 1552 - Size; 128
    layout (offset = 1680) vec3 spotLightWorldPositions[MaximumNumberOfSpotLights]; //Offset; 1680 - Size; 128

    //Physics data.
    layout (offset = 1808) float windStrength; //Offset; 1808 - Size; 16
    layout (offset = 1824) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1840
};

//Preprocessor defines.
#define DIRECTIONAL_LIGHT_SCREEN_SHADE_SHADOWS_SAMPLES 250
#define PI 3.141592f

layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 1) uniform samplerCube diffuseIrradianceTexture;
layout (set = 1, binding = 2) uniform samplerCube specularIrradianceTexture;
layout (set = 1, binding = 3) uniform sampler2D albedoTexture;
layout (set = 1, binding = 4) uniform sampler2D normalDirectionDepthTexture;
layout (set = 1, binding = 5) uniform sampler2D roughnessMetallicAmbientOcclusionTexture;

//Out parameters.
layout (location = 0) out vec4 fragmentColor;
layout (location = 1) out vec4 waterFragmentColor;

//Globals.
float ambientOcclusion;
float fragmentDepth;
float metallic;
float roughness;
float thinness;
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
    vec3 specularComponent = CalculateFresnelRoughness(viewAngle);
    vec3 diffuseComponent = 1.0f - specularComponent;
    diffuseComponent *= 1.0f - metallic;

    vec3 irradiance = texture(diffuseIrradianceTexture, normalDirection).rgb;
    vec3 diffuse = irradiance * albedoColor;

    vec3 reclectionDirection = reflect(-viewDirection, normalDirection);
    vec3 specularIrradiance = texture(specularIrradianceTexture, normalDirection).rgb;
    vec3 specular = mix(specularIrradiance, irradiance, roughness);

    return (diffuse * diffuseComponent + specular * specularComponent) * ambientOcclusion;
}

/*
*   Calculates a light.
*/
vec3 CalculateLight(vec3 lightDirection, vec3 radiance)
{
	radiance = mix(radiance, albedoColor, thinness);

    vec3 halfwayDirection = normalize(viewDirection + lightDirection);
    float lightViewAngle = clamp(dot(halfwayDirection, viewDirection), 0.0f, 1.0f);
    float lightAngle = mix(max(dot(normalDirection, lightDirection), 0.0f), 1.0f, thinness);

    float distribution = CalculateDistribution(halfwayDirection);
    float geometry = CalculateGeometry(lightAngle);
    vec3 fresnel = CalculateFresnel(lightViewAngle);

    vec3 diffuseComponent = vec3(1.0f) - fresnel;
    diffuseComponent *= 1.0f - metallic;

    vec3 nominator = distribution * geometry * fresnel;
    float denominator = 4 * viewAngle * lightAngle + 0.001f;
    vec3 specularComponent = nominator / denominator;

    //Return the combined components.
    return (diffuseComponent * albedoColor / PI + specularComponent) * radiance * lightAngle;
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
    vec3 radiance = mix(directionalLightColor, albedoColor, thinness) * directionalLightIntensity;

    return CalculateLight(lightDirection, radiance);
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

    vec3 radiance = mix(pointLightColors[index], albedoColor, thinness) * pointLightIntensities[index] * attenuation;

    return CalculateLight(lightDirection, radiance);
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

    //Calculate the inner/outer cone fade out.
    float epsilon = spotLightInnerCutoffAngles[index] - spotLightOuterCutoffAngles[index];
    float intensity = angle > spotLightInnerCutoffAngles[index] ? 1.0f : clamp((angle - spotLightOuterCutoffAngles[index]) / epsilon, 0.0f, 1.0f); 

    vec3 radiance = mix(spotLightColors[index], albedoColor, thinness) * spotLightIntensities[index] * intensity * attenuation;

    return CalculateLight(lightDirection, radiance);
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

    //Set the thinness.
    thinness = roughnessMetallicAmbientOcclusionSampler.a;

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
    waterFragmentColor = vec4(finalFragment, 1.0f);
}