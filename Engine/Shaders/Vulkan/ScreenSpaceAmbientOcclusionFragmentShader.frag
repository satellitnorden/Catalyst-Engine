//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Preprocessor defines.
#define SCREEN_SPACE_AMBIENT_OCCLUSION_BIAS (0.000000825f)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_RADIUS (1.0f)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_RADIUS_SQUARED (SCREEN_SPACE_AMBIENT_OCCLUSION_RADIUS * SCREEN_SPACE_AMBIENT_OCCLUSION_RADIUS)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES (4)
#define SCREEN_SPACE_AMBIENT_OCCLUSION_STRENGTH (16.0f)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D normalDepthTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

/*
*	Rotates a vector.
*/
vec3 RotateVector(vec3 original, vec3 rotationVector)
{
	float xCosine = cos(rotationVector.x);
	float xSine = sin(rotationVector.x);

	float yCosine = cos(rotationVector.y);
	float ySine = sin(rotationVector.y);

	float zCosine = cos(rotationVector.z);
	float zSine = sin(rotationVector.z);

	//Rotate the roll.
	float tempY = original.y * xCosine - original.z * xSine;
	original.z = original.y * xSine + original.z * xCosine;
	original.y = tempY;

	//Rotate the pitch
	float tempX1 = original.x * yCosine + original.z * ySine;
	original.z = -original.x * ySine + original.z * yCosine;
	original.x = tempX1;

	//Rotate the yaw.
	float tempX2 = original.x * zCosine - original.y * zSine;
	original.y = original.x * zSine + original.y * zCosine;
	original.x = tempX2;

	return original;
}

void main()
{
    //Sample the normal depth texture.
    vec4 normalDepthTextureSampler = texture(normalDepthTexture, fragmentTextureCoordinate);

    //Retrieve the normal.
    vec3 normal = normalDepthTextureSampler.xyz;

    //Retrieve the depth.
    float depth = normalDepthTextureSampler.w;

    //Retrieve the fragment world position.
    vec3 fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, depth);

    //Calculate the occlusion.
    float occlusion = 0.0f;

    for (int i = 1; i <= SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES; ++i)
    {
    	vec3 randomRotation = vec3((RandomFloat(vec3(gl_FragCoord + i * PI + depth)) * 2.0f - 1.0f) * (HALF_PI * 0.75f), (RandomFloat(vec3(gl_FragCoord + i * DOUBLE_PI + depth)) * 2.0f - 1.0f) * (HALF_PI * 0.75f), (RandomFloat(vec3(gl_FragCoord + i * PI + depth)) * 2.0f - 1.0f) * (HALF_PI * 0.75f));
        vec3 currentSamplePosition = fragmentWorldPosition + RotateVector(normal, randomRotation) * SCREEN_SPACE_AMBIENT_OCCLUSION_RADIUS * RandomFloat(vec3(gl_FragCoord + i * INVERSE_PI + depth));

        vec4 offset = vec4(currentSamplePosition, 1.0f);
        offset = viewMatrix * offset;
        offset.xyz /= offset.w;
        offset.xy = offset.xy * 0.5f + 0.5f;

        float currentSampleDepth = texture(normalDepthTexture, offset.xy).w;
        vec3 currentSampleActualPosition = CalculateFragmentWorldPosition(offset.xy, currentSampleDepth);

        float fade = mix(1.0f, 0.0f, SmoothStep(min(LengthSquared3(currentSamplePosition - currentSampleActualPosition) / SCREEN_SPACE_AMBIENT_OCCLUSION_RADIUS_SQUARED, 1.0f)));

        occlusion += (offset.z < currentSampleDepth - SCREEN_SPACE_AMBIENT_OCCLUSION_BIAS ? 1.0f : 0.0f) * fade;    
    }

    occlusion = 1.0f - (occlusion / SCREEN_SPACE_AMBIENT_OCCLUSION_SAMPLES);

    //Modify the value a bit.
    occlusion = pow(occlusion, SCREEN_SPACE_AMBIENT_OCCLUSION_STRENGTH);

    //Write the fragment.
    fragment = vec4(occlusion);
}