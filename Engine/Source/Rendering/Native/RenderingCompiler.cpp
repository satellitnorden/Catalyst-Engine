#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
//Header file.
#include <Rendering/Native/RenderingCompiler.h>

//Core.
#include <Core/Algorithms/HashAlgorithms.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/Pair.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Core/BinaryFile.h>
#include <File/Utilities/TextParsingUtilities.h>

//Resources.
#include <Resources/Building/RenderPipelineBuildParameters.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/ResourceSystem.h>

//Third party.
#include <ThirdParty/vulkan/shaderc/shaderc.h>

//Singleton definition.
DEFINE_SINGLETON(RenderingCompiler);

/*
*	Render pipeline cache class definition.
*/
class RenderPipelineCache final
{

public:

	//The current version.
	constexpr static uint64 CURRENT_VERSION{ 1 };

	/*
	*	Entry class definition.
	*/
	class Entry final
	{

	public:

		//The identifier.
		uint64 _Identifier;

		//The last write time.
		std::filesystem::file_time_type _LastWriteTime;
	};

	//The version.
	uint64 _Version;

	//The entries.
	DynamicArray<Entry> _Entries;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE RenderPipelineCache(const char *const RESTRICT file_path) NOEXCEPT
	{
		if (!File::Exists(file_path))
		{
			return;
		}

		//Read the file.
		BinaryFile<BinaryFileMode::IN> file{ file_path };

		//Read the version.
		uint64 version;
		file.Read(&version, sizeof(uint64));

		//If this is not the current version, disregard.
		if (version != CURRENT_VERSION)
		{
			file.Close();

			return;
		}

		//Read the number of entries.
		uint64 number_of_entries;
		file.Read(&number_of_entries, sizeof(uint64));

		_Entries.Upsize<false>(number_of_entries);

		//Read the entries.
		for (Entry &entry : _Entries)
		{
			file.Read(&entry, sizeof(Entry));
		}

		//Close the file.
		file.Close();
	}

	/*
	*	Returns if the given identifier with the given last write time needs a recompile.
	*/
	FORCE_INLINE NO_DISCARD bool NeedsRecompile(const uint64 identifier, const std::filesystem::file_time_type last_write_time) NOEXCEPT
	{
#if 1
		return true;
#else
		for (Entry &entry : _Entries)
		{
			if (entry._Identifier == identifier)
			{
				if (entry._LastWriteTime < last_write_time)
				{
					entry._LastWriteTime = last_write_time;

					return true;
				}

				else
				{
					return false;
				}
			}
		}

		_Entries.Emplace();
		Entry &new_entry{ _Entries.Back() };

		new_entry._Identifier = identifier;
		new_entry._LastWriteTime = last_write_time;

		return true;
#endif
	}

	/*
	*	Writes this shader cache.
	*/
	FORCE_INLINE void Write(const char *const RESTRICT file_path) NOEXCEPT
	{
		//Open the file.
		BinaryFile<BinaryFileMode::OUT> file{ file_path };

		//Write the version.
		const uint64 version{ CURRENT_VERSION };
		file.Write(&version, sizeof(uint64));

		//Write the number of entries.
		const uint64 number_of_entries{ _Entries.Size() };
		file.Write(&number_of_entries, sizeof(uint64));

		//Write the entries.
		for (const Entry &entry : _Entries)
		{
			file.Write(&entry, sizeof(Entry));
		}

		//Close the file.
		file.Close();
	}

};

/*
*	Gathers shader lines.
*/
void GatherShaderLines(std::ifstream& file, DynamicArray<std::string> &lines) NOEXCEPT
{
	uint8 number_of_brackets{ 0 };

	do
	{
		//Retrieve the new line.
		std::string new_line;
		if (!std::getline(file, new_line))
		{
			break;
		}

		if (TextParsingUtilities::OnlyWhitespace(new_line.data(), new_line.length())
			|| TextParsingUtilities::OnlyComment(new_line.data(), new_line.length()))
		{
			continue;
		}

		if (new_line == "{")
		{
			++number_of_brackets;
		}

		else if (new_line == "}")
		{
			--number_of_brackets;
		}

		//Add the new line.
		lines.Emplace(std::move(new_line));
	} while (number_of_brackets > 0);
}

