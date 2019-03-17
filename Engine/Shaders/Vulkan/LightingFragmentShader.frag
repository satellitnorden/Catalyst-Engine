//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//Layout specification.
layout (early_fragment_tests) in;

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
layout (set = 3, binding = 4) uniform sampler2D screenSpaceAmbientOcclusionTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

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

    //Sample the screen space ambient occlusion.
    float screenSpaceAmbientOcclusion = texture(screenSpaceAmbientOcclusionTexture, fragmentTextureCoordinate).x;

    //Write the fragment.
    fragment = vec4(CalculateLighting(  mix(texture(nightDiffuseIrradianceTexture, normalDirection).rgb, texture(dayDiffuseIrradianceTexture, normalDirection).rgb, environmentBlend),
                                        mix(texture(nightDiffuseTexture, normalDirection).rgb, texture(dayDiffuseTexture, normalDirection).rgb, environmentBlend),
                                        texture(directionalShadow, fragmentTextureCoordinate).r,
                                        fragmentWorldPosition, 
                                        albedoColor,
                                        normalDirection,
                                        roughness,
                                        metallic,
                                        ambientOcclusion * screenSpaceAmbientOcclusion,
                                        thickness),
                                        1.0f);

    /*
    //Write the fragment.
    if (gl_FragCoord.x < 980.0f)
    {
    	fragment = vec4(CalculateLighting(  mix(texture(nightDiffuseIrradianceTexture, normalDirection).rgb, texture(dayDiffuseIrradianceTexture, normalDirection).rgb, environmentBlend),
                                        mix(texture(nightDiffuseTexture, normalDirection).rgb, texture(dayDiffuseTexture, normalDirection).rgb, environmentBlend),
                                        texture(directionalShadow, fragmentTextureCoordinate).r,
                                        fragmentWorldPosition, 
                                        albedoColor,
                                        normalDirection,
                                        roughness,
                                        metallic,
                                        ambientOcclusion * screenSpaceAmbientOcclusion,
                                        thickness),
                                        1.0f);
    }

    else
    {
    	fragment = vec4(vec3(screenSpaceAmbientOcclusion), 1.0f);
    }
    */
}