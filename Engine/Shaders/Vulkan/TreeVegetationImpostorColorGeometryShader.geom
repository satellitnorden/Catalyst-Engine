//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) float impostorHalfWidth;
    layout (offset = 4) float impostorHeight;
};

//Layout specification.
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//In parameters.
layout (location = 0) in mat4 geometryTransformationMatrix[];

//Out parameters.
layout (location = 0) out vec3 fragmentNormal;
layout (location = 1) out vec2 fragmentTextureCoordinate;

void main()
{
	//Retrieve the world position.
	vec3 worldPosition = geometryTransformationMatrix[0][3].xyz;

	//Calculate the forward and right vector.
	vec3 forwardVector = normalize((cameraWorldPosition - vec3(0.0f, cameraWorldPosition.y, 0.0f)) - (worldPosition - vec3(0.0f, worldPosition.y, 0.0f)));
	vec3 rightVector = normalize(cross(forwardVector, vec3(0.0f, 1.0f, 0.0f)));
	vec3 upVector = vec3(0.0f, 1.0f, 0.0f);

	//Construct all the vertices.
	fragmentNormal = forwardVector;
	fragmentTextureCoordinate = vec2(0.0f, 1.0f);
	gl_Position = viewMatrix * vec4(worldPosition - rightVector * impostorHalfWidth, 1.0f);

	EmitVertex();

	fragmentNormal = forwardVector;
	fragmentTextureCoordinate = vec2(0.0f, 0.0f);
	gl_Position = viewMatrix * vec4(worldPosition - rightVector * impostorHalfWidth + upVector * impostorHeight, 1.0f);

	EmitVertex();

	fragmentNormal = forwardVector;
	fragmentTextureCoordinate = vec2(1.0f, 1.0f);
	gl_Position = viewMatrix * vec4(worldPosition + rightVector * impostorHalfWidth, 1.0f);

	EmitVertex();

	fragmentNormal = forwardVector;
	fragmentTextureCoordinate = vec2(1.0f, 0.0f);
	gl_Position = viewMatrix * vec4(worldPosition + rightVector * impostorHalfWidth + upVector * impostorHeight, 1.0f);

	EmitVertex();

	EndPrimitive();
}