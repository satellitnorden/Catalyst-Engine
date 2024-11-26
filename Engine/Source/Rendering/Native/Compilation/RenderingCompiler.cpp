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
#include <File/Core/File.h>
#include <File/Core/BinaryInputFile.h>
#include <File/Core/BinaryOutputFile.h>
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

//STL.
#include <filesystem>
#include <fstream>

//Constants.
#define ENGINE_RENDERING_DIRECTORY_PATH "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Rendering"
#define GAME_RENDERING_DIRECTORY_PATH "..\\..\\..\\Rendering"
#define GLOBAL_RENDER_DATA_FILE_PATH "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Rendering\\Global Render Data\\GlobalRenderData.global_render_data"

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
		BinaryInputFile file{ file_path };

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
		BinaryOutputFile file{ file_path };

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
*	Compute render target class definition.
*/
class ComputeRenderTarget final
{

public:

	//The name.
	DynamicString _Name;

	//The texture format.
	TextureFormat _TextureFormat;

};

/*
*	Input render target class definition.
*/
class InputRenderTarget final
{

public:

	//The name.
	DynamicString _Name;

	//The sample properties.
	SamplerProperties _SamplerProperties;

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

	//The compute render targets.
	DynamicArray<ComputeRenderTarget> _ComputeRenderTargets;

	//The input render targets.
	DynamicArray<InputRenderTarget> _InputRenderTargets;

	//The output render targets.
	DynamicArray<DynamicString> _OutputRenderTargets;

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

	//The hit group names.
	DynamicArray<DynamicString> _HitGroupNames;

	//The push constant data values.
	DynamicArray<PushConstantDataValue> _PushConstantDataValues;

	//The samplers.
	DynamicArray<Pair<DynamicString, SamplerProperties>> _Samplers;

	//The input stream subscriptions.
	DynamicArray<HashString> _InputStreamSubscriptions;

};

//Rendering compiler constants.
namespace RenderingCompilerConstants
{
	//TODO: Tie this into the ray tracing so there's only one definition of this.
	StaticArray<DynamicString, 2> HIT_GROUPS
	{
		"OpaqueModels",
		"MaskedModels"
	};
}

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

	//Try the game directory.
	{
		char buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(buffer, "%s\\Uniform Buffer Definitions\\%s.uniform_buffer_definition", GAME_RENDERING_DIRECTORY_PATH, name);

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
void FindShaderFunctionLibraryFilePath(const char *const RESTRICT name, DynamicString* const RESTRICT file_path) NOEXCEPT
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
*	Inserts ray tracing data into the given file.
*/
FORCE_INLINE void InsertRayTracingData(std::ofstream &output_file) NOEXCEPT
{
	uint32 current_resource_binding_index{ 0 };

	output_file << "layout (set = 2, binding = " << current_resource_binding_index++ << ") uniform accelerationStructureNV TOP_LEVEL_ACCELERATION_STRUCTURE;" << std::endl;

	for (const DynamicString &hit_group : RenderingCompilerConstants::HIT_GROUPS)
	{
		output_file << "layout (set = 2, binding = " << current_resource_binding_index++ << ") buffer " << hit_group.Data() << "_VERTEX_DATA_BUFFER { vec4 " << hit_group.Data() << "_VERTEX_DATA[]; } " << hit_group.Data() << "_VERTEX_BUFFERS[];" << std::endl;
		output_file << "layout (set = 2, binding = " << current_resource_binding_index++ << ") buffer " << hit_group.Data() << "_INDEX_DATA_BUFFER { uint " << hit_group.Data() << "_INDEX_DATA[]; } " << hit_group.Data() << "_INDEX_BUFFERS[];" << std::endl;
		output_file << "layout (set = 2, binding = " << current_resource_binding_index++ << ") buffer " << hit_group.Data() << "_MATERIAL_BUFFER { layout (offset = 0) uvec4[] " << hit_group.Data() << "_MATERIAL_INDICES; };" << std::endl;
	}
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
	//shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level::shaderc_optimization_level_performance);
	shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level::shaderc_optimization_level_zero);
	shaderc_compile_options_set_generate_debug_info(options);

	//Compile!
	shaderc_compilation_result_t result{ shaderc_compile_into_spv(compiler, shader_source.c_str(), shader_source.size(), shader_kind, file_path, "main", options) };

	//Check for errors.
	if (shaderc_result_get_num_errors(result) > 0)
	{
		LOG_ERROR(shaderc_result_get_error_message(result));
		ASSERT(false, "Error!");
	}

	//Retrieve the compiled file size.
	const uint64 compiled_file_size{ shaderc_result_get_length(result) };

	//Copy the data.
	data->Upsize<false>(compiled_file_size);
	Memory::Copy(data->Data(), shaderc_result_get_bytes(result), compiled_file_size);
}

