//Constants.
#define MOBILE_PASS_MODEL (0)
#define MOBILE_PASS_SKY (1)

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) mat4 MODEL_MATRIX;
    layout (offset = 64) vec3 SKY_LIGHT_LUMINANCE;
    layout (offset = 80) uint MATERIAL_INDEX;
    layout (offset = 84) uint MOBILE_PASS;
};

//In parameters.
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texture_coordinate;

//Out parameters.
layout (location = 0) out mat3 fragment_tangent_space_matrix;
layout (location = 3) out vec3 fragment_world_position;
layout (location = 4) out vec2 fragment_texture_coordinate;

void CatalystShaderMain()
{
	switch (MOBILE_PASS)
	{
		case MOBILE_PASS_MODEL:
		{
			//Calculate the tangent space matrix.
			vec3 tangent = vec3(MODEL_MATRIX * vec4(vertex_tangent, 0.0f));
			vec3 bitangent = vec3(MODEL_MATRIX * vec4(cross(vertex_normal, vertex_tangent), 0.0f));
			vec3 normal = vec3(MODEL_MATRIX * vec4(vertex_normal, 0.0f));

			//Pass data to the fragment shader.
			fragment_tangent_space_matrix = mat3(tangent, bitangent, normal);
			fragment_world_position = vec3(MODEL_MATRIX * vec4(vertex_position, 1.0f));
			fragment_texture_coordinate = vertex_texture_coordinate;

			//Write the position.
			gl_Position = WORLD_TO_CLIP_MATRIX * vec4(fragment_world_position, 1.0f);

			break;
		}

		case MOBILE_PASS_SKY:
		{
			//Set the fragment tangent space matrix.
			fragment_tangent_space_matrix = mat3(vec3(0.0f), vec3(0.0f), vec3(0.0f));

			//Calculate the viewport coordinates.
			float x = -1.0f + float((gl_VertexIndex & 2) << 1);
		    float y = -1.0f + float((gl_VertexIndex & 1) << 2);

		    //Calculate the texture coordinate.
		    fragment_texture_coordinate.x = (x + 1.0f) * 0.5f;
		    fragment_texture_coordinate.y = (y + 1.0f) * 0.5f;
		    
		    //Calculate the world position.
		    fragment_world_position = CalculateWorldPosition(vec2(x, y), FLOAT32_EPSILON);

		    //Set the position.
		    gl_Position = vec4(x, y, FLOAT32_EPSILON, 1.0f);

			break;
		}
	}
}