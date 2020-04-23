//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 POSITION;
    layout (offset = 16) float RADIUS;
    layout (offset = 20) uint NUMBER_OF_SEGMENTS;
};

//Out parameters.
layout (location = 0) out vec3 fragment_normal;

void main()
{
	//Calculate the stack index.
	uint stack_index = gl_VertexIndex / NUMBER_OF_SEGMENTS;

	//Calculate the stack angle.
	float stack_angle = PI / 2.0f - stack_index * (PI / NUMBER_OF_SEGMENTS);

	//Calculate the sector index.
	uint sector_index = gl_VertexIndex & (NUMBER_OF_SEGMENTS - 1);
    
	//Calculate the sector angle.
	float sector_angle = sector_index * (2.0f * PI / NUMBER_OF_SEGMENTS);

	//Calculate the unit position.
	vec3 unit_position;

	unit_position.x = (RADIUS * cos(stack_angle)) * cos(sector_angle);
	unit_position.y = (RADIUS * cos(stack_angle)) * sin(sector_angle);
	unit_position.z = RADIUS * sin(stack_angle);

	//Calculate the normal.
	fragment_normal = normalize(unit_position);

    //Write the position.
    gl_Position = viewMatrix * vec4(POSITION + unit_position, 1.0f);
} 