/*
*	Inserts common things from the render pipeline information into a GLSL shader.
*/
FORCE_INLINE void InsertRenderPipelineInformationToGLSL(const RenderPipelineInformation &render_pipeline_information, std::ofstream &glsl_file) NOEXCEPT
{
	//Remember the current resource binding.
	uint32 resource_binding_index{ 0 };

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

	//Insert any compute render targets.
	if (!render_pipeline_information._ComputeRenderTargets.Empty())
	{
		for (const ComputeRenderTarget &compute_render_target : render_pipeline_information._ComputeRenderTargets)
		{
			const char *RESTRICT format_string{ nullptr };

			switch (compute_render_target._TextureFormat)
			{
				case TextureFormat::R_UINT8:
				{
					format_string = "r8";

					break;
				}

				case TextureFormat::RGBA_UINT8:
				{
					format_string = "rgba8";

					break;
				}

				case TextureFormat::RGBA_FLOAT32:
				{
					format_string = "rgba32f";

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			glsl_file << "layout (set = 1, binding = " << resource_binding_index++ << ", " << format_string << ") uniform image2D " << compute_render_target._Name.Data() << "; " << std::endl;
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

	//Write the input render targets.
	if (!render_pipeline_information._InputRenderTargets.Empty())
	{
		for (const InputRenderTarget &input_render_target : render_pipeline_information._InputRenderTargets)
		{
			glsl_file << "layout (set = 1, binding = " << resource_binding_index++ << ") uniform sampler2D " << input_render_target._Name.Data() << ";" << std::endl;
		}

		glsl_file << std::endl;
	}
}

/*
*	Inserts utility function for ray tracing hit shaders into the given file.
*/
FORCE_INLINE void InsertRayTracingHitShaderUtilityFunctions(const DynamicString &hit_group_name, std::ofstream &output_file) NOEXCEPT
{
	output_file << "hitAttributeNV vec3 HIT_ATTRIBUTE;" << std::endl;

	output_file << std::endl;

	output_file << "struct HitVertexInformation" << std::endl;
	output_file << "{" << std::endl;
	output_file << "\tvec3 _Position;" << std::endl;
	output_file << "\tvec3 _Normal;" << std::endl;
	output_file << "\tvec3 _Tangent;" << std::endl;
	output_file << "\tvec2 _TextureCoordinate;" << std::endl;
	output_file << "};" << std::endl;

	output_file << std::endl;

	output_file << "HitVertexInformation GetHitVertexInformation()" << std::endl;
	output_file << "{" << std::endl;
	output_file << "\tHitVertexInformation hit_vertex_information;" << std::endl;

	for (uint32 vertex_index{ 0 }; vertex_index < 3; ++vertex_index)
	{
		output_file << "\tuint vertex_index_" << vertex_index << " = " << hit_group_name.Data() << "_INDEX_BUFFERS[gl_InstanceCustomIndexNV]." << hit_group_name.Data() << "_INDEX_DATA[gl_PrimitiveID * 3 + " << vertex_index << "];" << std::endl;

		for (uint32 vertex_data_index{ 0 }; vertex_data_index < 3; ++vertex_data_index)
		{
			output_file << "\tvec4 vertex_data_" << vertex_index << "_" << vertex_data_index << " = " << hit_group_name.Data() << "_VERTEX_BUFFERS[gl_InstanceCustomIndexNV]." << hit_group_name.Data() << "_VERTEX_DATA[3 * vertex_index_" << vertex_index << " + " << vertex_data_index << "];" << std::endl;
		}
	}

	output_file << "\tvec3 barycentric_coordinates = vec3(1.0f - HIT_ATTRIBUTE.x - HIT_ATTRIBUTE.y, HIT_ATTRIBUTE.x, HIT_ATTRIBUTE.y);" << std::endl;

	output_file << "\thit_vertex_information._Position = ";

	for (uint32 vertex_index{ 0 }; vertex_index < 3; ++vertex_index)
	{
		output_file << "vec3(vertex_data_" << vertex_index << "_0.x, vertex_data_" << vertex_index << "_0.y, vertex_data_" << vertex_index << "_0.z) * barycentric_coordinates[" << vertex_index << "]";

		if (vertex_index < 2)
		{
			output_file << " + ";
		}

		else
		{
			output_file << ";" << std::endl;
		}
	}

	output_file << "\thit_vertex_information._Position = gl_ObjectToWorldNV * vec4(hit_vertex_information._Normal, 1.0f);" << std::endl;

	output_file << "\thit_vertex_information._Normal = ";

	for (uint32 vertex_index{ 0 }; vertex_index < 3; ++vertex_index)
	{
		output_file << "vec3(vertex_data_" << vertex_index << "_0.w, vertex_data_" << vertex_index << "_1.x, vertex_data_" << vertex_index << "_1.y) * barycentric_coordinates[" << vertex_index << "]";

		if (vertex_index < 2)
		{
			output_file << " + ";
		}

		else
		{
			output_file << ";" << std::endl;
		}
	}

	output_file << "\thit_vertex_information._Normal = normalize(gl_ObjectToWorldNV * vec4(hit_vertex_information._Normal, 0.0f));" << std::endl;

	output_file << "\thit_vertex_information._Tangent = ";

	for (uint32 vertex_index{ 0 }; vertex_index < 3; ++vertex_index)
	{
		output_file << "vec3(vertex_data_" << vertex_index << "_1.z, vertex_data_" << vertex_index << "_1.w, vertex_data_" << vertex_index << "_2.x) * barycentric_coordinates[" << vertex_index << "]";

		if (vertex_index < 2)
		{
			output_file << " + ";
		}

		else
		{
			output_file << ";" << std::endl;
		}
	}

	output_file << "\thit_vertex_information._Tangent = normalize(gl_ObjectToWorldNV * vec4(hit_vertex_information._Tangent, 0.0f));" << std::endl;

	output_file << "\thit_vertex_information._TextureCoordinate = ";

	for (uint32 vertex_index{ 0 }; vertex_index < 3; ++vertex_index)
	{
		output_file << "vec2(vertex_data_" << vertex_index << "_2.y, vertex_data_" << vertex_index << "_2.z) * barycentric_coordinates[" << vertex_index << "]";

		if (vertex_index < 2)
		{
			output_file << " + ";
		}

		else
		{
			output_file << ";" << std::endl;
		}
	}

	output_file << "\treturn hit_vertex_information;" << std::endl;
	output_file << "}" << std::endl;

	output_file << std::endl;

	output_file << "uint GetHitMaterialIndex()" << std::endl;
	output_file << "{" << std::endl;
	output_file << "\treturn " << hit_group_name.Data() << "_MATERIAL_INDICES[gl_InstanceCustomIndexNV / 4][gl_InstanceCustomIndexNV & 3];" << std::endl;
	output_file << "}" << std::endl;

	output_file << std::endl;
}

/*
*	Generates a compute shader.
*/
void GenerateComputeShader
(
	std::ifstream &file,
	const char *const RESTRICT generated_file_path,
	const std::string &shader_name,
	const RenderPipelineInformation &render_pipeline_information,
	RenderPipelineBuildParameters *const RESTRICT parameters
) NOEXCEPT
{
	/*
	*	Compute local size struct definition.
	*/
	struct ComputeLocalSize final
	{
		uint32 _Width;
		uint32 _Height;
		uint32 _Depth;
	};

	/*
	*	Compute shared memory struct definition.
	*/
	struct ComputeSharedMemory final
	{
		DynamicString _Type;
		DynamicString _Name;
	};

	//Gather all the lines in the compute function.
	DynamicArray<std::string> lines;
	GatherShaderLines(file, lines);

	//Retrieve the compute local size.
	ComputeLocalSize compute_local_size;

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t position{ lines[i].find("ComputeLocalSize(") };

		if (position != std::string::npos)
		{
			StaticArray<DynamicString, 3> arguments;

			TextParsingUtilities::ParseFunctionArguments
			(
				lines[i].data(),
				lines[i].length(),
				arguments.Data()
			);

			compute_local_size._Width = static_cast<uint32>(std::stoul(arguments[0].Data()));
			compute_local_size._Height = static_cast<uint32>(std::stoul(arguments[1].Data()));
			compute_local_size._Depth = static_cast<uint32>(std::stoul(arguments[2].Data()));

			lines.EraseAt<true>(i);
		}

		else
		{
			++i;
		}
	}

	//Retrieve the compute shared memory.
	DynamicArray<ComputeSharedMemory> compute_shared_memory;

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t position{ lines[i].find("ComputeSharedMemory(") };

		if (position != std::string::npos)
		{
			StaticArray<DynamicString, 2> arguments;

			TextParsingUtilities::ParseFunctionArguments
			(
				lines[i].data(),
				lines[i].length(),
				arguments.Data()
			);

			compute_shared_memory.Emplace();

			compute_shared_memory.Back()._Type = arguments[0];
			compute_shared_memory.Back()._Name = arguments[1];

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
		sprintf_s(glsl_file_path, "%s\\%s_Compute.glsl", generated_file_path, shader_name.c_str());
		std::ofstream glsl_file{ glsl_file_path };

		//Write the version declaration.
		glsl_file << "#version 460" << std::endl;

		glsl_file << std::endl;

		//Insert extensions.
		GLSLCompilation::InsertExtensions(glsl_file);

		//Insert the global render data.
		GLSLCompilation::InsertFromFile(glsl_file, GLOBAL_RENDER_DATA_FILE_PATH);

		//Insert things from the render pipeline information.
		InsertRenderPipelineInformationToGLSL(render_pipeline_information, glsl_file);

		//Write the local size.
		glsl_file << "layout (local_size_x = " << compute_local_size._Width << ", local_size_y = " << compute_local_size._Height << ", local_size_z = " << compute_local_size._Depth << ") in;" << std::endl;
		glsl_file << std::endl;

		//Write the shared memory.
		if (!compute_shared_memory.Empty())
		{
			for (const ComputeSharedMemory &_compute_shared_memory : compute_shared_memory)
			{
				glsl_file << "shared " << _compute_shared_memory._Type.Data() << " " << _compute_shared_memory._Name.Data() << "[" << compute_local_size._Width << "][" << compute_local_size._Height << "][" << compute_local_size._Depth << "];" << std::endl;
			}

			glsl_file << std::endl;
		}

		//Generate the "ComputeWorkGroupSize()" function.
		glsl_file << "uvec3 ComputeWorkGroupSize()" << std::endl;
		glsl_file << "{" << std::endl;
		glsl_file << "\treturn uvec3(" << compute_local_size._Width << ", " << compute_local_size._Height << ", " << compute_local_size._Depth << ");" << std::endl;
		glsl_file << "}" << std::endl;
		glsl_file << std::endl;

		//Generate the "ComputeDimensions()" function.
		glsl_file << "uvec3 ComputeDimensions()" << std::endl;
		glsl_file << "{" << std::endl;
		glsl_file << "\treturn ComputeWorkGroupSize() * gl_NumWorkGroups;" << std::endl;
		glsl_file << "}" << std::endl;
		glsl_file << std::endl;

		//Write the "void main()" line.
		glsl_file << "void main()" << std::endl;

		//Write the remaining lines.
		for (uint64 i{ 0 }; i < glsl_lines.Size(); ++i)
		{
			//Cache the line.
			std::string& line{ glsl_lines[i] };

			//Replace "COMPUTE_GLOBAL_ID" with "gl_GlobalInvocationID".
			{
				size_t position{ line.find("COMPUTE_GLOBAL_ID") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("COMPUTE_GLOBAL_ID"), "gl_GlobalInvocationID");
					position = line.find("COMPUTE_GLOBAL_ID");
				}
			}

			//Replace "COMPUTE_LOCAL_ID" with "gl_LocalInvocationID".
			{
				size_t position{ line.find("COMPUTE_LOCAL_ID") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("COMPUTE_LOCAL_ID"), "gl_LocalInvocationID");
					position = line.find("COMPUTE_LOCAL_ID");
				}
			}

			//Replace "ImageLoad" with "imageLoad".
			{
				size_t position{ line.find("ImageLoad") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("ImageLoad"), "imageLoad");
					position = line.find("ImageLoad");
				}
			}

			//Replace "ImageStore" with "imageStore".
			{
				size_t position{ line.find("ImageStore") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("ImageStore"), "imageStore");
					position = line.find("ImageStore");
				}
			}

			//Replace "ComputeBarrier" with "barrier".
			{
				size_t position{ line.find("ComputeBarrier") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("ComputeBarrier"), "barrier");
					position = line.find("ComputeBarrier");
				}
			}

			//Write the line.
			glsl_file << line << std::endl;
		}

		//Close the file.
		glsl_file.close();

		//Compile the GLSL shader.
		CompileGLSLShader(glsl_file_path, shaderc_shader_kind::shaderc_compute_shader, &parameters->_ComputeShaderData._GLSLData);
	}
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

		//Insert things from the render pipeline information.
		InsertRenderPipelineInformationToGLSL(render_pipeline_information, glsl_file);

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

				//Replace "INSTANCE_INDEX" with "gl_InstanceIndex".
				{
					size_t position{ line.find("INSTANCE_INDEX") };

					while (position != std::string::npos)
					{
						line.replace(position, strlen("INSTANCE_INDEX"), "gl_InstanceIndex");
						position = line.find("INSTANCE_INDEX");
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

		//Insert things from the render pipeline information.
		InsertRenderPipelineInformationToGLSL(render_pipeline_information, glsl_file);

		//Write the input parameters.
		if (!input_parameters.Empty())
		{
			//Remember the current location index.
			uint32 location_index{ 0 };

			for (const InputParameter &input_parameter : input_parameters)
			{
				bool flat{ false };

				if (input_parameter._Type == "uint")
				{
					flat = true;
				}

				glsl_file << "layout (location = " << location_index << ") " << (flat ? "flat " : "") << "in " << input_parameter._Type.Data() << " " << input_parameter._Name.Data() << ";" << std::endl;
			
				location_index += GLSLCompilation::GetLocationOffsetForType(input_parameter._Type.Data());
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
*	Generates a ray generation shader.
*/
void GenerateRayGenerationShader
(
	std::ifstream &file,
	const char *const RESTRICT generated_file_path,
	const std::string &shader_name,
	const RenderPipelineInformation &render_pipeline_information,
	RenderPipelineBuildParameters *const RESTRICT parameters
) NOEXCEPT
{
	//Struct definitions.
	struct Payload final
	{
		uint32 _Index;
		DynamicString _Type;
		DynamicString _Name;
	};

	//Gather all the lines in the vertex function.
	DynamicArray<std::string> lines;
	GatherShaderLines(file, lines);

	//Gather the payloads.
	DynamicArray<Payload> payloads;

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t position{ lines[i].find("Payload(") };

		if (position != std::string::npos)
		{
			StaticArray<DynamicString, 3> arguments;

			const uint64 number_of_arguments
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					lines[i].data(),
					lines[i].length(),
					arguments.Data()
				)
			};

			ASSERT(number_of_arguments == 3, "Payload() requires three arguments!");

			payloads.Emplace();
			payloads.Back()._Index = static_cast<uint32>(std::stoull(arguments[0].Data()));
			payloads.Back()._Type = std::move(arguments[1]);
			payloads.Back()._Name = std::move(arguments[2]);

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
		sprintf_s(glsl_file_path, "%s\\%s_RayGeneration.glsl", generated_file_path, shader_name.c_str());
		std::ofstream glsl_file{ glsl_file_path };

		//Write the version declaration.
		glsl_file << "#version 460" << std::endl;

		glsl_file << std::endl;

		//Insert extensions.
		GLSLCompilation::InsertExtensions(glsl_file);
		GLSLCompilation::InsertRayTracingExtensions(glsl_file);

		//Insert the global render data.
		GLSLCompilation::InsertFromFile(glsl_file, GLOBAL_RENDER_DATA_FILE_PATH);

		//Insert things from the render pipeline information.
		InsertRenderPipelineInformationToGLSL(render_pipeline_information, glsl_file);

		//Insert ray tracing data.
		InsertRayTracingData(glsl_file);

		//Write the payloads.
		if (!payloads.Empty())
		{
			for (const Payload &payload : payloads)
			{
				glsl_file << "layout (location = " << payload._Index << ") rayPayloadNV " << payload._Type.Data() << " " << payload._Name.Data() << ";" << std::endl;
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

			//Replace "RAY_TRACING_ID" with "gl_LaunchIDNV".
			{
				size_t position{ line.find("RAY_TRACING_ID") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("RAY_TRACING_ID"), "gl_LaunchIDNV");
					position = line.find("RAY_TRACING_ID");
				}
			}

			//Replace "RAY_TRACING_SIZE" with "gl_LaunchSizeNV".
			{
				size_t position{ line.find("RAY_TRACING_SIZE") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("RAY_TRACING_SIZE"), "gl_LaunchSizeNV");
					position = line.find("RAY_TRACING_SIZE");
				}
			}

			//Replace "RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT" with "gl_RayFlagsTerminateOnFirstHitNV".
			{
				size_t position{ line.find("RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT"), "gl_RayFlagsTerminateOnFirstHitNV");
					position = line.find("RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT");
				}
			}

			//Replace "RAY_TRACING_FLAG_OPAQUE" with "gl_RayFlagsOpaqueNV".
			{
				size_t position{ line.find("RAY_TRACING_FLAG_OPAQUE") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("RAY_TRACING_FLAG_OPAQUE"), "gl_RayFlagsOpaqueNV");
					position = line.find("RAY_TRACING_FLAG_OPAQUE");
				}
			}

			//Replace "RAY_TRACING_FLAG_SKIP_CLOSEST_HIT" with "gl_RayFlagsSkipClosestHitShaderNV".
			{
				size_t position{ line.find("RAY_TRACING_FLAG_SKIP_CLOSEST_HIT") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("RAY_TRACING_FLAG_SKIP_CLOSEST_HIT"), "gl_RayFlagsSkipClosestHitShaderNV");
					position = line.find("RAY_TRACING_FLAG_SKIP_CLOSEST_HIT");
				}
			}

			//Replace "ImageLoad" with "imageLoad".
			{
				size_t position{ line.find("ImageLoad") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("ImageLoad"), "imageLoad");
					position = line.find("ImageLoad");
				}
			}

			//Replace "ImageStore" with "imageStore".
			{
				size_t position{ line.find("ImageStore") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("ImageStore"), "imageStore");
					position = line.find("ImageStore");
				}
			}

			//Process "TraceRay()" calls
			{
				size_t position{ line.find("TraceRay(") };

				if (position != std::string::npos)
				{
					StaticArray<DynamicString, 6> arguments;

					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							line.data(),
							line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 6, "TraceRay needs 6 arguments!");

					line = "traceNV\n";
					line += "(\n";
					line += "\tTOP_LEVEL_ACCELERATION_STRUCTURE, /*topLevel*/\n";
					line += std::string("\t") + std::string(arguments[0].Data()) + ", /*rayFlags*/\n";
					line += "\t0xff, /*cullMask*/\n";
					line += "\t0, /*sbtRecordOffset*/\n";
					line += "\t0, /*sbtRecordStride*/\n";
					line += std::string("\t") + std::string(arguments[1].Data()) + ", /*missIndex*/\n";
					line += std::string("\t") + std::string(arguments[2].Data()) + ", /*origin*/\n";
					line += "\tFLOAT32_EPSILON * 8.0f, /*Tmin*/\n";
					line += std::string("\t") + std::string(arguments[3].Data()) + ", /*direction*/\n";
					line += std::string("\t") + std::string(arguments[4].Data()) + ", /*Tmax*/\n";
					line += std::string("\t") + std::string(arguments[5].Data()) + " /*payload*/\n";
					line += ");";
				}
			}

			//Write the line.
			glsl_file << line << std::endl;
		}

		//Close the file.
		glsl_file.close();

		//Compile the GLSL shader.
		CompileGLSLShader(glsl_file_path, shaderc_shader_kind::shaderc_raygen_shader, &parameters->_RayGenerationShaderData._GLSLData);
	}
}