/*
*	Compiles a GLSL shader.
*/
void CompileGLSLShader(const char *const RESTRICT file_path, const shaderc_shader_kind shader_kind, DynamicArray<byte> *const RESTRICT data) NOEXCEPT
{
	//Retrieve the shader source.
	std::string shader_source;

	{
		std::ifstream file{ file_path };

		file.seekg(0, std::ios::end);
		shader_source.reserve(file.tellg());
		file.seekg(0, std::ios::beg);

		shader_source.assign
		(
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
		);
	}

	//Initialize the compiler.
	shaderc_compiler_t compiler{ shaderc_compiler_initialize() };

	//Initialize the options.
	shaderc_compile_options_t options{ shaderc_compile_options_initialize() };

	//Set the source language.
	shaderc_compile_options_set_source_language(options, shaderc_source_language::shaderc_source_language_glsl);

	//Set the optimization level.
	shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level::shaderc_optimization_level_performance);

	//Compile!
	shaderc_compilation_result_t result{ shaderc_compile_into_spv(compiler, shader_source.c_str(), shader_source.size(), shader_kind, file_path, "main", options) };

	//Check for errors.
	if (shaderc_result_get_num_errors(result) > 0)
	{
		ASSERT(false, shaderc_result_get_error_message(result));
	}

	//Retrieve the compiled file size.
	const uint64 compiled_file_size{ shaderc_result_get_length(result) };

	//Copy the data.
	data->Upsize<false>(compiled_file_size);
	Memory::Copy(data->Data(), shaderc_result_get_bytes(result), compiled_file_size);
}

/*
*	Generates a vertex shader.
*/
void GenerateVertexShader(std::ifstream &file, const char *const RESTRICT generated_file_path, const std::string &shader_name, RenderPipelineBuildParameters *const RESTRICT parameters) NOEXCEPT
{
	//Gather all the lines in the vertex function.
	DynamicArray<std::string> lines;
	GatherShaderLines(file, lines);

	//Gather the output parameters.
	DynamicArray<Pair<DynamicString, DynamicString>> output_parameters;

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t output_parameter_position{ lines[i].find("OutputParameter") };

		if (output_parameter_position != std::string::npos)
		{
			StaticArray<DynamicString, 2> output_parameter_strings;

			TextParsingUtilities::ParseFunctionArguments
			(
				lines[i].data(),
				lines[i].length(),
				output_parameter_strings.Data()
			);

			output_parameters.Emplace(std::move(output_parameter_strings[0]), std::move(output_parameter_strings[1]));

			lines.EraseAt<true>(i);
		}

		else
		{
			++i;
		}
	}

	//Gather the output value.
	DynamicString output_value;

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t output_value_position{ lines[i].find("OutputVertexPosition") };

		if (output_value_position != std::string::npos)
		{
			TextParsingUtilities::ParseFunctionArguments
			(
				lines[i].data(),
				lines[i].length(),
				&output_value
			);

			lines.EraseAt<true>(i);

			break;
		}

		else
		{
			++i;
		}
	}

	//Write the GLSL file.
	{
		//Make a copy of the lines.
		DynamicArray<std::string> glsl_lines{ lines };

		//Remember the current location index.
		uint32 location_index{ 0 };

		//Open the file.
		char glsl_file_path[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(glsl_file_path, "%s\\%s_Vertex.glsl", generated_file_path, shader_name.c_str());
		std::ofstream glsl_file{ glsl_file_path };

		//Write the version declaration.
		glsl_file << "#version 460" << std::endl;

		//Write the output parameters.
		if (!output_parameters.Empty())
		{
			for (const Pair<DynamicString, DynamicString> &output_parameter : output_parameters)
			{
				glsl_file << "layout (location = " << location_index++ << ") out " << output_parameter._First.Data() << " " << output_parameter._Second.Data() << ";" << std::endl;
			}

			glsl_file << std::endl;
		}

		//Write the "void main()" line.
		glsl_file << "void main()" << std::endl;

		//Write the remaining lines.
		for (uint64 i{ 0 }; i < glsl_lines.Size(); ++i)
		{
			//Cache the line.
			std::string &line{ glsl_lines[i] };

			//If this is the closing bracket, output the output value and be done with it.
			if (i == glsl_lines.LastIndex())
			{
				glsl_file << "\tgl_Position = " << output_value.Data() << ";" << std::endl;
				glsl_file << line;
			}

			else
			{
				//Replace "VERTEX_INDEX" with "gl_VertexIndex".
				{
					const size_t position{ line.find("VERTEX_INDEX") };

					if (position != std::string::npos)
					{
						line.replace(position, strlen("VERTEX_INDEX"), "gl_VertexIndex");
					}
				}

				//Write the line.
				glsl_file << line << std::endl;
			}
		}

		//Close the file.
		glsl_file.close();

		//Compile the GLSL shader.
		CompileGLSLShader(glsl_file_path, shaderc_shader_kind::shaderc_vertex_shader, &parameters->_VertexShaderData._GLSLData);
	}
}

