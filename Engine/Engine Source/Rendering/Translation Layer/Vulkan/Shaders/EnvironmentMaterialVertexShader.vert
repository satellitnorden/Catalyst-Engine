//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//The box vertices.
vec3 boxVertices[36] = vec3[]
(
	vec3(-1.0f, 1.0f, -1.0f),
	vec3(-1.0f, -1.0f, -1.0f),
	vec3(1.0f, -1.0f, -1.0f),
	vec3(1.0f, -1.0f, -1.0f),
	vec3(1.0f, 1.0f, -1.0f),
	vec3(-1.0f, 1.0f, -1.0f),

	vec3(-1.0f, -1.0f, 1.0f),
	vec3(-1.0f, -1.0f, -1.0f),
	vec3(-1.0f, 1.0f, -1.0f),
	vec3(-1.0f, 1.0f, -1.0f),
	vec3(-1.0f, 1.0f, 1.0f),
	vec3(-1.0f, -1.0f, 1.0f),

	vec3(1.0f, -1.0f, -1.0f),
	vec3(1.0f, -1.0f, 1.0f),
	vec3(1.0f, 1.0f, 1.0f),
	vec3(1.0f, 1.0f, 1.0f),
	vec3(1.0f, 1.0f, -1.0f),
	vec3(1.0f, -1.0f, -1.0f),

	vec3(-1.0f, -1.0f, 1.0f),
	vec3(-1.0f, 1.0f, 1.0f),
	vec3(1.0f, 1.0f, 1.0f),
	vec3(1.0f, 1.0f, 1.0f),
	vec3(1.0f, -1.0f, 1.0f),
	vec3(-1.0f, -1.0f, 1.0f),

	vec3(-1.0f, 1.0f, -1.0f),
	vec3(1.0f, 1.0f, -1.0f),
	vec3(1.0f, 1.0f, 1.0f),
	vec3(1.0f, 1.0f, 1.0f),
	vec3(-1.0f, 1.0f, 1.0f),
	vec3(-1.0f, 1.0f, -1.0f),

	vec3(-1.0f, -1.0f, -1.0f),
	vec3(-1.0f, -1.0f, 1.0f),
	vec3(1.0f, -1.0f, -1.0f),
	vec3(1.0f, -1.0f, -1.0f),
	vec3(-1.0f, -1.0f, 1.0f),
	vec3(1.0f, -1.0f, 1.0f)
);

layout (push_constant) uniform ViewData
{
    mat4 viewMatrix;
};

//Out parameters.
layout (location = 0) out vec3 fragmentPosition;

void main()
{
	vec3 vertexPosition = boxVertices[gl_VertexIndex];
    fragmentPosition = vertexPosition;  
    gl_Position = viewMatrix * vec4(vertexPosition, 1.0f);
}