/*
*	Generates a ray miss shader.
*/
void GenerateRayMissShader
(
	std::ifstream &file,
	const char *const RESTRICT generated_file_path,
	const std::string &shader_name,
	const RenderPipelineInformation &render_pipeline_information,
	RenderPipelineBuildParameters *const RESTRICT parameters
) NOEXCEPT
{
	//Struct definitions.
	struct Payload final
	{
		uint32 _Index;
		DynamicString _Type;
		DynamicString _Name;
	};

	//Gather all the lines in the vertex function.
	DynamicArray<std::string> lines;
	GatherShaderLines(file, lines);

	//Gather the payloads.
	DynamicArray<Payload> payloads;

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t position{ lines[i].find("Payload(") };

		if (position != std::string::npos)
		{
			StaticArray<DynamicString, 3> arguments;

			const uint64 number_of_arguments
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					lines[i].data(),
					lines[i].length(),
					arguments.Data()
				)
			};

			ASSERT(number_of_arguments == 3, "Payload() requires three arguments!");

			payloads.Emplace();
			payloads.Back()._Index = static_cast<uint32>(std::stoull(arguments[0].Data()));
			payloads.Back()._Type = std::move(arguments[1]);
			payloads.Back()._Name = std::move(arguments[2]);

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
		sprintf_s(glsl_file_path, "%s\\%s_RayMiss.glsl", generated_file_path, shader_name.c_str());
		std::ofstream glsl_file{ glsl_file_path };

		//Write the version declaration.
		glsl_file << "#version 460" << std::endl;

		glsl_file << std::endl;

		//Insert extensions.
		GLSLCompilation::InsertExtensions(glsl_file);
		GLSLCompilation::InsertRayTracingExtensions(glsl_file);

		//Insert the global render data.
		GLSLCompilation::InsertFromFile(glsl_file, GLOBAL_RENDER_DATA_FILE_PATH);

		//Insert things from the render pipeline information.
		InsertRenderPipelineInformationToGLSL(render_pipeline_information, glsl_file);

		//Insert ray tracing data.
		InsertRayTracingData(glsl_file);

		//Write the payloads.
		if (!payloads.Empty())
		{
			for (const Payload &payload : payloads)
			{
				glsl_file << "layout (location = " << payload._Index << ") rayPayloadInNV " << payload._Type.Data() << " " << payload._Name.Data() << ";" << std::endl;
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

			//Replace "WORLD_RAY_ORIGIN" with "gl_WorldRayOriginNV".
			{
				size_t position{ line.find("WORLD_RAY_ORIGIN") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("WORLD_RAY_ORIGIN"), "gl_WorldRayOriginNV");
					position = line.find("WORLD_RAY_ORIGIN");
				}
			}

			//Replace "WORLD_RAY_DIRECTION" with "gl_WorldRayDirectionNV".
			{
				size_t position{ line.find("WORLD_RAY_DIRECTION") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("WORLD_RAY_DIRECTION"), "gl_WorldRayDirectionNV");
					position = line.find("WORLD_RAY_DIRECTION");
				}
			}

			//Replace "RAY_HIT_DISTANCE" with "gl_HitTNV".
			{
				size_t position{ line.find("RAY_HIT_DISTANCE") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("RAY_HIT_DISTANCE"), "gl_HitTNV");
					position = line.find("RAY_HIT_DISTANCE");
				}
			}

			//Write the line.
			glsl_file << line << std::endl;
		}

		//Close the file.
		glsl_file.close();

		//Compile the GLSL shader.
		parameters->_RayMissShaderData.Emplace();
		CompileGLSLShader(glsl_file_path, shaderc_shader_kind::shaderc_miss_shader, &parameters->_RayMissShaderData.Back()._GLSLData);
	}
}

