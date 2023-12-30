#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
//Header file.
#include <Rendering/Native/Compilation/RenderingCompiler.h>

//Core.
#include <Core/Algorithms/HashAlgorithms.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/Pair.h>
#include <Core/General/Time.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Core/BinaryFile.h>
#include <File/Utilities/TextParsingUtilities.h>

//Rendering.
#include <Rendering/Native/SamplerProperties.h>
#include <Rendering/Native/Compilation/GLSLCompilation.h>

//Resources.
#include <Resources/Building/RenderPipelineBuildParameters.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/ResourceSystem.h>

//Third party.
#include <ThirdParty/vulkan/shaderc/shaderc.h>

//Constants.
#define ENGINE_RENDERING_DIRECTORY_PATH "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Rendering"
#define GLOBAL_RENDER_DATA_FILE_PATH "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Rendering\\Global Render Data\\GlobalRenderData.global_render_data"

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
#if 0
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
*	Uniform buffer include class definition.
*/
class UniformBufferInclude final
{

public:

	//The name.
	DynamicString _Name;

	//The file path.
	DynamicString _FilePath;

};

/*
*	Storage buffer include class definition.
*/
class StorageBufferInclude final
{

public:

	//The name.
	DynamicString _Name;

	//The file path.
	DynamicString _FilePath;

};

/*
*	Push constant data item value definition.
*/
class PushConstantDataValue final
{

public:

	//The type.
	DynamicString _Type;

	//The name.
	DynamicString _Name;

};

/*
*	Render pipeline information class definition.
*/
class RenderPipelineInformation final
{

public:

	//The uniform buffer includes.
	DynamicArray<UniformBufferInclude> _UniformBufferIncludes;

	//The storage buffer includes.
	DynamicArray<StorageBufferInclude> _StorageBufferIncludes;

	//The shader function library includes.
	DynamicArray<DynamicString> _ShaderFunctionLibraryIncludes;

	//The input render targets.
	DynamicArray<DynamicString> _InputRenderTargets;

	//The output depth buffers.
	DynamicString _OutputDepthBuffer;

	//The output render targets.
	DynamicArray<DynamicString> _OutputRenderTargets;

	//The render resolution.
	HashString _RenderResolution;

	//The color load operator.
	AttachmentLoadOperator _ColorLoadOperator{ AttachmentLoadOperator::DONT_CARE };

	//The color store operator.
	AttachmentStoreOperator _ColorStoreOperator{ AttachmentStoreOperator::DONT_CARE };

	//The depth/stencil load operator.
	AttachmentLoadOperator _DepthStencilLoadOperator{ AttachmentLoadOperator::DONT_CARE };

	//The depth/stencil store operator.
	AttachmentStoreOperator _DepthStencilStoreOperator{ AttachmentStoreOperator::DONT_CARE };

	//Denotes whether or not blend is enabled.
	bool _BlendEnabled{ false };

	//The blend color source factor.
	BlendFactor _BlendColorSourceFactor{ BlendFactor::SourceAlpha };

	//The blend color destination factor.
	BlendFactor _BlendColorDestinationFactor{ BlendFactor::OneMinusSourceAlpha };

	//The blend color operator.
	BlendOperator _BlendColorOperator{ BlendOperator::ADD };

	//The blend alpha source factor.
	BlendFactor _BlendAlphaSourceFactor{ BlendFactor::SourceAlpha };

	//The blend alpha destination factor.
	BlendFactor _BlendAlphaDestinationFactor{ BlendFactor::OneMinusSourceAlpha };

	//The blend alpha operator.
	BlendOperator _BlendAlphaOperator{ BlendOperator::ADD };

	//The cull mode.
	CullMode _CullMode{ CullMode::None };

	//Denotes whether or not depth test is enabled.
	bool _DepthTestEnabled{ false };

	//Denotes whether or not depth write is enabled.
	bool _DepthWriteEnabled{ false };

	//The depth compare operator.
	CompareOperator _DepthCompareOperator{ CompareOperator::Always };

	//Denotes whether or not stencil test is enabled.
	bool _StencilTestEnabled{ false };

	//The action performed on samples that fail the stencil test.
	StencilOperator _StencilFailOperator{ StencilOperator::Keep };

	//The action performed on samples that pass both the depth test and the stencil test.
	StencilOperator _StencilPassOperator{ StencilOperator::Keep };

	//The action performed on samples that fail the depth test but pass the stencil test.
	StencilOperator _StencilDepthFailOperator{ StencilOperator::Keep };

	//The stencil compare operator.
	CompareOperator _StencilCompareOperator{ CompareOperator::Always };

	//The stencil compare mask.
	uint32 _StencilCompareMask{ 0 };

	//The stencil write mask.
	uint32 _StencilWriteMask{ 0 };

	//The stencil reference mask.
	uint32 _StencilReferenceMask{ 0 };

	//The topology.
	Topology _Topology{ Topology::TriangleList };

	//The push constant data values.
	DynamicArray<PushConstantDataValue> _PushConstantDataValues;

	//The samplers.
	DynamicArray<Pair<DynamicString, SamplerProperties>> _Samplers;

	//The input stream subscriptions.
	DynamicArray<HashString> _InputStreamSubscriptions;

};

/*
*	Finds the file path for the common shader with the given name.
*/
void FindCommonShaderFilePath(const char* const RESTRICT name, DynamicString* const RESTRICT file_path) NOEXCEPT
{
	//Try the engine directory.
	{
		char buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(buffer, "%s\\Common Shaders\\%s.common_shader", ENGINE_RENDERING_DIRECTORY_PATH, name);

		if (File::Exists(buffer))
		{
			*file_path = buffer;

			return;
		}
	}

	ASSERT(false, "Couldn't find common shader file!");
}

/*
*	Finds the file path for the uniform buffer definition with the given name.
*/
void FindUniformBufferDefinitionFilePath(const char *const RESTRICT name, DynamicString *const RESTRICT file_path) NOEXCEPT
{
	//Try the engine directory.
	{
		char buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(buffer, "%s\\Uniform Buffer Definitions\\%s.uniform_buffer_definition", ENGINE_RENDERING_DIRECTORY_PATH, name);

		if (File::Exists(buffer))
		{
			*file_path = buffer;

			return;
		}
	}

	ASSERT(false, "Couldn't find uniform buffer definition file!");
}

