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

void main()
{
	//Sample the depth.
	float depth = texture(depthTexture, fragmentTextureCoordinate).r;

	//Calculate the texture coordinate.
	vec3 fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, depth);
	vec3 cubeTextureCoordinate = normalize(fragmentWorldPosition - perceiverWorldPosition);

    //Sample the cube map texture.
    vec3 cubeMapTextureSampler = mix(texture(nightTexture, cubeTextureCoordinate).rgb, texture(dayTexture, cubeTextureCoordinate).rgb, environmentBlend);

    //Calculate the sun weight.
    float sunWeight = pow(max(dot(normalize(cubeTextureCoordinate), -directionalLightDirection), 0.0f), 4096.0f) * directionalLightIntensity;

    //Calculate the sun color.
    vec3 sunColor = directionalLightColor * directionalLightIntensity;

    //Calculate the final sky color.
    vec3 skyColor = mix(cubeMapTextureSampler, sunColor, sunWeight);

    //Set the fragment color.
    fragmentColor = vec4(skyColor, 1.0f);
}