/*
*	Generates a fragment shader.
*/
void GenerateFragmentShader(std::ifstream &file, const char *const RESTRICT generated_file_path, const std::string &shader_name, RenderPipelineBuildParameters *const RESTRICT parameters) NOEXCEPT
{
	//Struct definitions.
	struct InputParameter final
	{
		DynamicString _Type;
		DynamicString _Name;
	};

	struct InputRenderTarget final
	{
		DynamicString _Identifier;
		DynamicString _MagnificationFilter;
		DynamicString _MipmapMode;
		DynamicString _AddressMode;
	};

	struct OutputRenderTarget final
	{
		DynamicString _Identifier;
	};

	struct OutputFragment final
	{
		DynamicString _Identifier;
		DynamicString _Value;
	};

	//Gather all the lines in the vertex function.
	DynamicArray<std::string> lines;
	GatherShaderLines(file, lines);

	//Gather the input parameters.
	DynamicArray<InputParameter> input_parameters;

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t input_parameter_position{ lines[i].find("InputParameter") };

		if (input_parameter_position != std::string::npos)
		{
			StaticArray<DynamicString, 2> input_parameter_strings;

			TextParsingUtilities::ParseFunctionArguments
			(
				lines[i].data(),
				lines[i].length(),
				input_parameter_strings.Data()
			);

			input_parameters.Emplace();
			input_parameters.Back()._Type = std::move(input_parameter_strings[0]);
			input_parameters.Back()._Name = std::move(input_parameter_strings[1]);

			lines.EraseAt<true>(i);
		}

		else
		{
			++i;
		}
	}

	//Gather the input render targets
	DynamicArray<InputRenderTarget> input_render_targets;

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t input_render_target_position{ lines[i].find("InputRenderTargets") };

		if (input_render_target_position != std::string::npos)
		{
			StaticArray<DynamicString, 4> input_render_target_strings;

			TextParsingUtilities::ParseFunctionArguments
			(
				lines[i].data(),
				lines[i].length(),
				input_render_target_strings.Data()
			);

			input_render_targets.Emplace();
			input_render_targets.Back()._Identifier = std::move(input_render_target_strings[0]);
			input_render_targets.Back()._MagnificationFilter = std::move(input_render_target_strings[1]);
			input_render_targets.Back()._MipmapMode = std::move(input_render_target_strings[2]);
			input_render_targets.Back()._AddressMode = std::move(input_render_target_strings[3]);

			lines.EraseAt<true>(i);
		}

		else
		{
			++i;
		}
	}

	//Gather the output render targets
	DynamicArray<OutputRenderTarget> output_render_targets;

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t output_render_target_position{ lines[i].find("OutputRenderTarget") };

		if (output_render_target_position != std::string::npos)
		{
			StaticArray<DynamicString, 1> output_render_target_strings;

			TextParsingUtilities::ParseFunctionArguments
			(
				lines[i].data(),
				lines[i].length(),
				output_render_target_strings.Data()
			);

			output_render_targets.Emplace();
			output_render_targets.Back()._Identifier = std::move(output_render_target_strings[0]);

			lines.EraseAt<true>(i);
		}

		else
		{
			++i;
		}
	}

	//Gather the output fragments
	DynamicArray<OutputFragment> output_fragments;

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t output_fragment_position{ lines[i].find("OutputFragment") };

		if (output_fragment_position != std::string::npos)
		{
			StaticArray<DynamicString, 2> output_fragment_strings;

			TextParsingUtilities::ParseFunctionArguments
			(
				lines[i].data(),
				lines[i].length(),
				output_fragment_strings.Data()
			);

			output_fragments.Emplace();
			output_fragments.Back()._Identifier = std::move(output_fragment_strings[0]);
			output_fragments.Back()._Value = std::move(output_fragment_strings[1]);

			lines.EraseAt<true>(i);
		}

		else
		{
			++i;
		}
	}

	//Write the GLSL file.
	{
		//Make a copy of the lines.
		DynamicArray<std::string> glsl_lines{ lines };

		//Open the file.
		char glsl_file_path[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(glsl_file_path, "%s\\%s_Fragment.glsl", generated_file_path, shader_name.c_str());
		std::ofstream glsl_file{ glsl_file_path };

		//Write the version declaration.
		glsl_file << "#version 460" << std::endl;

		//Write the input parameters.
		if (!input_parameters.Empty())
		{
			//Remember the current location index.
			uint32 location_index{ 0 };

			for (const InputParameter &input_parameter : input_parameters)
			{
				glsl_file << "layout (location = " << location_index++ << ") in " << input_parameter._Type.Data() << " " << input_parameter._Name.Data() << ";" << std::endl;
			}

			glsl_file << std::endl;
		}

		//Write the input render targets.
		if (!input_render_targets.Empty())
		{
			//Remember the current binding index.
			uint32 binding_index{ 0 };

			for (const InputRenderTarget &input_render_target : input_render_targets)
			{
				glsl_file << "layout (set = 0, binding = " << binding_index++ << ") uniform sampler2D " << input_render_target._Identifier.Data() << ";" << std::endl;
			}

			glsl_file << std::endl;
		}

		//Write the output render targets.
		if (!output_render_targets.Empty())
		{
			//Remember the current location index.
			uint32 location_index{ 0 };

			for (const OutputRenderTarget &output_render_target : output_render_targets)
			{
				glsl_file << "layout (location = " << location_index++ << ") out vec4 " << output_render_target._Identifier.Data() << ";" << std::endl;
			}

			glsl_file << std::endl;
		}

		//Write the "void main()" line.
		glsl_file << "void main()" << std::endl;

		//Write the remaining lines.
		for (uint64 i{ 0 }; i < glsl_lines.Size(); ++i)
		{
			//Cache the line.
			std::string &line{ glsl_lines[i] };

			//If this is the closing bracket, output the output fragments and be done with it.
			if (i == glsl_lines.LastIndex())
			{
				for (const OutputFragment &output_fragment : output_fragments)
				{
					glsl_file << "\t" << output_fragment._Identifier.Data() << " = " << output_fragment._Value.Data() << ";" << std::endl;
				}
				
				glsl_file << line;
			}

			else
			{
				//Write the line.
				glsl_file << line << std::endl;
			}
		}

		//Close the file.
		glsl_file.close();

		//Compile the GLSL shader.
		CompileGLSLShader(glsl_file_path, shaderc_shader_kind::shaderc_fragment_shader, &parameters->_FragmentShaderData._GLSLData);
	}

	//Write the input render targets.
	parameters->_InputRenderTargets.Reserve(input_render_targets.Size());

	for (const InputRenderTarget &input_render_target : input_render_targets)
	{
		parameters->_InputRenderTargets.Emplace(HashString(input_render_target._Identifier.Data()));
	}

	//Write the output render targets.
	parameters->_OutputRenderTargets.Reserve(output_render_targets.Size());

	for (const OutputRenderTarget &output_render_target : output_render_targets)
	{
		parameters->_OutputRenderTargets.Emplace(HashString(output_render_target._Identifier.Data()));
	}
}

