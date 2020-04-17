//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec2 minimum;
    layout (offset = 8) vec2 maximum;
};

//Out parameters.
layout (location = 0) out vec2 fragment_texture_coordinate;

void main()
{
	//Calculate X and Y components.
	float x = float(gl_VertexIndex == 1 || gl_VertexIndex == 2);
    float y = float(gl_VertexIndex == 2 || gl_VertexIndex == 3);

    //Calculate the texture coordinate.
    fragment_texture_coordinate.x = x;
    fragment_texture_coordinate.y = 1.0f - y;
    
    //Calculate the viewport coordinates.
    vec2 viewport_coordinates = vec2(mix(minimum.x,  maximum.x, x), 1.0f - mix(minimum.y, maximum.y, y));

    //Write the position.
    gl_Position = vec4(viewport_coordinates.x * 2.0f - 1.0f, viewport_coordinates.y * 2.0f - 1.0f, 0.0f, 1.0f);
} 