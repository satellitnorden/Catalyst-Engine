#if defined(AUDIO_PLUGIN)
//Header file.
#include <Audio/Plugins/Editor/Renderers/OpenGLPluginRenderer.h>

//Content.
#include <Content/Assets/FontAsset.h>
#include <Content/Assets/Texture2DAsset.h>

//Rendering.
#include <Rendering/Abstraction/OpenGL/OpenGLShader.h>
#include <Rendering/Abstraction/OpenGL/OpenGLTexture.h>
#include <Rendering/Native/Color.h>

//Systems.
#include <Systems/ContentSystem.h>

namespace Shaders
{
	//The vertex shader.
	constexpr const char *const RESTRICT VERTEX
	{
		"#version 330 core\n"
		"\n"
		"uniform vec2 RESOLUTION;\n"
		"\n"
		"uniform vec4 POSITIONS[4];\n"
		"uniform vec2 TEXTURE_COORDINATES[4];\n"
		"\n"
		"out vec2 TEXTURE_COORDINATE;\n"
		"out vec2 PIXEL_POSITION;\n"
		"\n"
		"vec2 ClipToPixel(vec2 X)\n"
		"{\n"
		"	return (X * 0.5f + 0.5f) * RESOLUTION;\n"
		"}\n"
		"\n"
		"void main()\n"
		"{\n"
		"	TEXTURE_COORDINATE = TEXTURE_COORDINATES[gl_VertexID];\n"
		"	PIXEL_POSITION = ClipToPixel(POSITIONS[gl_VertexID].xy);\n"
		"	gl_Position = POSITIONS[gl_VertexID];\n"
		"}\0"
	};

	constexpr const char *const RESTRICT FRAGMENT
	{
		"#version 330 core\n"
		"\n"
		"#define MODE_NONE uint(0)\n"
		"\n"
		"#define MODE_COLOR uint(1)\n"
		"#define MODE_TEXTURE uint(2)\n"
		"\n"
		"#define MODE_TEXT uint(3)\n"
		"\n"
		"#define SHAPE_NONE uint(0)\n"
		"\n"
		"#define SHAPE_SOLID_BOX uint(1)\n"
		"#define SHAPE_ROUNDED_BOX uint(2)\n"
		"#define SHAPE_SOLID_CIRCLE uint(3)\n"
		"#define SHAPE_HOLLOW_CIRCLE uint(4)\n"
		"#define SHAPE_LINE uint(5)\n"
		"\n"
		"\nin vec2 TEXTURE_COORDINATE;"
		"\nin vec2 PIXEL_POSITION;"
		"\n"
		"uniform vec2 RESOLUTION;\n"
		"\n"
		"uniform vec2 CORNERS[4];\n"
		"uniform vec2 CENTER;\n"
		"\n"
		"uniform uint MODE;\n"
		"uniform vec4 COLOR;\n"
		"uniform sampler2D TEXTURE;\n"
		"uniform vec4 COLOR_OPACITY;\n"
		"uniform float MODE_PARAMETER_1;\n"
		"uniform uint SHAPE;\n"
		"uniform float SHAPE_PARAMETER_1;\n"
		"uniform float SHAPE_PARAMETER_2;\n"
		"\n"
		"out vec4 FRAGMENT;\n"
		"\n"
		"float DistanceToLineSegment(vec2 start, vec2 end, vec2 point)\n"
		"{\n"
		"	vec2 point_to_start = point - start;\n"
		"	vec2 end_to_start = end - start;\n"
		"	float factor = clamp(dot(point_to_start, end_to_start) / dot(end_to_start, end_to_start), 0.0f, 1.0f);\n"
		"	return length(point_to_start - end_to_start * factor);\n"
		"}\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 color;\n"
		"\n"
		"	switch (MODE)\n"
		"	{\n"
		"		case MODE_COLOR:\n"
		"		{\n"
		"			color = COLOR;\n"
		"\n"
		"			break;\n"
		"		}\n"
		"		case MODE_TEXTURE:\n"
		"		{\n"
		"			color = texture(TEXTURE, TEXTURE_COORDINATE);\n"
		"\n"
		"			break;\n"
		"		}\n"
		"		case MODE_TEXT:\n"
		"		{\n"
		"			float text_sdf = texture(TEXTURE, TEXTURE_COORDINATE).x;\n"
		"			float screen_pixel_distance = fwidth(text_sdf);\n"
		"			float opacity = smoothstep(0.5f - screen_pixel_distance, 0.5f, text_sdf);\n"
		"			color = vec4(1.0f, 1.0f, 1.0f, opacity);\n"
		"\n"
		"			break;\n"
		"		}\n"
		"	}\n"
		"\n"
		"	color *= COLOR_OPACITY;\n"
		"\n"
		"	switch (SHAPE)\n"
		"	{\n"
		"		case SHAPE_SOLID_CIRCLE:\n"
		"		{\n"
		"			float distance_to_center = length(CENTER - PIXEL_POSITION);\n"
		"			float signed_distance = distance_to_center - SHAPE_PARAMETER_1;\n"
		"			float anti_aliasing = fwidth(signed_distance);\n"
		"			color.a *= 1.0f - smoothstep(0.0f, anti_aliasing, signed_distance);\n"
		"\n"
		"			break;\n"
		"		}\n"
		"\n"
		"		case SHAPE_HOLLOW_CIRCLE:\n"
		"		{\n"
		"			float distance_to_center = length(CENTER - PIXEL_POSITION);\n"
		"			float signed_distance_1 = distance_to_center - SHAPE_PARAMETER_1;\n"
		"			float signed_distance_2 = (distance_to_center - SHAPE_PARAMETER_2) * -1.0f;\n"
		"			float signed_distance = max(signed_distance_1, signed_distance_2);\n"
		"			float anti_aliasing = fwidth(signed_distance);\n"
		"			color.a *= 1.0f - smoothstep(0.0f, anti_aliasing, signed_distance);\n"
		"\n"
		"			break;\n"
		"		}\n"
		"\n"
		"		case SHAPE_LINE:\n"
		"		{\n"
		"			vec2 start = mix(CORNERS[0], CORNERS[3], 0.5f);\n"
		"			vec2 end = mix(CORNERS[1], CORNERS[2], 0.5f);\n"
		"			vec2 line_direction = normalize(end - start);\n"
		"			start += line_direction * (SHAPE_PARAMETER_1 + 1.0f);\n"
		"			end -= line_direction * (SHAPE_PARAMETER_1 + 1.0f);\n"
		"			float signed_distance = DistanceToLineSegment(start, end, PIXEL_POSITION) - SHAPE_PARAMETER_1;\n"
		"			float anti_aliasing = fwidth(signed_distance);\n"
		"			color.a *= 1.0f - smoothstep(0.0f, anti_aliasing, signed_distance);\n"
		"\n"
		"			break;\n"
		"		}\n"
		"	}\n"
		"\n"
		"	FRAGMENT = color;\n"
		"}\0"
	};
}

