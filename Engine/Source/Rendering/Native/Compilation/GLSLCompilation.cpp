//Header file.
#include <Rendering/Native/Compilation/GLSLCompilation.h>

//Systems.
#include <Systems/LogSystem.h>

//Third party.
#include <vulkan/shaderc/shaderc.h>

/*
*	Compiles a GLSL shader of the given shader stage with the given lines.
*/
NO_DISCARD bool GLSLCompilation::Compile(const CompileParameters &parameters) NOEXCEPT
{
	//Gather the shader source.
	DynamicArray<char> shader_source;

	{
		uint64 required_shader_source_size{ 0 };

		for (uint64 line_index{ 0 }; line_index < parameters._InputLines->Size(); ++line_index)
		{
			required_shader_source_size += parameters._InputLines->At(line_index).Length();

			if (line_index != parameters._InputLines->LastIndex())
			{
				++required_shader_source_size;
			}
		}

		shader_source.Upsize<false>(required_shader_source_size);
	}

	{
		uint64 current_index{ 0 };

		for (uint64 line_index{ 0 }; line_index < parameters._InputLines->Size(); ++line_index)
		{
			Memory::Copy(&shader_source[current_index], parameters._InputLines->At(line_index).Data(), parameters._InputLines->At(line_index).Length() * sizeof(char));
			current_index += parameters._InputLines->At(line_index).Length();

			if (line_index != parameters._InputLines->LastIndex())
			{
				constexpr char new_line_character{ '\n' };
				
				Memory::Copy(&shader_source[current_index], &new_line_character, 1 * sizeof(char));
				current_index += 1;
			}
		}
	}

	//Figure out the shader kind.
	shaderc_shader_kind shader_kind;

	switch (parameters._ShaderStage)
	{
		case ShaderStage::COMPUTE:
		{
			shader_kind = shaderc_shader_kind::shaderc_glsl_compute_shader;

			break;
		}

		case ShaderStage::FRAGMENT:
		{
			shader_kind = shaderc_shader_kind::shaderc_glsl_fragment_shader;

			break;
		}

		case ShaderStage::RAY_ANY_HIT:
		{
			shader_kind = shaderc_shader_kind::shaderc_glsl_anyhit_shader;

			break;
		}

		case ShaderStage::RAY_CLOSEST_HIT:
		{
			shader_kind = shaderc_shader_kind::shaderc_glsl_closesthit_shader;

			break;
		}

		case ShaderStage::RAY_GENERATION:
		{
			shader_kind = shaderc_shader_kind::shaderc_glsl_raygen_shader;

			break;
		}

		case ShaderStage::RAY_MISS:
		{
			shader_kind = shaderc_shader_kind::shaderc_glsl_miss_shader;

			break;
		}

		case ShaderStage::VERTEX:
		{
			shader_kind = shaderc_shader_kind::shaderc_glsl_vertex_shader;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Initialize the compiler.
	shaderc_compiler_t compiler{ shaderc_compiler_initialize() };

	//Initialize the options.
	shaderc_compile_options_t options{ shaderc_compile_options_initialize() };

	//Set the source language.
	shaderc_compile_options_set_source_language(options, shaderc_source_language::shaderc_source_language_glsl);

	//Set the optimization level.
#if 0
	shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level::shaderc_optimization_level_zero);
	shaderc_compile_options_set_generate_debug_info(options);
#else
	shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level::shaderc_optimization_level_performance);
#endif

	//Set the target environment.
	shaderc_compile_options_set_target_env(options, shaderc_target_env::shaderc_target_env_vulkan, shaderc_env_version::shaderc_env_version_vulkan_1_4);

	//Set warnings as errors.
	shaderc_compile_options_set_warnings_as_errors(options);

	//Compile!
	shaderc_compilation_result_t result{ shaderc_compile_into_spv(compiler, shader_source.Data(), shader_source.Size(), shader_kind, parameters._InputFilePath, "main", options) };

	//Check for errors.
	if (shaderc_result_get_num_errors(result) > 0)
	{
		LOG_ERROR(shaderc_result_get_error_message(result));
		ASSERT(false, "Error!");

		return false;
	}

	//Retrieve the compiled file size.
	const uint64 compiled_file_size{ shaderc_result_get_length(result) };

	//Copy the data.
	parameters._OutputData->Upsize<false>(compiled_file_size);
	Memory::Copy(parameters._OutputData->Data(), shaderc_result_get_bytes(result), compiled_file_size);

	return true;
}