/*
*	Generates a ray closest hit shader.
*/
void GenerateRayClosestHitShader
(
	std::ifstream &file,
	const char *const RESTRICT generated_file_path,
	const std::string &shader_name,
	const RenderPipelineInformation &render_pipeline_information,
	RenderPipelineBuildParameters *const RESTRICT parameters
) NOEXCEPT
{
	//Struct definitions.
	struct Payload final
	{
		uint32 _Index;
		DynamicString _Type;
		DynamicString _Name;
	};

	//Gather all the lines in the vertex function.
	DynamicArray<std::string> lines;
	GatherShaderLines(file, lines);

	//Gather the payloads.
	DynamicArray<Payload> incoming_payloads;
	DynamicArray<Payload> outgoing_payloads;

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t position{ lines[i].find("IncomingPayload(") };

		if (position != std::string::npos)
		{
			StaticArray<DynamicString, 3> arguments;

			const uint64 number_of_arguments
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					lines[i].data(),
					lines[i].length(),
					arguments.Data()
				)
			};

			ASSERT(number_of_arguments == 3, "Payload() requires three arguments!");

			incoming_payloads.Emplace();
			incoming_payloads.Back()._Index = static_cast<uint32>(std::stoull(arguments[0].Data()));
			incoming_payloads.Back()._Type = std::move(arguments[1]);
			incoming_payloads.Back()._Name = std::move(arguments[2]);

			lines.EraseAt<true>(i);
		}

		else
		{
			++i;
		}
	}

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t position{ lines[i].find("OutgoingPayload(") };

		if (position != std::string::npos)
		{
			StaticArray<DynamicString, 3> arguments;

			const uint64 number_of_arguments
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					lines[i].data(),
					lines[i].length(),
					arguments.Data()
				)
			};

			ASSERT(number_of_arguments == 3, "Payload() requires three arguments!");

			outgoing_payloads.Emplace();
			outgoing_payloads.Back()._Index = static_cast<uint32>(std::stoull(arguments[0].Data()));
			outgoing_payloads.Back()._Type = std::move(arguments[1]);
			outgoing_payloads.Back()._Name = std::move(arguments[2]);

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
		sprintf_s(glsl_file_path, "%s\\%s_RayClosestHit.glsl", generated_file_path, shader_name.c_str());
		std::ofstream glsl_file{ glsl_file_path };

		//Write the version declaration.
		glsl_file << "#version 460" << std::endl;

		glsl_file << std::endl;

		//Insert extensions.
		GLSLCompilation::InsertExtensions(glsl_file);
		GLSLCompilation::InsertRayTracingExtensions(glsl_file);

		//Insert the global render data.
		GLSLCompilation::InsertFromFile(glsl_file, GLOBAL_RENDER_DATA_FILE_PATH);

		//Insert things from the render pipeline information.
		InsertRenderPipelineInformationToGLSL(render_pipeline_information, glsl_file);

		//Insert ray tracing data.
		InsertRayTracingData(glsl_file);

		//Insert the ray traing hit shader utility functions.
		InsertRayTracingHitShaderUtilityFunctions(render_pipeline_information._HitGroupNames.Back(), glsl_file);

		//Write the payloads.
		if (!incoming_payloads.Empty())
		{
			for (const Payload &payload : incoming_payloads)
			{
				glsl_file << "layout (location = " << payload._Index << ") rayPayloadInNV " << payload._Type.Data() << " " << payload._Name.Data() << "; " << std::endl;
			}

			glsl_file << std::endl;
		}

		if (!outgoing_payloads.Empty())
		{
			for (const Payload &payload : outgoing_payloads)
			{
				glsl_file << "layout (location = " << payload._Index << ") rayPayloadNV " << payload._Type.Data() << " " << payload._Name.Data() << "; " << std::endl;
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

			//Replace "RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT" with "gl_RayFlagsTerminateOnFirstHitNV".
			{
				size_t position{ line.find("RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT"), "gl_RayFlagsTerminateOnFirstHitNV");
					position = line.find("RAY_TRACING_FLAG_TERMINATE_ON_FIRST_HIT");
				}
			}

			//Replace "RAY_TRACING_FLAG_SKIP_CLOSEST_HIT" with "gl_RayFlagsSkipClosestHitShaderNV".
			{
				size_t position{ line.find("RAY_TRACING_FLAG_SKIP_CLOSEST_HIT") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("RAY_TRACING_FLAG_SKIP_CLOSEST_HIT"), "gl_RayFlagsSkipClosestHitShaderNV");
					position = line.find("RAY_TRACING_FLAG_SKIP_CLOSEST_HIT");
				}
			}

			//Replace "WORLD_RAY_ORIGIN" with "gl_WorldRayOriginNV".
			{
				size_t position{ line.find("WORLD_RAY_ORIGIN") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("WORLD_RAY_ORIGIN"), "gl_WorldRayOriginNV");
					position = line.find("WORLD_RAY_ORIGIN");
				}
			}

			//Replace "WORLD_RAY_DIRECTION" with "gl_WorldRayDirectionNV".
			{
				size_t position{ line.find("WORLD_RAY_DIRECTION") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("WORLD_RAY_DIRECTION"), "gl_WorldRayDirectionNV");
					position = line.find("WORLD_RAY_DIRECTION");
				}
			}

			//Replace "RAY_HIT_DISTANCE" with "gl_HitTNV".
			{
				size_t position{ line.find("RAY_HIT_DISTANCE") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("RAY_HIT_DISTANCE"), "gl_HitTNV");
					position = line.find("RAY_HIT_DISTANCE");
				}
			}

			//Process "TraceRay()" calls
			{
				size_t position{ line.find("TraceRay(") };

				if (position != std::string::npos)
				{
					StaticArray<DynamicString, 6> arguments;

					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							line.data(),
							line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 6, "TraceRay needs 6 arguments!");

					line = "traceNV\n";
					line += "(\n";
					line += "\tTOP_LEVEL_ACCELERATION_STRUCTURE, /*topLevel*/\n";
					line += std::string("\t") + std::string(arguments[0].Data()) + ", /*rayFlags*/\n";
					line += "\t0xff, /*cullMask*/\n";
					line += "\t0, /*sbtRecordOffset*/\n";
					line += "\t0, /*sbtRecordStride*/\n";
					line += std::string("\t") + std::string(arguments[1].Data()) + ", /*missIndex*/\n";
					line += std::string("\t") + std::string(arguments[2].Data()) + ", /*origin*/\n";
					line += "\tFLOAT32_EPSILON * 8.0f, /*Tmin*/\n";
					line += std::string("\t") + std::string(arguments[3].Data()) + ", /*direction*/\n";
					line += std::string("\t") + std::string(arguments[4].Data()) + ", /*Tmax*/\n";
					line += std::string("\t") + std::string(arguments[5].Data()) + " /*payload*/\n";
					line += ");";
				}
			}

			//Write the line.
			glsl_file << line << std::endl;
		}

		//Close the file.
		glsl_file.close();

		//Compile the GLSL shader.
		CompileGLSLShader(glsl_file_path, shaderc_shader_kind::shaderc_closesthit_shader, &parameters->_RayHitGroupShaderData.Back()._RayClosestHitShaderData._GLSLData);
	}
}