namespace Audio
{

	/*
	*	Initializes this plugin renderer.
	*/
	void OpenGLPluginRenderer::Initialize(const Resolution resolution, Audio::PluginWindow *const RESTRICT window) NOEXCEPT
	{
		//Set the resolution.
		_Resolution = resolution;

		//Set the window.
		_Window = window;

		//Create the vertex shader.
		OpenGL::Identifier vertex_shader;

		if (!OpenGL::Shader::Create(&vertex_shader, GL_VERTEX_SHADER, Shaders::VERTEX))
		{
			ASSERT(false, "Could not compile vertex shader!");
		}

		//Create the fragment shader.
		OpenGL::Identifier fragment_shader;

		if (!OpenGL::Shader::Create(&fragment_shader, GL_FRAGMENT_SHADER, Shaders::FRAGMENT))
		{
			ASSERT(false, "Could not compile fragment shader!");
		}

		//Set up the program.
		OPEN_GL_CALL(_Program = glCreateProgram());
		OPEN_GL_CALL(glAttachShader(_Program, vertex_shader));
		OPEN_GL_CALL(glAttachShader(_Program, fragment_shader));
		OPEN_GL_CALL(glLinkProgram(_Program));

		{
			int32 success;

			OPEN_GL_CALL(glGetProgramiv(_Program, GL_LINK_STATUS, &success));

			if (success == 0)
			{
				char info_log[512];
				OPEN_GL_CALL(glGetProgramInfoLog(_Program, ARRAY_LENGTH(info_log), nullptr, info_log));
				LOG_ERROR(info_log);
				BREAKPOINT();
			}
		}

		//Delete the shaders, don't need them anymore after we've linked them into the program.
		OPEN_GL_CALL(glDeleteShader(vertex_shader));
		OPEN_GL_CALL(glDeleteShader(fragment_shader));

		//Generate the vertex array object.
		OPEN_GL_CALL(glGenVertexArrays(1, &_VertexArrayObject));

		//Create the textures.
		CreateTextures();
	}

