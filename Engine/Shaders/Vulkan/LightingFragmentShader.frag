//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Post processing data.
layout (push_constant) uniform LightingData
{
    bool screenSpaceAmbientOcclusionEnabled;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform samplerCube nightDiffuseTexture;
layout (set = 1, binding = 1) uniform samplerCube nightDiffuseIrradianceTexture;
layout (set = 2, binding = 0) uniform samplerCube dayDiffuseTexture;
layout (set = 2, binding = 1) uniform samplerCube dayDiffuseIrradianceTexture;
layout (set = 3, binding = 0) uniform sampler2D albedoTexture;
layout (set = 3, binding = 1) uniform sampler2D normalDirectionDepthTexture;
layout (set = 3, binding = 2) uniform sampler2D roughnessMetallicAmbientOcclusionTexture;
layout (set = 3, binding = 3) uniform sampler2D directionalShadow;

//Out parameters.
layout (location = 0) out vec4 fragmentColor;

void main()
{
    //Sample values from the textures.
    vec3 albedoColor = pow(texture(albedoTexture, fragmentTextureCoordinate).rgb, vec3(2.2f));
    vec4 normalDirectionDepthSampler = texture(normalDirectionDepthTexture, fragmentTextureCoordinate);
    vec4 roughnessMetallicAmbientOcclusionSampler = texture(roughnessMetallicAmbientOcclusionTexture, fragmentTextureCoordinate);

    //Set the normal direction.
    vec3 normalDirection = normalDirectionDepthSampler.xyz;

    //Set the fragment depth.
    float fragmentDepth = normalDirectionDepthSampler.a;

    //Calculate the world position of this fragment.
    vec3 fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, fragmentDepth);

    //Set the roughness.
    float roughness = roughnessMetallicAmbientOcclusionSampler.r;

    //Set the metallic.
    float metallic = roughnessMetallicAmbientOcclusionSampler.g;

    //Set the ambient occlusion.
    float ambientOcclusion = roughnessMetallicAmbientOcclusionSampler.b;

    //Set the thickness.
    float thickness = roughnessMetallicAmbientOcclusionSampler.a;

    //Calculate globals.
    vec3 viewDirection = normalize(cameraWorldPosition - fragmentWorldPosition);
    float viewAngle = max(dot(normalDirection, viewDirection), 0.0f);
    vec3 surfaceColor = mix(vec3(0.04f), albedoColor, metallic);

    //Start off with just the ambient lighting.
    vec3 finalFragment = CalculateAmbient(  surfaceColor,
                                            roughness,
                                            viewAngle,
                                            metallic,
                                            nightDiffuseIrradianceTexture,
                                            dayDiffuseIrradianceTexture,
                                            normalDirection,
                                            albedoColor,
                                            viewDirection,
                                            nightDiffuseTexture,
                                            dayDiffuseTexture,
                                            ambientOcclusion);

    //Calculate the directional light.
    finalFragment += CalculateDirectionalLight( albedoColor,
                                                thickness,
                                                viewDirection,
                                                normalDirection,
                                                roughness,
                                                viewAngle,
                                                surfaceColor,
                                                metallic) * texture(directionalShadow, fragmentTextureCoordinate).r;

    //Calculate all point lights.
    for (int i = 0; i < numberOfPointLights; ++i)
    {
        finalFragment += CalculatePointLight(   i,
                                                fragmentWorldPosition, 
                                                viewAngle,
                                                viewDirection,
                                                albedoColor,
                                                normalDirection,
                                                roughness,
                                                metallic,
                                                ambientOcclusion,
                                                thickness);
    }

    //Calculate all spot lights.
    for (int i = 0; i < numberOfSpotLights; ++i)
    {
        finalFragment += CalculateSpotLight(    i,
                                                fragmentWorldPosition, 
                                                viewAngle,
                                                viewDirection,
                                                albedoColor,
                                                normalDirection,
                                                roughness,
                                                metallic,
                                                ambientOcclusion,
                                                thickness);
    }

    //Set the final fragment color.
    fragmentColor = vec4(finalFragment, 1.0f);
}