/*
*	Generates a ray any hit shader.
*/
void GenerateRayAnyHitShader
(
	std::ifstream &file,
	const char *const RESTRICT generated_file_path,
	const std::string &shader_name,
	const RenderPipelineInformation &render_pipeline_information,
	RenderPipelineBuildParameters *const RESTRICT parameters
) NOEXCEPT
{
	//Struct definitions.
	struct Payload final
	{
		uint32 _Index;
		DynamicString _Type;
		DynamicString _Name;
	};

	//Gather all the lines in the vertex function.
	DynamicArray<std::string> lines;
	GatherShaderLines(file, lines);

	//Gather the payloads.
	DynamicArray<Payload> payloads;

	for (uint64 i{ 0 }; i < lines.Size();)
	{
		const size_t position{ lines[i].find("Payload(") };

		if (position != std::string::npos)
		{
			StaticArray<DynamicString, 3> arguments;

			const uint64 number_of_arguments
			{
				TextParsingUtilities::ParseFunctionArguments
				(
					lines[i].data(),
					lines[i].length(),
					arguments.Data()
				)
			};

			ASSERT(number_of_arguments == 3, "Payload() requires three arguments!");

			payloads.Emplace();
			payloads.Back()._Index = static_cast<uint32>(std::stoull(arguments[0].Data()));
			payloads.Back()._Type = std::move(arguments[1]);
			payloads.Back()._Name = std::move(arguments[2]);

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
		sprintf_s(glsl_file_path, "%s\\%s_RayAnyHit.glsl", generated_file_path, shader_name.c_str());
		std::ofstream glsl_file{ glsl_file_path };

		//Write the version declaration.
		glsl_file << "#version 460" << std::endl;

		glsl_file << std::endl;

		//Insert extensions.
		GLSLCompilation::InsertExtensions(glsl_file);
		GLSLCompilation::InsertRayTracingExtensions(glsl_file);

		//Insert the global render data.
		GLSLCompilation::InsertFromFile(glsl_file, GLOBAL_RENDER_DATA_FILE_PATH);

		//Insert things from the render pipeline information.
		InsertRenderPipelineInformationToGLSL(render_pipeline_information, glsl_file);

		//Insert ray tracing data.
		InsertRayTracingData(glsl_file);

		//Insert the ray traing hit shader utility functions.
		InsertRayTracingHitShaderUtilityFunctions(render_pipeline_information._HitGroupNames.Back(), glsl_file);

		//Write the payloads.
		if (!payloads.Empty())
		{
			for (const Payload &payload : payloads)
			{
				glsl_file << "layout (location = " << payload._Index << ") rayPayloadInNV " << payload._Type.Data() << " " << payload._Name.Data() << ";" << std::endl;
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

			//Replace "WORLD_RAY_ORIGIN" with "gl_WorldRayOriginNV".
			{
				size_t position{ line.find("WORLD_RAY_ORIGIN") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("WORLD_RAY_ORIGIN"), "gl_WorldRayOriginNV");
					position = line.find("WORLD_RAY_ORIGIN");
				}
			}

			//Replace "WORLD_RAY_DIRECTION" with "gl_WorldRayDirectionNV".
			{
				size_t position{ line.find("WORLD_RAY_DIRECTION") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("WORLD_RAY_DIRECTION"), "gl_WorldRayDirectionNV");
					position = line.find("WORLD_RAY_DIRECTION");
				}
			}

			//Replace "RAY_HIT_DISTANCE" with "gl_HitTNV".
			{
				size_t position{ line.find("RAY_HIT_DISTANCE") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("RAY_HIT_DISTANCE"), "gl_HitTNV");
					position = line.find("RAY_HIT_DISTANCE");
				}
			}

			//Replace "IgnoreHit()" with "ignoreIntersectionNV()".
			{
				size_t position{ line.find("IgnoreHit()") };

				while (position != std::string::npos)
				{
					line.replace(position, strlen("IgnoreHit()"), "ignoreIntersectionNV()");
					position = line.find("IgnoreHit()");
				}
			}

			//Write the line.
			glsl_file << line << std::endl;
		}

		//Close the file.
		glsl_file.close();

		//Compile the GLSL shader.
		CompileGLSLShader(glsl_file_path, shaderc_shader_kind::shaderc_anyhit_shader, &parameters->_RayHitGroupShaderData.Back()._RayAnyHitShaderData._GLSLData);
	}
}

