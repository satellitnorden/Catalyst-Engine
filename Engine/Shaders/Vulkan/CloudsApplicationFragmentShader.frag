//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D cloud_texture;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
    //Write the fragment.
    fragment = Upsample(cloud_texture, fragment_texture_coordinate);
}