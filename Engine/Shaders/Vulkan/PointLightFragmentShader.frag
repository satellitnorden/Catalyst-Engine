//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 color;
    layout (offset = 16) vec3 worldPosition;
    layout (offset = 32) float attenuationDistance;
    layout (offset = 36) float intensity;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D albedoTexture;
layout (set = 1, binding = 1) uniform sampler2D normalDepthTexture;
layout (set = 1, binding = 2) uniform sampler2D materialPropertiesTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Sample the albedo.
	vec3 albedo = texture(albedoTexture, fragmentTextureCoordinate).rgb;

	//Sample the normal/depth.
	vec4 normalDepthTextureSampler = texture(normalDepthTexture, fragmentTextureCoordinate);
	vec3 normal = normalDepthTextureSampler.xyz;
	float depth = normalDepthTextureSampler.w;

	//Sample the material properties.
	vec4 materialProperties = texture(materialPropertiesTexture, fragmentTextureCoordinate);

	//Calculate the fragment world position.
	vec3 fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, depth);

	//Calculate the view direction.
    vec3 viewDirection = normalize(cameraWorldPosition - fragmentWorldPosition);

    //Calculate the view angle.
    float viewAngle = max(dot(normal, viewDirection), 0.0f);

    //Write the fragment.
    fragment = vec4(CalculatePointLight(	worldPosition,
    										attenuationDistance,
    										color,
    										intensity,
    										fragmentWorldPosition,
    										viewAngle,
    										viewDirection,
    										albedo,
    										normal,
    										materialProperties[0],
    										materialProperties[1],
    										materialProperties[2],
    										materialProperties[3]), 1.0f);
}