/*
*	Runs the shader compiler.
*	Returns if new rendering data was compiled.
*/
NO_DISCARD bool RenderingCompiler::Run() NOEXCEPT
{
	//Cache the start time.
	const TimePoint start_time;

	//Remember if new rendering data was compiled.
	bool new_rendering_data_compiled{ false };

	//Parse render pipelines.
	{
		new_rendering_data_compiled |= ParseRenderPipelinesInDirectory(false, ENGINE_RENDERING_DIRECTORY_PATH "\\Render Pipelines");
		new_rendering_data_compiled |= ParseRenderPipelinesInDirectory(true, GAME_RENDERING_DIRECTORY_PATH "\\Render Pipelines");
	}

	LOG_INFORMATION("Rendering Compiler took %f seconds.", start_time.GetSecondsSince());

	return new_rendering_data_compiled;
}

/*
*	Parses render pipelines in the given directory.
*	Returns if new rendering data was compiled.
*/
NO_DISCARD bool RenderingCompiler::ParseRenderPipelinesInDirectory(const bool is_game, const char *const RESTRICT directory_path) NOEXCEPT
{
	//Skip if the directory doesn't exist.
	if (!std::filesystem::exists(std::string(directory_path)))
	{
		return false;
	}

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

		if (is_game)
		{
			sprintf_s(output_buffer, "..\\..\\..\\Content\\Intermediate\\Render Pipelines\\%s_RenderPipeline", render_pipeline_name.data());
		}

		else
		{
			sprintf_s(output_buffer, "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Intermediate\\Base\\Render Pipelines\\%s_RenderPipeline", render_pipeline_name.data());
		}

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

			//Is this a compute render target declaration?
			{
				const size_t input_render_target_position{ current_line.find("ComputeRenderTarget(") };

				if (input_render_target_position != std::string::npos)
				{
					StaticArray<DynamicString, 2> arguments;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						arguments.Data()
					);

					render_pipeline_information._ComputeRenderTargets.Emplace();

					render_pipeline_information._ComputeRenderTargets.Back()._Name = arguments[0];

					if (arguments[1] == "R_UINT8")
					{
						render_pipeline_information._ComputeRenderTargets.Back()._TextureFormat = TextureFormat::R_UINT8;
					}

					else if (arguments[1] == "RGB_UINT8")
					{
						render_pipeline_information._ComputeRenderTargets.Back()._TextureFormat = TextureFormat::RGB_UINT8;
					}

					else if (arguments[1] == "RGBA_UINT8")
					{
						render_pipeline_information._ComputeRenderTargets.Back()._TextureFormat = TextureFormat::RGBA_UINT8;
					}

					else if (arguments[1] == "RG_FLOAT16")
					{
						render_pipeline_information._ComputeRenderTargets.Back()._TextureFormat = TextureFormat::RG_FLOAT16;
					}

					else if (arguments[1] == "R_FLOAT32")
					{
						render_pipeline_information._ComputeRenderTargets.Back()._TextureFormat = TextureFormat::R_FLOAT32;
					}

					else if (arguments[1] == "RG_FLOAT32")
					{
						render_pipeline_information._ComputeRenderTargets.Back()._TextureFormat = TextureFormat::RG_FLOAT32;
					}

					else if (arguments[1] == "RGBA_FLOAT32")
					{
						render_pipeline_information._ComputeRenderTargets.Back()._TextureFormat = TextureFormat::RGBA_FLOAT32;
					}

					else
					{
						ASSERT(false, "Unknown argument %s", arguments[1].Data());
					}

					continue;
				}
			}

			//Is this an input render target declaration?
			{
				const size_t input_render_target_position{ current_line.find("InputRenderTarget") };

				if (input_render_target_position != std::string::npos)
				{
					StaticArray<DynamicString, 4> arguments;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						arguments.Data()
					);

					render_pipeline_information._InputRenderTargets.Emplace();

					InputRenderTarget &new_input_render_target{ render_pipeline_information._InputRenderTargets.Back() };

					new_input_render_target._Name = arguments[0];

					if (arguments[1] == "LINEAR")
					{
						new_input_render_target._SamplerProperties._MagnificationFilter = TextureFilter::LINEAR;
					}

					else if (arguments[1] == "NEAREST")
					{
						new_input_render_target._SamplerProperties._MagnificationFilter = TextureFilter::NEAREST;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					if (arguments[2] == "LINEAR")
					{
						new_input_render_target._SamplerProperties._MipmapMode = MipmapMode::LINEAR;
					}

					else if (arguments[2] == "NEAREST")
					{
						new_input_render_target._SamplerProperties._MipmapMode = MipmapMode::NEAREST;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					if (arguments[3] == "CLAMP_TO_BORDER")
					{
						new_input_render_target._SamplerProperties._AddressMode = AddressMode::CLAMP_TO_BORDER;
					}

					else if (arguments[3] == "CLAMP_TO_EDGE")
					{
						new_input_render_target._SamplerProperties._AddressMode = AddressMode::CLAMP_TO_EDGE;
					}

					else if (arguments[3] == "MIRROR_CLAMP_TO_EDGE")
					{
						new_input_render_target._SamplerProperties._AddressMode = AddressMode::MIRROR_CLAMP_TO_EDGE;
					}

					else if (arguments[3] == "MIRRORED_REPEAT")
					{
						new_input_render_target._SamplerProperties._AddressMode = AddressMode::MIRRORED_REPEAT;
					}

					else if (arguments[3] == "REPEAT")
					{
						new_input_render_target._SamplerProperties._AddressMode = AddressMode::REPEAT;
					}

					else
					{
						ASSERT(false, "Invalid argument!");
					}

					new_input_render_target._SamplerProperties._AnisotropicSamples = 0;

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

					else if (string == "GREATER_OR_EQUAL")
					{
						render_pipeline_information._DepthCompareOperator = CompareOperator::GreaterOrEqual;
					}

					else if (string == "LESS")
					{
						render_pipeline_information._DepthCompareOperator = CompareOperator::Less;
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

			//Is this a common ray closest hit shader include?
			{
				const size_t position{ current_line.find("IncludeCommonRayClosestHitShader(") };

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

					GenerateRayClosestHitShader(common_vertex_shader_file, generated_file_path, render_pipeline_name, render_pipeline_information, &parameters);

					continue;
				}
			}

			//Is this the beginning of a new hit group?
			{
				const size_t position{ current_line.find("HitGroup(") };

				if (position != std::string::npos)
				{
					//Parse the argument.
					DynamicString argument;

					TextParsingUtilities::ParseFunctionArguments
					(
						current_line.data(),
						current_line.length(),
						&argument
					);

					//Add the new hit group.
					render_pipeline_information._HitGroupNames.Emplace(argument);
					parameters._RayHitGroupShaderData.Emplace();
					parameters._RayHitGroupShaderData.Back()._Identifier = HashString(argument.Data());

					continue;
				}
			}

			//Is this the beginning of a compute shader?
			if (current_line == "Compute")
			{
				GenerateComputeShader(file, generated_file_path, render_pipeline_name, render_pipeline_information, &parameters);
			}
			
			//Is this the beginning of a vertex shader?
			else if (current_line == "Vertex")
			{
				GenerateVertexShader(file, generated_file_path, render_pipeline_name, render_pipeline_information, &parameters);
			}

			//Is this the beginning of a fragment shader?
			else if (current_line == "Fragment")
			{
				GenerateFragmentShader(file, generated_file_path, render_pipeline_name, render_pipeline_information, &parameters);
			}

			//Is this the beginning of a ray generation shader?
			else if (current_line == "RayGeneration")
			{
				GenerateRayGenerationShader(file, generated_file_path, render_pipeline_name, render_pipeline_information, &parameters);
			}

			//Is this the beginning of a ray miss shader?
			else if (current_line == "RayMiss")
			{
				GenerateRayMissShader(file, generated_file_path, render_pipeline_name, render_pipeline_information, &parameters);
			}

			//Is this the beginning of a ray closest hit shader?
			else if (current_line == "RayClosestHit")
			{
				GenerateRayClosestHitShader(file, generated_file_path, render_pipeline_name, render_pipeline_information, &parameters);
			}

			//Is this the beginning of a ray any hit shader?
			else if (current_line == "RayAnyHit")
			{
				GenerateRayAnyHitShader(file, generated_file_path, render_pipeline_name, render_pipeline_information, &parameters);
			}

			//Ignore lines that are consumed by code generation.
			else if (	current_line.find("OutputDepthBuffer(") != std::string::npos
						|| current_line.find("RenderResolution(") != std::string::npos
						|| current_line.find("ColorLoadOperator(") != std::string::npos
						|| current_line.find("ColorStoreOperator(") != std::string::npos
						|| current_line.find("DepthStencilLoadOperator(") != std::string::npos
						|| current_line.find("DepthStencilStoreOperator(") != std::string::npos
						|| current_line.find("CullMode(") != std::string::npos
						|| current_line.find("BlendEnable(") != std::string::npos
						|| current_line.find("BlendColorSourceFactor(") != std::string::npos
						|| current_line.find("BlendColorDestinationFactor(") != std::string::npos
						|| current_line.find("BlendColorOperator(") != std::string::npos
					|| current_line.find("BlendAlphaSourceFactor(") != std::string::npos
					|| current_line.find("BlendAlphaDestinationFactor(") != std::string::npos
					|| current_line.find("BlendAlphaOperator(") != std::string::npos)
			{
				continue;
			}

			else
			{
				ASSERT(false, "Unknown line %s", current_line.data());
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

		//Fill in the compute render targets.
		if (!render_pipeline_information._ComputeRenderTargets.Empty())
		{
			parameters._ComputeRenderTargets.Reserve(render_pipeline_information._ComputeRenderTargets.Size());

			for (const ComputeRenderTarget &compute_render_target : render_pipeline_information._ComputeRenderTargets)
			{
				parameters._ComputeRenderTargets.Emplace(HashString(compute_render_target._Name.Data()));
			}
		}

		//Fill in the input render targets.
		if (!render_pipeline_information._InputRenderTargets.Empty())
		{
			parameters._InputRenderTargets.Reserve(render_pipeline_information._InputRenderTargets.Size());

			for (const InputRenderTarget &input_render_target : render_pipeline_information._InputRenderTargets)
			{
				parameters._InputRenderTargets.Emplace(HashString(input_render_target._Name.Data()), input_render_target._SamplerProperties);
			}
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