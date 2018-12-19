//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShadowUtilities.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec3 fragmentNormal;
layout (location = 1) in float fragmentLengthFactor;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Discard this fragment according to the length factor.
    if (fragmentLengthFactor == 0.0f
        || fragmentLengthFactor < RandomFloat(gl_FragCoord.xyz))
    {
        discard;
    }

    else
    {
        //Write the depth.
        fragment = vec4(gl_FragCoord.z + CalculateDirectionalShadowBias(fragmentNormal), 0.0f, 0.0f, 0.0f);
    }
}