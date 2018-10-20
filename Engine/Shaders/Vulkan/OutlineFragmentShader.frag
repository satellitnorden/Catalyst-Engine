//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform OutlineData
{
    layout (offset = 64) vec3 color;
};

//In parameters.
layout (location = 0) in vec3 fragmentWorldPosition;
layout (location = 1) in mat3 fragmentTangentSpaceMatrix;
layout (location = 4) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Calculate the outline weight.
    float weight = sin(totalGameTime * 5.0f) * 0.5f + 0.5f;
    weight *= weight;

    //Write the fragment.
    fragment = vec4(color, weight);
}