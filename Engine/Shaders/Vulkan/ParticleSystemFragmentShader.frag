//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//In parameters.
layout (location = 0) in vec3 fragmentNormal;
layout (location = 1) in vec2 fragmentTextureCoordinate;
layout (location = 2) in float fragmentFadeFactor;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D albedoTexture;

//Out parameters.
layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normalDepth;
layout (location = 2) out vec4 materialProperties;

void main()
{
    //Sample the albedo.
    vec4 albedoTextureSampler = texture(albedoTexture, fragmentTextureCoordinate);

    //Calculate the blend factor.
    float blendFactor = albedoTextureSampler.a * fragmentFadeFactor;

    //Discard this fragment according to blend factor.
    if (blendFactor <= RandomFloat(gl_FragCoord.xyz))
    {
        discard;
    }

    else
    {
    	//Write the albedo.
    	albedo = albedoTextureSampler;

    	//Write the normal.
	    normalDepth = vec4(fragmentNormal, gl_FragCoord.z);

	    //Write the roughness.
	    materialProperties.r = 1.0f;

	    //Write the metallic.
	    materialProperties.g = 0.0f;

	    //Write the ambient occlusion.
	    materialProperties.b = 1.0f;

	    //Write the thickness.
	    materialProperties.a = 0.0f;
    }
}