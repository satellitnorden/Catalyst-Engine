//Constants.
#define MODE_NONE (0)
#define MODE_ALBEDO (1)
#define MODE_THICKNESS (2)
#define MODE_NORMAL (3)
#define MODE_DEPTH (4)
#define MODE_ROUGHNESS (5)
#define MODE_METALLIC (6)
#define MODE_AMBIENT_OCCLUSION (7)
#define MODE_EMISSIVE (8)
#define MODE_DIFFUSE_IRRADIANCE (9)

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragment_texture_coordinate;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uint MODE;
};

//Texture samplers.
layout (set = 1, binding = 0) uniform sampler2D SOURCE_TEXTURE;

//Out parameters.
layout (location = 0) out vec4 fragment;

void CatalystShaderMain()
{
	switch (MODE)
	{
		case MODE_NONE:
		{
			fragment = vec4(1.0f, 0.0f, 0.0f, 1.0f);

			break;
		}

		case MODE_ALBEDO:
		{
			fragment = vec4(texture(SOURCE_TEXTURE, fragment_texture_coordinate).rgb, 1.0f);

			break;
		}

		case MODE_THICKNESS:
		{
			fragment = vec4(vec3(texture(SOURCE_TEXTURE, fragment_texture_coordinate).a), 1.0f);

			break;
		}

		case MODE_NORMAL:
		{
			fragment = vec4(texture(SOURCE_TEXTURE, fragment_texture_coordinate).rgb * 0.5f + 0.5f, 1.0f);

			break;
		}

		case MODE_DEPTH:
		{
			fragment = vec4(vec3(texture(SOURCE_TEXTURE, fragment_texture_coordinate).a), 1.0f);

			break;
		}

		case MODE_ROUGHNESS:
		{
			fragment = vec4(vec3(texture(SOURCE_TEXTURE, fragment_texture_coordinate).r), 1.0f);

			break;
		}

		case MODE_METALLIC:
		{
			fragment = vec4(vec3(texture(SOURCE_TEXTURE, fragment_texture_coordinate).g), 1.0f);

			break;
		}

		case MODE_AMBIENT_OCCLUSION:
		{
			fragment = vec4(vec3(texture(SOURCE_TEXTURE, fragment_texture_coordinate).b), 1.0f);

			break;
		}

		case MODE_EMISSIVE:
		{
			fragment = vec4(vec3(texture(SOURCE_TEXTURE, fragment_texture_coordinate).a), 1.0f);

			break;
		}

		case MODE_DIFFUSE_IRRADIANCE:
		{
			fragment = vec4(vec3(texture(SOURCE_TEXTURE, fragment_texture_coordinate).rgb), 1.0f);

			break;
		}

		default:
		{
			fragment = vec4(1.0f, 0.0f, 0.0f, 1.0f);

			break;
		}
	}
}