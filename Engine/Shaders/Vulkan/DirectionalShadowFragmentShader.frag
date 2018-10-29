//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define ITERATIONS (4)
#define INFLUENCE_PER_ITERATION (1.0f / ITERATIONS)
#define MAXIMUM_OFFSET (0.001f)
#define SHADOW_BIAS (0.004f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D normalDirectionDepthTexture;
layout (set = 1, binding = 1) uniform sampler2D directionalShadowMap;

//Out parameters.
layout (location = 0) out vec4 directionalShadow;

void main()
{
    //Sample values from the textures.
    vec4 normalDirectionDepthSampler = texture(normalDirectionDepthTexture, fragmentTextureCoordinate);

    //Set the fragment depth.
    float fragmentDepth = normalDirectionDepthSampler.a;

    //Calculate the world position of this fragment.
    vec3 fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, fragmentDepth);

    //Calculate the directional light screen space position.
    vec4 directionalLightShadowMapCoordinate = directionalLightViewMatrix * vec4(fragmentWorldPosition, 1.0f);
    directionalLightShadowMapCoordinate.xy = directionalLightShadowMapCoordinate.xy * 0.5f + 0.5f;

    //Calculate the shadow multiplier.
    float shadowMultiplier = 0.0f;

    for (int i = 0; i < ITERATIONS; ++i)
    {
        float offsetX = mix(-MAXIMUM_OFFSET, MAXIMUM_OFFSET, RandomFloat(vec3(gl_FragCoord.xy, i * PI)));
        float offsetY = mix(-MAXIMUM_OFFSET, MAXIMUM_OFFSET, RandomFloat(vec3(gl_FragCoord.xy, i * PHI)));

    	vec2 offset = vec2(offsetX, offsetY);

	    float directionalDepth = texture(directionalShadowMap, directionalLightShadowMapCoordinate.xy + offset).r;
	    float compare = directionalLightShadowMapCoordinate.z - SHADOW_BIAS;

	    shadowMultiplier += (compare >= 1.0f || compare < directionalDepth ? 1.0f : 0.0f) * INFLUENCE_PER_ITERATION;
    }

    //Set the final fragment color.
    directionalShadow = vec4(shadowMultiplier, 0.0f, 0.0f, 0.0f);
}