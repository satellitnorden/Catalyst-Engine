//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 MINIMUM;
    layout (offset = 16) vec3 MAXIMUM;
};

//Out parameters.
layout (location = 0) out vec3 fragment_normal;

/*
*	Returns the vertex position and normal for the given index.
*/
void CalculateVertexPositionAndNormal(uint index, out vec3 vertex_position, out vec3 vertex_normal)
{
	switch (index)
	{
		//Back, first triangle.
		case 0:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(0.0f, 0.0f, -1.0f);

			break;
		}

		case 1:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(0.0f, 0.0f, -1.0f);

			break;
		}

		case 2:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(0.0f, 0.0f, -1.0f);

			break;
		}

		//Back, second triangle.
		case 3:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(0.0f, 0.0f, -1.0f);

			break;
		}

		case 4:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(0.0f, 0.0f, -1.0f);

			break;
		}

		case 5:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(0.0f, 0.0f, -1.0f);

			break;
		}

		//Front, first triangle.
		case 6:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(0.0f, 0.0f, 1.0f);

			break;
		}

		case 7:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(0.0f, 0.0f, 1.0f);

			break;
		}

		case 8:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(0.0f, 0.0f, 1.0f);

			break;
		}

		//Front, second triangle.
		case 9:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(0.0f, 0.0f, 1.0f);

			break;
		}

		case 10:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(0.0f, 0.0f, 1.0f);

			break;
		}

		case 11:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(0.0f, 0.0f, 1.0f);

			break;
		}

		//Left, first triangle.
		case 12:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(-1.0f, 0.0f, 0.0f);

			break;
		}

		case 13:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(-1.0f, 0.0f, 0.0f);

			break;
		}

		case 14:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(-1.0f, 0.0f, 0.0f);

			break;
		}

		//Left, second triangle.
		case 15:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(-1.0f, 0.0f, 0.0f);

			break;
		}

		case 16:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(-1.0f, 0.0f, 0.0f);

			break;
		}

		case 17:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(-1.0f, 0.0f, 0.0f);

			break;
		}

		//Right, first triangle.
		case 18:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(1.0f, 0.0f, 0.0f);

			break;
		}

		case 19:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(1.0f, 0.0f, 0.0f);

			break;
		}

		case 20:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(1.0f, 0.0f, 0.0f);

			break;
		}

		//Right, second triangle.
		case 21:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(1.0f, 0.0f, 0.0f);

			break;
		}

		case 22:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(1.0f, 0.0f, 0.0f);

			break;
		}

		case 23:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(1.0f, 0.0f, 0.0f);

			break;
		}

		//Bottom, first triangle.
		case 24:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(0.0f, -1.0f, 0.0f);

			break;
		}

		case 25:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(0.0f, -1.0f, 0.0f);

			break;
		}

		case 26:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(0.0f, -1.0f, 0.0f);

			break;
		}

		//Bottom, second triangle.
		case 27:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(0.0f, -1.0f, 0.0f);

			break;
		}

		case 28:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(0.0f, -1.0f, 0.0f);

			break;
		}

		case 29:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MINIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(0.0f, -1.0f, 0.0f);

			break;
		}

		//Top, first triangle.
		case 30:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(0.0f, 1.0f, 0.0f);

			break;
		}

		case 31:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(0.0f, 1.0f, 0.0f);

			break;
		}

		case 32:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(0.0f, 1.0f, 0.0f);

			break;
		}

		//Top, second triangle.
		case 33:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MINIMUM.z;

			vertex_normal = vec3(0.0f, 1.0f, 0.0f);

			break;
		}

		case 34:
		{
			vertex_position.x = MAXIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(0.0f, 1.0f, 0.0f);

			break;
		}

		case 35:
		{
			vertex_position.x = MINIMUM.x;
			vertex_position.y = MAXIMUM.y;
			vertex_position.z = MAXIMUM.z;

			vertex_normal = vec3(0.0f, 1.0f, 0.0f);

			break;
		}

		default:
		{
			vertex_position = vec3(0.0f, 0.0f, 0.0f);
			vertex_normal = vec3(0.0f, 1.0f, 0.0f);

			break;
		}
	}
}

void CatalystShaderMain()
{
	//Calculate the vertex position and normal.
	vec3 vertex_position;
	vec3 vertex_normal;

	CalculateVertexPositionAndNormal(gl_VertexIndex, vertex_position, vertex_normal);

    //Write the position.
    gl_Position = WORLD_TO_CLIP_MATRIX * vec4(vertex_position, 1.0f);
} 