//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystTemporalAccumulationCore.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D accumulationTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	//Sample the accumulation texture.
	vec4 accumulationTextureSampler = texture(accumulationTexture, fragmentTextureCoordinate);

	//Unpack the number of accumulation description.
	AccumulationDescription accumulationDescription = UnpackAccumulationDescription(floatBitsToUint(accumulationTextureSampler.w));

    //Write the fragment.
    fragment = vec4(accumulationTextureSampler.rgb / accumulationDescription.accumulations, 1.0f);
}