	/*
	*	Renders.
	*/
	void OpenGLPluginRenderer::Render(const DynamicArray<UI::RenderCommand> &render_commands) NOEXCEPT
	{
		//Clear the framebuffer.
		OPEN_GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		OPEN_GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
		
		//Start using the program.
		OPEN_GL_CALL(glUseProgram(_Program));

		//Bind the vertex array object.
		OPEN_GL_CALL(glBindVertexArray(_VertexArrayObject));

		//Set up the blending.
		OPEN_GL_CALL(glEnable(GL_BLEND));
		OPEN_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//Render all render commands.
		for (const UI::RenderCommand &render_command : render_commands)
		{
			Render(render_command);
		}

		//Finally, swap the buffers!
		_Window->SwapBuffers();
	}

	/*
	*	Terminates this plugin renderer.
	*/
	void OpenGLPluginRenderer::Terminate() NOEXCEPT
	{

	}

	/*
	*	Creates the textures.
	*/
	void OpenGLPluginRenderer::CreateTextures() NOEXCEPT
	{
		//Create texturres for all texture 2D assets.
		{
			const HashTable<HashString, Asset *RESTRICT> &assets{ ContentSystem::Instance->GetAllAssetsOfType(Texture2DAsset::TYPE_IDENTIFIER) };

			for (const Asset *const RESTRICT asset : assets.ValueIterator())
			{
				const Texture2DAsset *const RESTRICT _asset{ static_cast<const Texture2DAsset *const RESTRICT>(asset) };

				OpenGL::Identifier identifier;
				OpenGL::Texture::Create(&identifier, _asset->_Texture2D.GetWidth(), _asset->_Texture2D.GetHeight(), GL_RGBA, _asset->_Texture2D.Data());

				_Textures.Add(_asset->_Index, identifier);
			}
		}

		//Create texturres for all font assets.
		{
			const HashTable<HashString, Asset *RESTRICT> &assets{ ContentSystem::Instance->GetAllAssetsOfType(FontAsset::TYPE_IDENTIFIER) };

			for (const Asset *const RESTRICT asset : assets.ValueIterator())
			{
				const FontAsset *const RESTRICT _asset{ static_cast<const FontAsset *const RESTRICT>(asset) };

				OpenGL::Identifier identifier;
				OpenGL::Texture::Create(&identifier, _asset->_Texture2D.GetWidth(), _asset->_Texture2D.GetHeight(), GL_RED, _asset->_Texture2D.Data());

				_Textures.Add(_asset->_MasterTextureIndex, identifier);
			}
		}
	}