/*
*	Runs the shader compiler.
*	Returns if new rendering data was compiled.
*/
NO_DISCARD bool RenderingCompiler::Run() NOEXCEPT
{
	//Remember if new rendering data was compiled.
	bool new_rendering_data_compiled{ false };

	//Parse render pipelines.
	{
		//Define constants.
		constexpr const char *const RESTRICT ENGINE_RENDER_PIPELINES_PATH{ "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Rendering\\Render Pipelines" };

		//Parse the directories.
		new_rendering_data_compiled |= ParseRenderPipelinesInDirectory(ENGINE_RENDER_PIPELINES_PATH);
	}

	return new_rendering_data_compiled;
}

/*
*	Parses render pipelines in the given directory.
*	Returns if new rendering data was compiled.
*/
NO_DISCARD bool RenderingCompiler::ParseRenderPipelinesInDirectory(const char *const RESTRICT directory_path) NOEXCEPT
{
	//Remember if new rendering data was compiled.
	bool new_rendering_data_compiled{ false };

	//Create the "Generated" directory.
	char generated_file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(generated_file_path, "%s\\Generated", directory_path);
	
	File::CreateDirectory(generated_file_path);
		
	//Read the render pipeline cache.
	char render_pipeline_cache_file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(render_pipeline_cache_file_path, "%s\\RenderPipelineCache.render_pipeline_cache", generated_file_path);
	RenderPipelineCache render_pipeline_cache{ render_pipeline_cache_file_path };

	//Find all .render_pipeline files.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		if (entry.is_directory())
		{
			continue;
		}

		//Cache the file path string.
		const std::string file_path_string{ entry.path().string() };

		//Cache the shader file properties.
		const size_t last_slash_position{ file_path_string.find_last_of("\\") };
		const std::string render_pipeline_name{ file_path_string.substr(last_slash_position + 1, file_path_string.length() - last_slash_position - strlen(".render_pipeline") - 1) };
		const uint64 render_pipeline_identifier{ HashAlgorithms::MurmurHash64(render_pipeline_name.data(), sizeof(char) * render_pipeline_name.length()) };
		const std::filesystem::file_time_type render_pipeline_last_write_time{ std::filesystem::last_write_time(file_path_string) };

		//Check if this render pipeline needs a recompile.
		if (!render_pipeline_cache.NeedsRecompile(render_pipeline_identifier, render_pipeline_last_write_time))
		{
			continue;
		}

		LOG_INFORMATION("Compiling render pipeline %s", render_pipeline_name.c_str());

		//Set up the build parameters.
		RenderPipelineBuildParameters parameters;

		//Fill in the output.
		char output_buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(output_buffer, "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Render Pipelines\\%s_RenderPipeline", render_pipeline_name.data());

		parameters._Output = output_buffer;

		//Open the file.
		std::ifstream file{ file_path_string };

		std::string current_line;

		while (std::getline(file, current_line))
		{
			//Is this the beginning of a vertex shader?
			if (current_line == "Vertex")
			{
				GenerateVertexShader(file, generated_file_path, render_pipeline_name, &parameters);
			}

			//Is this the beginning of a vertex shader?
			else if (current_line == "Fragment")
			{
				GenerateFragmentShader(file, generated_file_path, render_pipeline_name, &parameters);
			}
		}

		//Build the render pipeline!
		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildRenderPipeline(parameters);

		//New rendering data was compiled!
		new_rendering_data_compiled = true;
	}

	//Write the render pipeline cache.
	render_pipeline_cache.Write(render_pipeline_cache_file_path);

	return new_rendering_data_compiled;
}
#endif