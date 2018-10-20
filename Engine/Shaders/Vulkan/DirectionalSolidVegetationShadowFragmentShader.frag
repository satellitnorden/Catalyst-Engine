//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in float fragmentLengthFactor;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Discard this fragment according to the length factor.
    if (fragmentLengthFactor == 0.0f
        || fragmentLengthFactor < RandomFloat(gl_FragCoord.x * gl_FragCoord.y * gl_FragCoord.z))
    {
        discard;
    }

    else
    {
        //Write the depth.
        fragment = vec4(gl_FragCoord.z, 0.0f, 0.0f, 0.0f);
    }
}