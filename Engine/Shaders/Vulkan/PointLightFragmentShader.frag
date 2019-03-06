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

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D albedoTexture;
layout (set = 1, binding = 1) uniform sampler2D normalDepthTexture;
layout (set = 1, binding = 2) uniform sampler2D materialPropertiesTexture;
layout (set = 1, binding = 3) uniform sampler2D screenSpaceAmbientOcclusionTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Calculate the coordinates.
    vec2 coordinates = gl_FragCoord.xy / vec2(1920.0f, 1080.0f);

	//Sample the albedo.
	vec3 albedo = texture(albedoTexture, coordinates).rgb;

	//Sample the normal/depth.
	vec4 normalDepthTextureSampler = texture(normalDepthTexture, coordinates);
	vec3 normal = normalDepthTextureSampler.xyz;
	float depth = normalDepthTextureSampler.w;

	//Sample the material properties.
	vec4 materialProperties = texture(materialPropertiesTexture, coordinates);

    //Sample the screen space ambient occlusion.
    float screenSpaceAmbientOcclusion = texture(screenSpaceAmbientOcclusionTexture, coordinates).x;

	//Calculate the fragment world position.
	vec3 fragmentWorldPosition = CalculateFragmentWorldPosition(coordinates, depth);

	//Calculate the view direction.
    vec3 viewDirection = normalize(perceiverWorldPosition - fragmentWorldPosition);

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
    										materialProperties[3]) * screenSpaceAmbientOcclusion, 1.0f);
}