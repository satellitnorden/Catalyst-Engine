//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D depthTexture;
layout (set = 2, binding = 0) uniform samplerCube nightTexture;
layout (set = 3, binding = 0) uniform samplerCube dayTexture;

//Out parameters.
layout (location = 0) out vec4 fragmentColor;

/*
*   Calculates the fragment world position.
*/
vec3 CalculateFragmentWorldPosition(vec2 textureCoordinate, float depth)
{
    vec2 nearPlaneCoordinate = textureCoordinate * 2.0f - 1.0f;
    vec3 fragmentScreenSpacePosition = vec3(nearPlaneCoordinate, depth);
    vec4 viewSpacePosition = inverseProjectionMatrix * vec4(fragmentScreenSpacePosition, 1.0f);
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseCameraMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

void main()
{
	//Sample the depth.
	float depth = texture(depthTexture, fragmentTextureCoordinate).r;

	//Calculate the texture coordinate.
	vec3 fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, depth);
	vec3 cubeTextureCoordinate = normalize(fragmentWorldPosition - cameraWorldPosition);

    //Sample the cube map texture.
    vec3 cubeMapTextureSampler = mix(texture(nightTexture, cubeTextureCoordinate).rgb, texture(dayTexture, cubeTextureCoordinate).rgb, environmentBlend);

    //Calculate the sun weight.
    float sunDirection = dot(normalize(cubeTextureCoordinate), -directionalLightDirection);
    float sunWeight = sunDirection < 0.9985f ? 0.0f : sunDirection > 0.9995f ? 1.0f : (sunDirection - 0.9985f) * 1000.0f;
    sunWeight *= min(directionalLightIntensity, 1.0f);
    sunWeight *= sunWeight;

    //Calculate the sun color.
    vec3 sunColor = directionalLightColor * directionalLightIntensity;

    //Calculate the final sky color.
    vec3 skyColor = mix(cubeMapTextureSampler, sunColor, sunWeight);

    //Set the fragment color.
    fragmentColor = vec4(skyColor, 1.0f);
}