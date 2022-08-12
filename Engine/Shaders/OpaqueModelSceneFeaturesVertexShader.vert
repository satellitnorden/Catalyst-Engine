//Push constant data.
layout (push_constant) uniform ModelData
{
    layout (offset = 0) mat4 previousModelMatrix;
    layout (offset = 64) mat4 currentModelMatrix;
};

//In parameters.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexTextureCoordinate;

//Out parameters.
layout (location = 0) out mat3 fragmentTangentSpaceMatrix;
layout (location = 3) out vec3 fragmentPreviousWorldPosition;
layout (location = 4) out vec3 fragmentCurrentWorldPosition;
layout (location = 5) out vec2 fragmentTextureCoordinate;

void CatalystShaderMain()
{
	vec3 tangent = normalize(vec3(currentModelMatrix * vec4(vertexTangent, 0.0f)));
	vec3 bitangent = normalize(vec3(currentModelMatrix * vec4(cross(vertexNormal, vertexTangent), 0.0f)));
	vec3 normal = normalize(vec3(currentModelMatrix * vec4(vertexNormal, 0.0f)));

	fragmentTangentSpaceMatrix = mat3(tangent, bitangent, normal);
	fragmentPreviousWorldPosition = vec3(previousModelMatrix * vec4(vertexPosition, 1.0f));
	fragmentCurrentWorldPosition = vec3(currentModelMatrix * vec4(vertexPosition, 1.0f));
	fragmentTextureCoordinate = vertexTextureCoordinate;

	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(fragmentCurrentWorldPosition, 1.0f);
}