/*
*	Finds the file path for the storage buffer definition with the given name.
*/
void FindStorageBufferDefinitionFilePath(const char *const RESTRICT name, DynamicString *const RESTRICT file_path) NOEXCEPT
{
	//Try the engine directory.
	{
		char buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(buffer, "%s\\Storage Buffer Definitions\\%s.storage_buffer_definition", ENGINE_RENDERING_DIRECTORY_PATH, name);

		if (File::Exists(buffer))
		{
			*file_path = buffer;

			return;
		}
	}

	ASSERT(false, "Couldn't find storage buffer definition file!");
}

/*
*	Finds the file path for the shader function library with the given name.
*/
void FindShaderFunctionLibraryFilePath(const char* const RESTRICT name, DynamicString* const RESTRICT file_path) NOEXCEPT
{
	//Try the engine directory.
	{
		char buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(buffer, "%s\\Shader Function Libraries\\%s.shader_function_library", ENGINE_RENDERING_DIRECTORY_PATH, name);

		if (File::Exists(buffer))
		{
			*file_path = buffer;

			return;
		}
	}

	ASSERT(false, "Couldn't find shader function library file!");
}

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
void GenerateVertexShader
(
	std::ifstream &file,
	const char *const RESTRICT generated_file_path,
	const std::string &shader_name,
	const RenderPipelineInformation &render_pipeline_information,
	RenderPipelineBuildParameters *const RESTRICT parameters
) NOEXCEPT
{
	//Gather all the lines in the vertex function.
	DynamicArray<std::string> lines;
	GatherShaderLines(file, lines);

	//Gather the input parameters parameters.
	DynamicArray<Pair<DynamicString, DynamicString>> input_parameters;

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

			input_parameters.Emplace(std::move(input_parameter_strings[0]), std::move(input_parameter_strings[1]));

			lines.EraseAt<true>(i);
		}

		else
		{
			++i;
		}
	}

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

		//Remember the current resource binding.
		uint32 resource_binding_index{ 0 };

		//Open the file.
		char glsl_file_path[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(glsl_file_path, "%s\\%s_Vertex.glsl", generated_file_path, shader_name.c_str());
		std::ofstream glsl_file{ glsl_file_path };

		//Write the version declaration.
		glsl_file << "#version 460" << std::endl;

		glsl_file << std::endl;

		//Insert extensions.
		GLSLCompilation::InsertExtensions(glsl_file);

		//Insert the global render data.
		GLSLCompilation::InsertFromFile(glsl_file, GLOBAL_RENDER_DATA_FILE_PATH);

		//Insert any included uniform buffers.
		if (!render_pipeline_information._UniformBufferIncludes.Empty())
		{
			for (const UniformBufferInclude &uniform_buffer_include : render_pipeline_information._UniformBufferIncludes)
			{
				GLSLCompilation::InsertBufferDefinition
				(
					glsl_file,
					uniform_buffer_include._FilePath.Data(),
					resource_binding_index++
				);

				glsl_file << std::endl;
			}
		}

		//Insert any included storage buffers.
		if (!render_pipeline_information._StorageBufferIncludes.Empty())
		{
			for (const StorageBufferInclude &storage_buffer_include : render_pipeline_information._StorageBufferIncludes)
			{
				GLSLCompilation::InsertBufferDefinition
				(
					glsl_file,
					storage_buffer_include._FilePath.Data(),
					resource_binding_index++
				);

				glsl_file << std::endl;
			}
		}

		//Insert any samplers.
		if (!render_pipeline_information._Samplers.Empty())
		{
			for (const Pair<DynamicString, SamplerProperties> &sampler : render_pipeline_information._Samplers)
			{
				glsl_file << "layout (set = 1, binding = " << resource_binding_index++ << ") uniform sampler " << sampler._First.Data() << ";" << std::endl;
			}

			glsl_file << std::endl;
		}

		//Insert any included shader function libraries.
		if (!render_pipeline_information._ShaderFunctionLibraryIncludes.Empty())
		{
			for (const DynamicString &shader_function_library_include : render_pipeline_information._ShaderFunctionLibraryIncludes)
			{
				GLSLCompilation::InsertShaderFunctionLibrary
				(
					glsl_file,
					shader_function_library_include.Data()
				);

				glsl_file << std::endl;
			}
		}

		//Write the push constant data.
		if (!render_pipeline_information._PushConstantDataValues.Empty())
		{
			uint64 current_offset{ 0 };

			glsl_file << "layout (push_constant) uniform PushConstantData" << std::endl;
			glsl_file << "{" << std::endl;

			for (const PushConstantDataValue &push_constant_data_value : render_pipeline_information._PushConstantDataValues)
			{
				glsl_file << "\tlayout (offset = " << current_offset << ") " << push_constant_data_value._Type.Data() << " " << push_constant_data_value._Name.Data() << ";" << std::endl;
				current_offset += GLSLCompilation::GetByteOffsetForType(push_constant_data_value._Type.Data());
			}

			glsl_file << "};" << std::endl;

			glsl_file << std::endl;
		}

		//Write the input parameters.
		if (!input_parameters.Empty())
		{
			//Remember the current location index.
			uint32 location_index{ 0 };

			for (const Pair<DynamicString, DynamicString> &input_parameter : input_parameters)
			{
				glsl_file << "layout (location = " << location_index++ << ") in " << input_parameter._First.Data() << " " << input_parameter._Second.Data() << ";" << std::endl;
			}

			glsl_file << std::endl;
		}

		//Write the input render targets.
		if (!render_pipeline_information._InputRenderTargets.Empty())
		{
			for (const DynamicString& input_render_target : render_pipeline_information._InputRenderTargets)
			{
				glsl_file << "layout (set = 1, binding = " << resource_binding_index++ << ") uniform sampler2D " << input_render_target.Data() << ";" << std::endl;
			}

			glsl_file << std::endl;
		}

		//Write the output parameters.
		if (!output_parameters.Empty())
		{
			//Remember the current location index.
			uint32 location_index{ 0 };

			for (const Pair<DynamicString, DynamicString> &output_parameter : output_parameters)
			{
				glsl_file << "layout (location = " << location_index << ") out " << output_parameter._First.Data() << " " << output_parameter._Second.Data() << ";" << std::endl;
			
				location_index += GLSLCompilation::GetLocationOffsetForType(output_parameter._First.Data());
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
					size_t position{ line.find("VERTEX_INDEX") };

					while (position != std::string::npos)
					{
						line.replace(position, strlen("VERTEX_INDEX"), "gl_VertexIndex");
						position = line.find("VERTEX_INDEX");
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
void GenerateFragmentShader
(
	std::ifstream &file,
	const char *const RESTRICT generated_file_path,
	const std::string &shader_name,
	const RenderPipelineInformation &render_pipeline_information,
	RenderPipelineBuildParameters *const RESTRICT parameters
) NOEXCEPT
{
	//Struct definitions.
	struct InputParameter final
	{
		DynamicString _Type;
		DynamicString _Name;
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

	//Check if early fragment tests should be enabled.
	const bool should_enable_early_fragment_tests
	{
		render_pipeline_information._DepthTestEnabled || render_pipeline_information._StencilTestEnabled
	};

	bool can_enable_early_fragment_tests{ true };

	for (const std::string &line : lines)
	{
		const size_t position{ line.find("discard;") };

		if (position != std::string::npos)
		{
			can_enable_early_fragment_tests = false;

			break;
		}
	}

	const bool enable_early_fragment_tests{ should_enable_early_fragment_tests && can_enable_early_fragment_tests };

	//Write the GLSL file.
	{
		//Make a copy of the lines.
		DynamicArray<std::string> glsl_lines{ lines };

		//Remember the current resource binding.
		uint32 resource_binding_index{ 0 };

		//Open the file.
		char glsl_file_path[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(glsl_file_path, "%s\\%s_Fragment.glsl", generated_file_path, shader_name.c_str());
		std::ofstream glsl_file{ glsl_file_path };

		//Write the version declaration.
		glsl_file << "#version 460" << std::endl;

		glsl_file << std::endl;

		//Insert extensions.
		GLSLCompilation::InsertExtensions(glsl_file);

		//Add the early fragment tests flag.
		if (enable_early_fragment_tests)
		{
			glsl_file << "layout (early_fragment_tests) in;" << std::endl;
			glsl_file << std::endl;
		}

		//Insert the global render data.
		GLSLCompilation::InsertFromFile(glsl_file, GLOBAL_RENDER_DATA_FILE_PATH);

		//Insert any included uniform buffers.
		if (!render_pipeline_information._UniformBufferIncludes.Empty())
		{
			for (const UniformBufferInclude &uniform_buffer_include : render_pipeline_information._UniformBufferIncludes)
			{
				GLSLCompilation::InsertBufferDefinition
				(
					glsl_file,
					uniform_buffer_include._FilePath.Data(),
					resource_binding_index++
				);

				glsl_file << std::endl;
			}
		}

		//Insert any included storage buffers.
		if (!render_pipeline_information._StorageBufferIncludes.Empty())
		{
			for (const StorageBufferInclude &storage_buffer_include : render_pipeline_information._StorageBufferIncludes)
			{
				GLSLCompilation::InsertBufferDefinition
				(
					glsl_file,
					storage_buffer_include._FilePath.Data(),
					resource_binding_index++
				);

				glsl_file << std::endl;
			}
		}

		//Insert any samplers.
		if (!render_pipeline_information._Samplers.Empty())
		{
			for (const Pair<DynamicString, SamplerProperties> &sampler : render_pipeline_information._Samplers)
			{
				glsl_file << "layout (set = 1, binding = " << resource_binding_index++ << ") uniform sampler " << sampler._First.Data() << ";" << std::endl;
			}

			glsl_file << std::endl;
		}

		//Insert any included shader function libraries.
		if (!render_pipeline_information._ShaderFunctionLibraryIncludes.Empty())
		{
			for (const DynamicString &shader_function_library_include : render_pipeline_information._ShaderFunctionLibraryIncludes)
			{
				GLSLCompilation::InsertShaderFunctionLibrary
				(
					glsl_file,
					shader_function_library_include.Data()
				);

				glsl_file << std::endl;
			}
		}

		//Write the push constant data.
		if (!render_pipeline_information._PushConstantDataValues.Empty())
		{
			uint64 current_offset{ 0 };

			glsl_file << "layout (push_constant) uniform PushConstantData" << std::endl;
			glsl_file << "{" << std::endl;

			for (const PushConstantDataValue& push_constant_data_value : render_pipeline_information._PushConstantDataValues)
			{
				glsl_file << "\tlayout (offset = " << current_offset << ") " << push_constant_data_value._Type.Data() << " " << push_constant_data_value._Name.Data() << ";" << std::endl;
				current_offset += GLSLCompilation::GetByteOffsetForType(push_constant_data_value._Type.Data());
			}

			glsl_file << "};" << std::endl;

			glsl_file << std::endl;
		}

		//Write the input parameters.
		if (!input_parameters.Empty())
		{
			//Remember the current location index.
			uint32 location_index{ 0 };

			for (const InputParameter &input_parameter : input_parameters)
			{
				glsl_file << "layout (location = " << location_index << ") in " << input_parameter._Type.Data() << " " << input_parameter._Name.Data() << ";" << std::endl;
			
				location_index += GLSLCompilation::GetLocationOffsetForType(input_parameter._Type.Data());
			}

			glsl_file << std::endl;
		}

		//Write the input render targets.
		if (!render_pipeline_information._InputRenderTargets.Empty())
		{
			for (const DynamicString &input_render_target : render_pipeline_information._InputRenderTargets)
			{
				glsl_file << "layout (set = 1, binding = " << resource_binding_index++ << ") uniform sampler2D " << input_render_target.Data() << ";" << std::endl;
			}

			glsl_file << std::endl;
		}

		//Write the output render targets.
		if (!render_pipeline_information._OutputRenderTargets.Empty())
		{
			//Remember the current location index.
			uint32 location_index{ 0 };

			for (const DynamicString &output_render_target : render_pipeline_information._OutputRenderTargets)
			{
				glsl_file << "layout (location = " << location_index++ << ") out vec4 " << output_render_target.Data() << ";" << std::endl;
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

			//Replace "FRAGMENT_COORDINATE" with "gl_FragCoord".
			{
				size_t position{ line.find("FRAGMENT_COORDINATE") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("FRAGMENT_COORDINATE"), "gl_FragCoord");
					position = line.find("FRAGMENT_COORDINATE");
				}
			}

			//Replace "FRAGMENT_FRONT_FACING" with "gl_FrontFacing".
			{
				const size_t position{ line.find("FRAGMENT_FRONT_FACING") };

				if (position != std::string::npos)
				{
					line.replace(position, strlen("FRAGMENT_FRONT_FACING"), "gl_FrontFacing");
				}
			}

			//Replace "OutputFragment(X)" calls.
			{
				const size_t position{ line.find("OutputFragment(") };

				if (position != std::string::npos)
				{
					StaticArray<DynamicString, 2> output_fragment_strings;

					TextParsingUtilities::ParseFunctionArguments
					(
						lines[i].data(),
						lines[i].length(),
						output_fragment_strings.Data()
					);

					line = "\t" + std::string(output_fragment_strings[0].Data()) + " = " + std::string(output_fragment_strings[1].Data()) + ";";
				}
			}

			//Write the line.
			glsl_file << line << std::endl;
		}

		//Close the file.
		glsl_file.close();

		//Compile the GLSL shader.
		CompileGLSLShader(glsl_file_path, shaderc_shader_kind::shaderc_fragment_shader, &parameters->_FragmentShaderData._GLSLData);
	}
}

/*
*	Runs the shader compiler.
*	Returns if new rendering data was compiled.
*/
NO_DISCARD bool RenderingCompiler::Run() NOEXCEPT
{
	//Cache the start time.
	const TimePoint start_time{ GetCurrentTimePoint() };

	//Remember if new rendering data was compiled.
	bool new_rendering_data_compiled{ false };

	//Parse render pipelines.
	{
		new_rendering_data_compiled |= ParseRenderPipelinesInDirectory(ENGINE_RENDERING_DIRECTORY_PATH "\\Render Pipelines");
	}

	LOG_INFORMATION("Rendering Compiler took %f seconds.", start_time.GetSecondsSince());

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

		//Set up the render pipeline information.
		RenderPipelineInformation render_pipeline_information;

		//Fill in the output.
		char output_buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(output_buffer, "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Render Pipelines\\%s_RenderPipeline", render_pipeline_name.data());

		parameters._Output = output_buffer;

		//Open the file.
		std::ifstream file{ file_path_string };

		std::string current_line;

		while (std::getline(file, current_line))
		{
			//Disregard whitespace and comment-only lines.
			if (TextParsingUtilities::OnlyWhitespace(current_line.data(), current_line.length())
				|| TextParsingUtilities::OnlyComment(current_line.data(), current_line.length()))
			{
				continue;
			}

			//Is this a uniform buffer include declaration?
			{
				const size_t input_render_target_position{ current_line.find("IncludeUniformBuffer") };

				if (input_render_target_position != std::string::npos)
				{
					render_pipeline_information._UniformBufferIncludes.Emplace();

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&render_pipeline_information._UniformBufferIncludes.Back()._Name
					);

					
					FindUniformBufferDefinitionFilePath
					(
						render_pipeline_information._UniformBufferIncludes.Back()._Name.Data(),
						&render_pipeline_information._UniformBufferIncludes.Back()._FilePath
					);

					continue;
				}
			}

			//Is this a storage buffer include declaration?
			{
				const size_t position{ current_line.find("IncludeStorageBuffer") };

				if (position != std::string::npos)
				{
					render_pipeline_information._StorageBufferIncludes.Emplace();

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&render_pipeline_information._StorageBufferIncludes.Back()._Name
					);


					FindStorageBufferDefinitionFilePath
					(
						render_pipeline_information._StorageBufferIncludes.Back()._Name.Data(),
						&render_pipeline_information._StorageBufferIncludes.Back()._FilePath
					);

					continue;
				}
			}

			//Is this a shader function library include declaration?
			{
				const size_t input_render_target_position{ current_line.find("IncludeShaderFunctionLibrary") };

				if (input_render_target_position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					render_pipeline_information._ShaderFunctionLibraryIncludes.Emplace();
					FindShaderFunctionLibraryFilePath(string.Data(), &render_pipeline_information._ShaderFunctionLibraryIncludes.Back());

					continue;
				}
			}

			//Is this an input render target declaration?
			{
				const size_t input_render_target_position{ current_line.find("InputRenderTarget") };

				if (input_render_target_position != std::string::npos)
				{
					StaticArray<DynamicString, 4> input_render_target_strings;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						input_render_target_strings.Data()
					);

					render_pipeline_information._InputRenderTargets.Emplace(std::move(input_render_target_strings[0]));

					continue;
				}
			}

			//Is this an output depth buffer declaration?
			{
				const size_t output_depth_buffer_position{ current_line.find("OutputDepthBuffer") };

				if (output_depth_buffer_position != std::string::npos)
				{
					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&render_pipeline_information._OutputDepthBuffer
					);

					continue;
				}
			}

			//Is this an output render target declaration?
			{
				const size_t output_render_target_position{ current_line.find("OutputRenderTarget") };

				if (output_render_target_position != std::string::npos)
				{
					render_pipeline_information._OutputRenderTargets.Emplace();

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&render_pipeline_information._OutputRenderTargets.Back()
					);

					continue;
				}
			}

			//Is this a render resolution declaration?
			{
				const size_t position{ current_line.find("RenderResolution") };

				if (position != std::string::npos)
				{
					DynamicString argument;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&argument
					);

					render_pipeline_information._RenderResolution = HashString(argument.Data());

					continue;
				}
			}

			//Is this a color load operator declaration?
			{
				const size_t position{ current_line.find("ColorLoadOperator") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "LOAD")
					{
						render_pipeline_information._ColorLoadOperator = AttachmentLoadOperator::LOAD;
					}

					else if (string == "CLEAR")
					{
						render_pipeline_information._ColorLoadOperator = AttachmentLoadOperator::CLEAR;
					}

					else if (string == "DONT_CARE")
					{
						render_pipeline_information._ColorLoadOperator = AttachmentLoadOperator::DONT_CARE;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a color store operator declaration?
			{
				const size_t position{ current_line.find("ColorStoreOperator") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "STORE")
					{
						render_pipeline_information._ColorStoreOperator = AttachmentStoreOperator::STORE;
					}

					else if (string == "DONT_CARE")
					{
						render_pipeline_information._ColorStoreOperator = AttachmentStoreOperator::DONT_CARE;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a depth/stencil load operator declaration?
			{
				const size_t position{ current_line.find("DepthStencilLoadOperator") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "LOAD")
					{
						render_pipeline_information._DepthStencilLoadOperator = AttachmentLoadOperator::LOAD;
					}

					else if (string == "CLEAR")
					{
						render_pipeline_information._DepthStencilLoadOperator = AttachmentLoadOperator::CLEAR;
					}

					else if (string == "DONT_CARE")
					{
						render_pipeline_information._DepthStencilLoadOperator = AttachmentLoadOperator::DONT_CARE;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a depth/stencil store operator declaration?
			{
				const size_t position{ current_line.find("DepthStencilStoreOperator") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "STORE")
					{
						render_pipeline_information._DepthStencilStoreOperator = AttachmentStoreOperator::STORE;
					}

					else if (string == "DONT_CARE")
					{
						render_pipeline_information._DepthStencilStoreOperator = AttachmentStoreOperator::DONT_CARE;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a blend enable declaration?
			{
				const size_t position{ current_line.find("BlendEnable") };

				if (position != std::string::npos)
				{
					render_pipeline_information._BlendEnabled = true;

					continue;
				}
			}

			//Is this a blend color source factor declaration?
			{
				const size_t position{ current_line.find("BlendColorSourceFactor") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "ZERO")
					{
						render_pipeline_information._BlendColorSourceFactor = BlendFactor::Zero;
					}

					else if (string == "ONE")
					{
						render_pipeline_information._BlendColorSourceFactor = BlendFactor::One;
					}

					else if (string == "SOURCE_COLOR")
					{
						render_pipeline_information._BlendColorSourceFactor = BlendFactor::SourceColor;
					}

					else if (string == "ONE_MINUS_SOURCE_COLOR")
					{
						render_pipeline_information._BlendColorSourceFactor = BlendFactor::OneMinusSourceColor;
					}

					else if (string == "DESTINATION_COLOR")
					{
						render_pipeline_information._BlendColorSourceFactor = BlendFactor::DestinationColor;
					}

					else if (string == "ONE_MINUS_DESTINATION_COLOR")
					{
						render_pipeline_information._BlendColorSourceFactor = BlendFactor::OneMinusDestinationColor;
					}

					else if (string == "SOURCE_ALPHA")
					{
						render_pipeline_information._BlendColorSourceFactor = BlendFactor::SourceAlpha;
					}

					else if (string == "ONE_MINUS_SOURCE_ALPHA")
					{
						render_pipeline_information._BlendColorSourceFactor = BlendFactor::OneMinusSourceAlpha;
					}

					else if (string == "DESTINATION_ALPHA")
					{
						render_pipeline_information._BlendColorSourceFactor = BlendFactor::DestinationAlpha;
					}

					else if (string == "ONE_MINUS_DESTINATION_ALPHA")
					{
						render_pipeline_information._BlendColorSourceFactor = BlendFactor::OneMinusDestinationAlpha;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a blend color destination factor declaration?
			{
				const size_t position{ current_line.find("BlendColorDestinationFactor") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "ZERO")
					{
						render_pipeline_information._BlendColorDestinationFactor = BlendFactor::Zero;
					}

					else if (string == "ONE")
					{
						render_pipeline_information._BlendColorDestinationFactor = BlendFactor::One;
					}

					else if (string == "SOURCE_COLOR")
					{
						render_pipeline_information._BlendColorDestinationFactor = BlendFactor::SourceColor;
					}

					else if (string == "ONE_MINUS_SOURCE_COLOR")
					{
						render_pipeline_information._BlendColorDestinationFactor = BlendFactor::OneMinusSourceColor;
					}

					else if (string == "DESTINATION_COLOR")
					{
						render_pipeline_information._BlendColorDestinationFactor = BlendFactor::DestinationColor;
					}

					else if (string == "ONE_MINUS_DESTINATION_COLOR")
					{
						render_pipeline_information._BlendColorDestinationFactor = BlendFactor::OneMinusDestinationColor;
					}

					else if (string == "SOURCE_ALPHA")
					{
						render_pipeline_information._BlendColorDestinationFactor = BlendFactor::SourceAlpha;
					}

					else if (string == "ONE_MINUS_SOURCE_ALPHA")
					{
						render_pipeline_information._BlendColorDestinationFactor = BlendFactor::OneMinusSourceAlpha;
					}

					else if (string == "DESTINATION_ALPHA")
					{
						render_pipeline_information._BlendColorDestinationFactor = BlendFactor::DestinationAlpha;
					}

					else if (string == "ONE_MINUS_DESTINATION_ALPHA")
					{
						render_pipeline_information._BlendColorDestinationFactor = BlendFactor::OneMinusDestinationAlpha;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a blend color operator declaration?
			{
				const size_t position{ current_line.find("BlendColorOperator") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "ADD")
					{
						render_pipeline_information._BlendColorOperator = BlendOperator::ADD;
					}

					else if (string == "SUBTRACT")
					{
						render_pipeline_information._BlendColorOperator = BlendOperator::SUBTRACT;
					}

					else if (string == "REVERSE_SUBTRACT")
					{
						render_pipeline_information._BlendColorOperator = BlendOperator::REVERSE_SUBTRACT;
					}

					else if (string == "MIN")
					{
						render_pipeline_information._BlendColorOperator = BlendOperator::MIN;
					}

					else if (string == "MAX")
					{
						render_pipeline_information._BlendColorOperator = BlendOperator::MAX;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a blend alpha source factor declaration?
			{
				const size_t position{ current_line.find("BlendAlphaSourceFactor") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "ZERO")
					{
						render_pipeline_information._BlendAlphaSourceFactor = BlendFactor::Zero;
					}

					else if (string == "ONE")
					{
						render_pipeline_information._BlendAlphaSourceFactor = BlendFactor::One;
					}

					else if (string == "SOURCE_COLOR")
					{
						render_pipeline_information._BlendAlphaSourceFactor = BlendFactor::SourceColor;
					}

					else if (string == "ONE_MINUS_SOURCE_COLOR")
					{
						render_pipeline_information._BlendAlphaSourceFactor = BlendFactor::OneMinusSourceColor;
					}

					else if (string == "DESTINATION_COLOR")
					{
						render_pipeline_information._BlendAlphaSourceFactor = BlendFactor::DestinationColor;
					}

					else if (string == "ONE_MINUS_DESTINATION_COLOR")
					{
						render_pipeline_information._BlendAlphaSourceFactor = BlendFactor::OneMinusDestinationColor;
					}

					else if (string == "SOURCE_ALPHA")
					{
						render_pipeline_information._BlendAlphaSourceFactor = BlendFactor::SourceAlpha;
					}

					else if (string == "ONE_MINUS_SOURCE_ALPHA")
					{
						render_pipeline_information._BlendAlphaSourceFactor = BlendFactor::OneMinusSourceAlpha;
					}

					else if (string == "DESTINATION_ALPHA")
					{
						render_pipeline_information._BlendAlphaSourceFactor = BlendFactor::DestinationAlpha;
					}

					else if (string == "ONE_MINUS_DESTINATION_ALPHA")
					{
						render_pipeline_information._BlendAlphaSourceFactor = BlendFactor::OneMinusDestinationAlpha;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a blend alpha destination factor declaration?
			{
				const size_t position{ current_line.find("BlendAlphaDestinationFactor") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "ZERO")
					{
						render_pipeline_information._BlendAlphaDestinationFactor = BlendFactor::Zero;
					}

					else if (string == "ONE")
					{
						render_pipeline_information._BlendAlphaDestinationFactor = BlendFactor::One;
					}

					else if (string == "SOURCE_COLOR")
					{
						render_pipeline_information._BlendAlphaDestinationFactor = BlendFactor::SourceColor;
					}

					else if (string == "ONE_MINUS_SOURCE_COLOR")
					{
						render_pipeline_information._BlendAlphaDestinationFactor = BlendFactor::OneMinusSourceColor;
					}

					else if (string == "DESTINATION_COLOR")
					{
						render_pipeline_information._BlendAlphaDestinationFactor = BlendFactor::DestinationColor;
					}

					else if (string == "ONE_MINUS_DESTINATION_COLOR")
					{
						render_pipeline_information._BlendAlphaDestinationFactor = BlendFactor::OneMinusDestinationColor;
					}

					else if (string == "SOURCE_ALPHA")
					{
						render_pipeline_information._BlendAlphaDestinationFactor = BlendFactor::SourceAlpha;
					}

					else if (string == "ONE_MINUS_SOURCE_ALPHA")
					{
						render_pipeline_information._BlendAlphaDestinationFactor = BlendFactor::OneMinusSourceAlpha;
					}

					else if (string == "DESTINATION_ALPHA")
					{
						render_pipeline_information._BlendAlphaDestinationFactor = BlendFactor::DestinationAlpha;
					}

					else if (string == "ONE_MINUS_DESTINATION_ALPHA")
					{
						render_pipeline_information._BlendAlphaDestinationFactor = BlendFactor::OneMinusDestinationAlpha;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a blend alpha operator declaration?
			{
				const size_t position{ current_line.find("BlendAlphaOperator") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "ADD")
					{
						render_pipeline_information._BlendAlphaOperator = BlendOperator::ADD;
					}

					else if (string == "SUBTRACT")
					{
						render_pipeline_information._BlendAlphaOperator = BlendOperator::SUBTRACT;
					}

					else if (string == "REVERSE_SUBTRACT")
					{
						render_pipeline_information._BlendAlphaOperator = BlendOperator::REVERSE_SUBTRACT;
					}

					else if (string == "MIN")
					{
						render_pipeline_information._BlendAlphaOperator = BlendOperator::MIN;
					}

					else if (string == "MAX")
					{
						render_pipeline_information._BlendAlphaOperator = BlendOperator::MAX;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a cull mode declaration?
			{
				const size_t position{ current_line.find("CullMode") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "NONE")
					{
						render_pipeline_information._CullMode = CullMode::None;
					}

					else if (string == "BACK")
					{
						render_pipeline_information._CullMode = CullMode::Back;
					}

					else if (string == "FRONT")
					{
						render_pipeline_information._CullMode = CullMode::Front;
					}

					else if (string == "FRONT_AND_BACK")
					{
						render_pipeline_information._CullMode = CullMode::FrontAndBack;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a depth test enable declaration?
			{
				const size_t position{ current_line.find("DepthTestEnable") };

				if (position != std::string::npos)
				{
					render_pipeline_information._DepthTestEnabled = true;

					continue;
				}
			}

			//Is this a depth write enable declaration?
			{
				const size_t position{ current_line.find("DepthWriteEnable") };

				if (position != std::string::npos)
				{
					render_pipeline_information._DepthWriteEnabled = true;

					continue;
				}
			}

			//Is this a depth compare operator declaration?
			{
				const size_t position{ current_line.find("DepthCompareOperator") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "ALWAYS")
					{
						render_pipeline_information._DepthCompareOperator = CompareOperator::Always;
					}

					else if (string == "EQUAL")
					{
						render_pipeline_information._DepthCompareOperator = CompareOperator::Equal;
					}

					else if (string == "GREATER")
					{
						render_pipeline_information._DepthCompareOperator = CompareOperator::Greater;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a stencil test enable declaration?
			{
				const size_t position{ current_line.find("StencilTestEnable") };

				if (position != std::string::npos)
				{
					render_pipeline_information._StencilTestEnabled = true;

					continue;
				}
			}

			//Is this a stencil fail operator declaration?
			{
				const size_t position{ current_line.find("StencilFailOperator") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "KEEP")
					{
						render_pipeline_information._StencilFailOperator = StencilOperator::Keep;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a stencil pass operator declaration?
			{
				const size_t position{ current_line.find("StencilPassOperator") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "KEEP")
					{
						render_pipeline_information._StencilPassOperator = StencilOperator::Keep;
					}

					else if (string == "REPLACE")
					{
						render_pipeline_information._StencilPassOperator = StencilOperator::Replace;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a stencil depth fail operator declaration?
			{
				const size_t position{ current_line.find("StencilDepthFailOperator") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "KEEP")
					{
						render_pipeline_information._StencilDepthFailOperator = StencilOperator::Keep;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a stencil compare operator declaration?
			{
				const size_t position{ current_line.find("StencilCompareOperator") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "ALWAYS")
					{
						render_pipeline_information._StencilCompareOperator = CompareOperator::Always;
					}

					else if (string == "EQUAL")
					{
						render_pipeline_information._StencilCompareOperator = CompareOperator::Equal;
					}

					else if (string == "GREATER")
					{
						render_pipeline_information._StencilCompareOperator = CompareOperator::Greater;
					}

					else if (string == "GREATER_OR_EQUAL")
					{
						render_pipeline_information._StencilCompareOperator = CompareOperator::GreaterOrEqual;
					}

					else if (string == "LESS")
					{
						render_pipeline_information._StencilCompareOperator = CompareOperator::Less;
					}

					else if (string == "LESS_OR_EQUAL")
					{
						render_pipeline_information._StencilCompareOperator = CompareOperator::LessOrEqual;
					}

					else if (string == "NEVER")
					{
						render_pipeline_information._StencilCompareOperator = CompareOperator::Never;
					}

					else if (string == "NOT_EQUAL")
					{
						render_pipeline_information._StencilCompareOperator = CompareOperator::NotEqual;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a stencil compare mask declaration?
			{
				const size_t position{ current_line.find("StencilCompareMask") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					render_pipeline_information._StencilCompareMask = std::stoul(string.Data());

					continue;
				}
			}

			//Is this a stencil write mask declaration?
			{
				const size_t position{ current_line.find("StencilWriteMask") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					render_pipeline_information._StencilWriteMask = std::stoul(string.Data());

					continue;
				}
			}

			//Is this a stencil reference mask declaration?
			{
				const size_t position{ current_line.find("StencilReferenceMask") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					render_pipeline_information._StencilReferenceMask = std::stoul(string.Data());

					continue;
				}
			}

			//Is this a topology declaration?
			{
				const size_t position{ current_line.find("Topology") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					if (string == "LINE_LIST")
					{
						render_pipeline_information._Topology = Topology::LineList;
					}

					else if (string == "LINE_STRIP")
					{
						render_pipeline_information._Topology = Topology::LineStrip;
					}

					else if (string == "TRIANGLE_FAN")
					{
						render_pipeline_information._Topology = Topology::TriangleFan;
					}

					else if (string == "TRIANGLE_LIST")
					{
						render_pipeline_information._Topology = Topology::TriangleList;
					}

					else if (string == "TRIANGLE_STRIP")
					{
						render_pipeline_information._Topology = Topology::TriangleStrip;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					continue;
				}
			}

			//Is this a push constant data declaration?
			{
				const size_t position{ current_line.find("PushConstantData") };

				if (position != std::string::npos)
				{
					StaticArray<DynamicString, 2> strings;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						strings.Data()
					);

					render_pipeline_information._PushConstantDataValues.Emplace();
					PushConstantDataValue &value{ render_pipeline_information._PushConstantDataValues.Back() };

					value._Type = std::move(strings[0]);
					value._Name = std::move(strings[1]);

					continue;
				}
			}

			//Is this a sampler declaration?
			{
				const size_t position{ current_line.find("Sampler") };

				if (position != std::string::npos)
				{
					StaticArray<DynamicString, 5> strings;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						strings.Data()
					);

					render_pipeline_information._Samplers.Emplace();
					Pair<DynamicString, SamplerProperties> &new_sampler{ render_pipeline_information._Samplers.Back() };

					new_sampler._First = std::move(strings[0]);

					if (strings[1] == "LINEAR")
					{
						new_sampler._Second._MagnificationFilter = TextureFilter::LINEAR;
					}

					else if (strings[1] == "NEAREST")
					{
						new_sampler._Second._MagnificationFilter = TextureFilter::NEAREST;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					if (strings[2] == "LINEAR")
					{
						new_sampler._Second._MipmapMode = MipmapMode::LINEAR;
					}

					else if (strings[2] == "NEAREST")
					{
						new_sampler._Second._MipmapMode = MipmapMode::NEAREST;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					if (strings[3] == "CLAMP_TO_BORDER")
					{
						new_sampler._Second._AddressMode = AddressMode::CLAMP_TO_BORDER;
					}

					else if (strings[3] == "CLAMP_TO_EDGE")
					{
						new_sampler._Second._AddressMode = AddressMode::CLAMP_TO_EDGE;
					}

					else if (strings[3] == "MIRROR_CLAMP_TO_EDGE")
					{
						new_sampler._Second._AddressMode = AddressMode::MIRROR_CLAMP_TO_EDGE;
					}

					else if (strings[3] == "MIRRORED_REPEAT")
					{
						new_sampler._Second._AddressMode = AddressMode::MIRRORED_REPEAT;
					}

					else if (strings[3] == "REPEAT")
					{
						new_sampler._Second._AddressMode = AddressMode::REPEAT;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					new_sampler._Second._AnisotropicSamples = static_cast<uint8>(std::stoul(strings[4].Data()));

					continue;
				}
			}

			//Is this an input stream subscription declaration?
			{
				const size_t position{ current_line.find("SubscribeToInputStream") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					render_pipeline_information._InputStreamSubscriptions.Emplace(HashString(string.Data()));

					continue;
				}
			}

			//Is this a common vertex shader include?
			{
				const size_t position{ current_line.find("IncludeCommonVertexShader(") };

				if (position != std::string::npos)
				{
					DynamicString string;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&string
					);

					DynamicString common_vertex_shader_file_path;
					FindCommonShaderFilePath(string.Data(), &common_vertex_shader_file_path);
					std::ifstream common_vertex_shader_file{ common_vertex_shader_file_path.Data() };

					GenerateVertexShader(common_vertex_shader_file, generated_file_path, render_pipeline_name, render_pipeline_information, &parameters);

					continue;
				}
			}
			
			//Is this the beginning of a vertex shader?
			if (current_line == "Vertex")
			{
				GenerateVertexShader(file, generated_file_path, render_pipeline_name, render_pipeline_information, &parameters);
			}

			//Is this the beginning of a vertex shader?
			else if (current_line == "Fragment")
			{
				GenerateFragmentShader(file, generated_file_path, render_pipeline_name, render_pipeline_information, &parameters);
			}

			else
			{
				ASSERT(false, "Unknown line " << current_line.data());
			}
		}

		//Fill in the included uniform buffers.
		if (!render_pipeline_information._UniformBufferIncludes.Empty())
		{
			parameters._IncludedUniformBuffers.Reserve(render_pipeline_information._UniformBufferIncludes.Size());

			for (const UniformBufferInclude &uniform_buffer_include : render_pipeline_information._UniformBufferIncludes)
			{
				parameters._IncludedUniformBuffers.Emplace(HashString(uniform_buffer_include._Name.Data()));
			}
		}

		//Fill in the included storage buffers.
		if (!render_pipeline_information._StorageBufferIncludes.Empty())
		{
			parameters._IncludedStorageBuffers.Reserve(render_pipeline_information._StorageBufferIncludes.Size());

			for (const StorageBufferInclude &storage_buffer_include : render_pipeline_information._StorageBufferIncludes)
			{
				parameters._IncludedStorageBuffers.Emplace(HashString(storage_buffer_include._Name.Data()));
			}
		}

		//Fill in the input render targets.
		if (!render_pipeline_information._InputRenderTargets.Empty())
		{
			parameters._InputRenderTargets.Reserve(render_pipeline_information._InputRenderTargets.Size());

			for (const DynamicString &input_render_target : render_pipeline_information._InputRenderTargets)
			{
				parameters._InputRenderTargets.Emplace(HashString(input_render_target.Data()));
			}
		}

		//Fill in the output depth buffer.
		if (render_pipeline_information._OutputDepthBuffer)
		{
			parameters._OutputDepthBuffer = HashString(render_pipeline_information._OutputDepthBuffer.Data());
		}

		//Fill in the output render targets.
		if (!render_pipeline_information._OutputRenderTargets.Empty())
		{
			parameters._OutputRenderTargets.Reserve(render_pipeline_information._OutputRenderTargets.Size());

			for (const DynamicString &output_render_target : render_pipeline_information._OutputRenderTargets)
			{
				parameters._OutputRenderTargets.Emplace(HashString(output_render_target.Data()));
			}
		}

		//Fill in the render resolution.
		parameters._RenderResolution = render_pipeline_information._RenderResolution;

		//Fill in the load/store operators.
		parameters._ColorLoadOperator = render_pipeline_information._ColorLoadOperator;
		parameters._ColorStoreOperator = render_pipeline_information._ColorStoreOperator;
		parameters._DepthStencilLoadOperator = render_pipeline_information._DepthStencilLoadOperator;
		parameters._DepthStencilStoreOperator = render_pipeline_information._DepthStencilStoreOperator;

		//Copy the blend properties.
		parameters._BlendEnabled = render_pipeline_information._BlendEnabled;
		parameters._BlendColorSourceFactor = render_pipeline_information._BlendColorSourceFactor;
		parameters._BlendColorDestinationFactor = render_pipeline_information._BlendColorDestinationFactor;
		parameters._BlendColorOperator = render_pipeline_information._BlendColorOperator;
		parameters._BlendAlphaSourceFactor = render_pipeline_information._BlendAlphaSourceFactor;
		parameters._BlendAlphaDestinationFactor = render_pipeline_information._BlendAlphaDestinationFactor;
		parameters._BlendAlphaOperator = render_pipeline_information._BlendAlphaOperator;

		//Copy the cull mode.
		parameters._CullMode = render_pipeline_information._CullMode;

		//Copy depth/stencil properties.
		parameters._DepthTestEnabled = render_pipeline_information._DepthTestEnabled;
		parameters._DepthWriteEnabled = render_pipeline_information._DepthWriteEnabled;
		parameters._DepthCompareOperator = render_pipeline_information._DepthCompareOperator;
		parameters._StencilTestEnabled = render_pipeline_information._StencilTestEnabled;
		parameters._StencilFailOperator = render_pipeline_information._StencilFailOperator;
		parameters._StencilPassOperator = render_pipeline_information._StencilPassOperator;
		parameters._StencilDepthFailOperator = render_pipeline_information._StencilDepthFailOperator;
		parameters._StencilCompareOperator = render_pipeline_information._StencilCompareOperator;
		parameters._StencilCompareMask = render_pipeline_information._StencilCompareMask;
		parameters._StencilWriteMask = render_pipeline_information._StencilWriteMask;
		parameters._StencilReferenceMask = render_pipeline_information._StencilReferenceMask;

		//Copy the topology.
		parameters._Topology = render_pipeline_information._Topology;

		//Fill in the sampler properties.
		for (const Pair<DynamicString, SamplerProperties> &sampler : render_pipeline_information._Samplers)
		{
			parameters._SamplerProperties.Emplace(sampler._Second);
		}

		//Copy the input stream subscriptions.
		parameters._InputStreamSubscriptions = std::move(render_pipeline_information._InputStreamSubscriptions);

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