	/*
	*	Renders the given render command.
	*/
	void OpenGLPluginRenderer::Render(const UI::RenderCommand &render_command) NOEXCEPT
	{
		//Push the resolution uniform.
		{
			OPEN_GL_CALL(const int32 resolution_uniform_location{ glGetUniformLocation(_Program, "RESOLUTION") });

			const Vector2<float32> resolution
			{
				static_cast<float32>(_Resolution._Width),
				static_cast<float32>(_Resolution._Height)
			};

			OPEN_GL_CALL(glUniform2f(resolution_uniform_location, resolution._X, resolution._Y));
		}

		//Push the corners and the center uniform.
		{
			OPEN_GL_CALL(const int32 corners_uniform_location{ glGetUniformLocation(_Program, "CORNERS") });

			const Vector2<float32> corners[4]
			{
				Vector2<float32>
				(
					(render_command._Positions[0]._X * 0.5f + 0.5f) * static_cast<float32>(_Resolution._Width),
					(1.0f - (render_command._Positions[0]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Resolution._Height)
				),
				Vector2<float32>
				(
					(render_command._Positions[1]._X * 0.5f + 0.5f) * static_cast<float32>(_Resolution._Width),
					(1.0f - (render_command._Positions[1]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Resolution._Height)
				),
				Vector2<float32>
				(
					(render_command._Positions[2]._X * 0.5f + 0.5f) * static_cast<float32>(_Resolution._Width),
					(1.0f - (render_command._Positions[2]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Resolution._Height)
				),
				Vector2<float32>
				(
					(render_command._Positions[3]._X * 0.5f + 0.5f) * static_cast<float32>(_Resolution._Width),
					(1.0f - (render_command._Positions[3]._Y * 0.5f + 0.5f)) * static_cast<float32>(_Resolution._Height)
				),
			};

			OPEN_GL_CALL(glUniform2fv(corners_uniform_location, 4, reinterpret_cast<const float32 *const RESTRICT>(corners)));

			OPEN_GL_CALL(const int32 center_uniform_location{ glGetUniformLocation(_Program, "CENTER") });

			const Vector2<float32> center{ BaseMath::LinearlyInterpolate(corners[0], corners[2], 0.5f)};

			OPEN_GL_CALL(glUniform2f(center_uniform_location, center._X, center._Y));
		}

		//Push the positions uniform.
		OPEN_GL_CALL(const int32 positions_uniform_location{ glGetUniformLocation(_Program, "POSITIONS") });

		StaticArray<Vector4<float32>, 4> positions{ render_command._Positions };

		for (Vector4<float32> &position : positions)
		{
			position._Y *= -1.0f;
		}

		OPEN_GL_CALL(glUniform4fv(positions_uniform_location, 4, reinterpret_cast<const float32 *const RESTRICT>(positions.Data())));

		//Push the texture coordinates uniform.
		OPEN_GL_CALL(const int32 texture_coordinates_uniform_location{ glGetUniformLocation(_Program, "TEXTURE_COORDINATES") });
		OPEN_GL_CALL(glUniform2fv(texture_coordinates_uniform_location, 4, reinterpret_cast<const float32 *const RESTRICT>(render_command._TextureCoordinates.Data())));

		//Push the mode uniform.
		OPEN_GL_CALL(const int32 mode_uniform_location{ glGetUniformLocation(_Program, "MODE") });
		OPEN_GL_CALL(glUniform1ui(mode_uniform_location, static_cast<uint32>(render_command._Mode)));

		//Push the color or texture uniform.
		if (render_command._Mode == UI::RenderCommand::Mode::COLOR)
		{
			const Vector4<float32> color{ Color(render_command._ColorOrTexture).Get() };

			OPEN_GL_CALL(const int32 color_uniform_location{ glGetUniformLocation(_Program, "COLOR") });
			OPEN_GL_CALL(glUniform4f(color_uniform_location, color._R, color._G, color._B, color._A));
		}
		
		else
		{
			OPEN_GL_CALL(glBindTexture(GL_TEXTURE_2D, _Textures[render_command._ColorOrTexture]));
		}

		//Push the color/opacity uniform.
		{
			const Vector4<float32> color_opacity{ Color(render_command._ColorOpacity).Get() };

			OPEN_GL_CALL(const int32 color_opacity_uniform_location{ glGetUniformLocation(_Program, "COLOR_OPACITY") });
			OPEN_GL_CALL(glUniform4f(color_opacity_uniform_location, color_opacity._R, color_opacity._G, color_opacity._B, color_opacity._A));
		}

		//Push the mode parameter 1 uniform.
		OPEN_GL_CALL(const int32 mode_parameter_1_uniform_location{ glGetUniformLocation(_Program, "MODE_PARAMETER_1") });
		OPEN_GL_CALL(glUniform1f(mode_parameter_1_uniform_location, render_command._ModeParameter1_float32));

		//Push the shape uniform.
		OPEN_GL_CALL(const int32 shape_uniform_location{ glGetUniformLocation(_Program, "SHAPE") });
		OPEN_GL_CALL(glUniform1ui(shape_uniform_location, static_cast<uint32>(render_command._Shape)));

		//Push the shape parameter 1 uniform.
		OPEN_GL_CALL(const int32 shape_parameter_1_uniform_location{ glGetUniformLocation(_Program, "SHAPE_PARAMETER_1") });
		OPEN_GL_CALL(glUniform1f(shape_parameter_1_uniform_location, render_command._ShapeParameter1_float32));

		//Push the shape parameter 2 uniform.
		OPEN_GL_CALL(const int32 shape_parameter_2_uniform_location{ glGetUniformLocation(_Program, "SHAPE_PARAMETER_2") });
		OPEN_GL_CALL(glUniform1f(shape_parameter_2_uniform_location, render_command._ShapeParameter2_float32));

		//Draw!
		OPEN_GL_CALL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));
	}

}
#endif