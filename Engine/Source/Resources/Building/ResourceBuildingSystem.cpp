#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
//Header file.
#include <Resources/Building/ResourceBuildingSystem.h>

//Core.
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//Animation.
#include <Animation/AnimatedVertex.h>

//Concurrency.
#include <Concurrency/Task.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryOutputFile.h>
#include <File/Readers/FBXReader.h>
#include <File/Readers/JPGReader.h>
#include <File/Readers/PNGReader.h>
#include <File/Readers/WAVReader.h>
#include <File/Writers/PNGWriter.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Noise/HaltonSequence.h>
#include <Math/Noise/HammersleySequence.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/TextureCube.h>
#include <Rendering/Native/Vertex.h>

//Resources
#include <Resources/Core/ResourcesCore.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/TaskSystem.h>

//Third party.
#include <ThirdParty/stb_image.h>
#include <ThirdParty/stb_image_resize.h>
#include <ThirdParty/stb_truetype/stb_truetype.h>
#include <ThirdParty/vulkan/shaderc/shaderc.h>

//STL.
#include <filesystem>

/*
*	Builds the resource collection recursively.
*/
void BuildResourceCollectionsRecursive(	const ResourceCollectionBuildParameters &parameters,
										const char *const RESTRICT directory_path,
										BinaryOutputFile *RESTRICT *RESTRICT current_file,
										uint64 *const RESTRICT file_counter,
										uint64 *const RESTRICT current_file_size) NOEXCEPT
{
	//Iterate over all files in the folder.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//If this is a directory, call this function again recursively.
		if (entry.is_directory())
		{
			BuildResourceCollectionsRecursive(parameters, entry.path().string().c_str(), current_file, file_counter, current_file_size);
		}

		else
		{
			//Open the resource file.
			BinaryInputFile resource_file{ entry.path().string().c_str() };

			//Get the size of the resource file.
			const uint64 resource_file_size{ resource_file.Size() };

			//Create another resource collection file, if needed.
			if (*current_file_size + resource_file_size >= parameters._MaximumFileSize)
			{
				//Close and delete the current file.
				(*current_file)->Close();
				delete (*current_file);

				//Create a new file.
				char buffer[MAXIMUM_FILE_PATH_LENGTH];
				sprintf_s(buffer, "%s_%llu.crc", parameters._Output, (*file_counter)++);

				(*current_file) = new BinaryOutputFile(buffer);

				//Reset the current file size.
				(*current_file_size) = 0;
			}

			//Read the data in the resource file.
			void *RESTRICT resource_file_data{ Memory::Allocate(resource_file_size) };
			resource_file.Read(resource_file_data, resource_file_size);

			//Write the resource file data to the resource collection file.
			(*current_file)->Write(resource_file_data, resource_file_size);

			//Free the resource file data.
			Memory::Free(resource_file_data);

			//Close the resource file.
			resource_file.Close();

			//Update the current file size.
			(*current_file_size) += resource_file_size;
		}
	}
}

/*
*	Builds resource collections.
*/
void ResourceBuildingSystem::BuildResourceCollections(const ResourceCollectionBuildParameters &parameters) NOEXCEPT
{
	//Set up variables.
	uint64 file_counter{ 0 };
	uint64 current_file_size{ 0 };

	//Create the first file.
	char buffer[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(buffer, "%s_%llu.crc", parameters._Output, file_counter++);

	BinaryOutputFile *RESTRICT current_file{ new BinaryOutputFile(buffer) };

	//Build the resource collections recursively.
	BuildResourceCollectionsRecursive(parameters, parameters._Folder, &current_file, &file_counter, &current_file_size);

	//Close the file.
	current_file->Close();
}

/*
*	Builds a raw data.
*/
void ResourceBuildingSystem::BuildRawData(const RawDataBuildParameters &parameters) NOEXCEPT
{
	//What should the output file path name be?
	DynamicString output_file_path_name{ parameters._Output };
	output_file_path_name += ".cr";

	//Open the output file to be written to.
	BinaryOutputFile output_file{ output_file_path_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::RAW_DATA_TYPE_IDENTIFIER, HashString(parameters._ResourceIdentifier), parameters._ResourceIdentifier };
	output_file.Write(&header, sizeof(ResourceHeader));

	//Write the data size.
	const uint64 data_size{ sizeof(byte) * parameters._Data->Size() };
	output_file.Write(&data_size, sizeof(uint64));

	//Write the data.
	output_file.Write(parameters._Data->Data(), data_size);

	//Close the output file.
	output_file.Close();
}

/*
*	Builds a render pipeline.
*/
void ResourceBuildingSystem::BuildRenderPipeline(const RenderPipelineBuildParameters &parameters) NOEXCEPT
{
	//What should the output file path name be?
	DynamicString output_file_path_name{ parameters._Output };
	output_file_path_name += ".cr";

	//Figure out the resource identifier.
	const char *resource_identifier{ nullptr };

	{
		const uint64 output_length{ StringUtilities::StringLength(parameters._Output) };

		for (int64 i{ static_cast<int64>(output_length) - 1 }; i >= 0; --i)
		{
			if (parameters._Output[i] == '\\')
			{
				resource_identifier = &parameters._Output[i + 1];

				break;
			}
		}
	}

	//Open the output file to be written to.
	BinaryOutputFile output_file{ output_file_path_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::RENDER_PIPELINE_TYPE_IDENTIFIER, HashString(resource_identifier), resource_identifier };
	output_file.Write(&header, sizeof(ResourceHeader));

	//Write the compute shader data.
	{
		if (!parameters._ComputeShaderData._GLSLData.Empty())
		{
			//Write that it has GLSL shader data.
			const bool has_data{ true };
			output_file.Write(&has_data, sizeof(bool));

			//Write the data size.
			const uint64 data_size{ parameters._ComputeShaderData._GLSLData.Size() };
			output_file.Write(&data_size, sizeof(uint64));

			//Write the data.
			output_file.Write(parameters._ComputeShaderData._GLSLData.Data(), parameters._ComputeShaderData._GLSLData.Size());
		}

		else
		{
			//Write that it doesn't GLSL shader data.
			const bool has_data{ false };
			output_file.Write(&has_data, sizeof(bool));
		}
	}

	//Write the vertex shader data.
	{
		if (!parameters._VertexShaderData._GLSLData.Empty())
		{
			//Write that it has GLSL shader data.
			const bool has_data{ true };
			output_file.Write(&has_data, sizeof(bool));

			//Write the data size.
			const uint64 data_size{ parameters._VertexShaderData._GLSLData.Size() };
			output_file.Write(&data_size, sizeof(uint64));

			//Write the data.
			output_file.Write(parameters._VertexShaderData._GLSLData.Data(), parameters._VertexShaderData._GLSLData.Size());
		}

		else
		{
			//Write that it doesn't GLSL shader data.
			const bool has_data{ false };
			output_file.Write(&has_data, sizeof(bool));
		}
	}

	//Write the fragment shader data.
	{
		if (!parameters._FragmentShaderData._GLSLData.Empty())
		{
			//Write that it has GLSL shader data.
			const bool has_data{ true };
			output_file.Write(&has_data, sizeof(bool));

			//Write the data size.
			const uint64 data_size{ parameters._FragmentShaderData._GLSLData.Size() };
			output_file.Write(&data_size, sizeof(uint64));

			//Write the data.
			output_file.Write(parameters._FragmentShaderData._GLSLData.Data(), parameters._FragmentShaderData._GLSLData.Size());
		}

		else
		{
			//Write that it doesn't GLSL shader data.
			const bool has_data{ false };
			output_file.Write(&has_data, sizeof(bool));
		}
	}

	//Write the ray generation shader data.
	{
		if (!parameters._RayGenerationShaderData._GLSLData.Empty())
		{
			//Write that it has GLSL shader data.
			const bool has_data{ true };
			output_file.Write(&has_data, sizeof(bool));

			//Write the data size.
			const uint64 data_size{ parameters._RayGenerationShaderData._GLSLData.Size() };
			output_file.Write(&data_size, sizeof(uint64));

			//Write the data.
			output_file.Write(parameters._RayGenerationShaderData._GLSLData.Data(), parameters._RayGenerationShaderData._GLSLData.Size());
		}

		else
		{
			//Write that it doesn't GLSL shader data.
			const bool has_data{ false };
			output_file.Write(&has_data, sizeof(bool));
		}
	}

	//Write the ray miss shader data.
	{
		const uint64 number_of_ray_miss_shaders{ parameters._RayMissShaderData.Size() };
		output_file.Write(&number_of_ray_miss_shaders, sizeof(uint64));

		for (const RenderPipelineBuildParameters::ShaderData &ray_miss_shader_data : parameters._RayMissShaderData)
		{
			//Write the data size.
			const uint64 data_size{ ray_miss_shader_data._GLSLData.Size() };
			output_file.Write(&data_size, sizeof(uint64));

			//Write the data.
			output_file.Write(ray_miss_shader_data._GLSLData.Data(), ray_miss_shader_data._GLSLData.Size());
		}
	}

	//Write the ray hit groups.
	{
		uint64 number_of_ray_hit_groups{ parameters._RayHitGroupShaderData.Size() };
		output_file.Write(&number_of_ray_hit_groups, sizeof(uint64));

		for (const RenderPipelineBuildParameters::RayHitGroupShaderData &ray_hit_group_shader_data : parameters._RayHitGroupShaderData)
		{
			//Write the identifier.
			output_file.Write(&ray_hit_group_shader_data._Identifier, sizeof(HashString));

			//Write the ray closest hit shader data, if it exists.
			if (!ray_hit_group_shader_data._RayClosestHitShaderData._GLSLData.Empty())
			{
				//Write that it has GLSL shader data.
				const bool has_data{ true };
				output_file.Write(&has_data, sizeof(bool));

				//Write the data size.
				const uint64 data_size{ ray_hit_group_shader_data._RayClosestHitShaderData._GLSLData.Size() };
				output_file.Write(&data_size, sizeof(uint64));

				//Write the data.
				output_file.Write(ray_hit_group_shader_data._RayClosestHitShaderData._GLSLData.Data(), ray_hit_group_shader_data._RayClosestHitShaderData._GLSLData.Size());
			}

			else
			{
				//Write that it doesn't GLSL shader data.
				const bool has_data{ false };
				output_file.Write(&has_data, sizeof(bool));
			}

			//Write the ray any hit shader data, if it exists.
			if (!ray_hit_group_shader_data._RayAnyHitShaderData._GLSLData.Empty())
			{
				//Write that it has GLSL shader data.
				const bool has_data{ true };
				output_file.Write(&has_data, sizeof(bool));

				//Write the data size.
				const uint64 data_size{ ray_hit_group_shader_data._RayAnyHitShaderData._GLSLData.Size() };
				output_file.Write(&data_size, sizeof(uint64));

				//Write the data.
				output_file.Write(ray_hit_group_shader_data._RayAnyHitShaderData._GLSLData.Data(), ray_hit_group_shader_data._RayAnyHitShaderData._GLSLData.Size());
			}

			else
			{
				//Write that it doesn't GLSL shader data.
				const bool has_data{ false };
				output_file.Write(&has_data, sizeof(bool));
			}
		}
	}

	//Write the included uniform buffers.
	{
		const uint64 number_of_included_uniform_buffers{ parameters._IncludedUniformBuffers.Size() };
		output_file.Write(&number_of_included_uniform_buffers, sizeof(uint64));

		if (!parameters._IncludedUniformBuffers.Empty())
		{
			//Write the data.
			output_file.Write(parameters._IncludedUniformBuffers.Data(), sizeof(HashString) * parameters._IncludedUniformBuffers.Size());
		}
	}

	//Write the included storage buffers.
	{
		const uint64 number_of_included_storage_buffers{ parameters._IncludedStorageBuffers.Size() };
		output_file.Write(&number_of_included_storage_buffers, sizeof(uint64));

		if (!parameters._IncludedStorageBuffers.Empty())
		{
			//Write the data.
			output_file.Write(parameters._IncludedStorageBuffers.Data(), sizeof(HashString) * parameters._IncludedStorageBuffers.Size());
		}
	}

	//Write the compute render targets.
	{
		//Write the count.
		const uint64 count{ parameters._ComputeRenderTargets.Size() };
		output_file.Write(&count, sizeof(uint64));

		//Write the data.
		if (count > 0)
		{
			output_file.Write(parameters._ComputeRenderTargets.Data(), sizeof(HashString) * parameters._ComputeRenderTargets.Size());
		}
	}

	//Write the input render targets.
	{
		//Write the count.
		const uint64 count{ parameters._InputRenderTargets.Size() };
		output_file.Write(&count, sizeof(uint64));

		//Write the data.
		if (count > 0)
		{
			output_file.Write(parameters._InputRenderTargets.Data(), sizeof(Pair<HashString, SamplerProperties>) * parameters._InputRenderTargets.Size());
		}
	}

	//Write the output render targets.
	{
		//Write the count.
		const uint64 count{ parameters._OutputRenderTargets.Size() };
		output_file.Write(&count, sizeof(uint64));

		//Write the data.
		if (count > 0)
		{
			output_file.Write(parameters._OutputRenderTargets.Data(), sizeof(HashString) * parameters._OutputRenderTargets.Size());
		}
	}

	//Write the blend properties.
	output_file.Write(&parameters._BlendColorOperator, sizeof(BlendOperator));
	output_file.Write(&parameters._BlendAlphaSourceFactor, sizeof(BlendFactor));
	output_file.Write(&parameters._BlendAlphaDestinationFactor, sizeof(BlendFactor));
	output_file.Write(&parameters._BlendAlphaOperator, sizeof(BlendOperator));

	//Write the depth/stencil properties.
	output_file.Write(&parameters._DepthTestEnabled, sizeof(bool));
	output_file.Write(&parameters._DepthWriteEnabled, sizeof(bool));
	output_file.Write(&parameters._DepthCompareOperator, sizeof(CompareOperator));
	output_file.Write(&parameters._StencilTestEnabled, sizeof(bool));
	output_file.Write(&parameters._StencilFailOperator, sizeof(StencilOperator));
	output_file.Write(&parameters._StencilPassOperator, sizeof(StencilOperator));
	output_file.Write(&parameters._StencilDepthFailOperator, sizeof(StencilOperator));
	output_file.Write(&parameters._StencilCompareOperator, sizeof(CompareOperator));
	output_file.Write(&parameters._StencilCompareMask, sizeof(uint32));
	output_file.Write(&parameters._StencilWriteMask, sizeof(uint32));
	output_file.Write(&parameters._StencilReferenceMask, sizeof(uint32));

	//Write the topology.
	output_file.Write(&parameters._Topology, sizeof(Topology));

	//Write the sampler properties.
	{
		const uint64 number_of_sampler_properties{ parameters._SamplerProperties.Size() };
		output_file.Write(&number_of_sampler_properties, sizeof(uint64));

		if (!parameters._SamplerProperties.Empty())
		{
			output_file.Write(parameters._SamplerProperties.Data(), sizeof(SamplerProperties) * parameters._SamplerProperties.Size());
		}
	}

	//Write the input stream subscriptions.
	{
		const uint64 number_of_input_stream_subscriptions{ parameters._InputStreamSubscriptions.Size() };
		output_file.Write(&number_of_input_stream_subscriptions, sizeof(uint64));

		if (!parameters._InputStreamSubscriptions.Empty())
		{
			output_file.Write(parameters._InputStreamSubscriptions.Data(), sizeof(HashString)* parameters._InputStreamSubscriptions.Size());
		}
	}

	//Close the output file.
	output_file.Close();
}

/*
*	Builds a shader.
*/
void ResourceBuildingSystem::BuildShader(const ShaderBuildParameters &parameters) NOEXCEPT
{
#if 0 //Use new way. (:
	//Create a compiler-ready version of the file.
	std::string shader_source;

	{
		std::ifstream file{ parameters._FilePath };

		file.seekg(0, std::ios::end);   
		shader_source.reserve(file.tellg());
		file.seekg(0, std::ios::beg);

		shader_source.assign(	std::istreambuf_iterator<char>(file),
								std::istreambuf_iterator<char>());

		//Add the defines.
		for (const char* const RESTRICT define : parameters._Defines)
		{
			shader_source = std::string("#define ") + define + std::string("\n") + shader_source;
		}

		//Add the header data.
		{
			shader_source = "#version 460 \n#include \"C:\\Github\\Catalyst-Engine\\Engine\\Shaders\\CatalystShaderCommon.glsl\" \n" + shader_source;
		}

		//Replace "CatalystShaderMain" with "main".
		{
			size_t position{ shader_source.find("CatalystShaderMain") };

			if (position != std::string::npos)
			{
				shader_source.replace(position, strlen("CatalystShaderMain"), "main");
			}
		}
	}

	//Initialize the compiler.
	shaderc_compiler_t compiler{ shaderc_compiler_initialize() };

	//Initialize the options.
	shaderc_compile_options_t options{ shaderc_compile_options_initialize() };

	//Set the source language.
	shaderc_compile_options_set_source_language(options, shaderc_source_language::shaderc_source_language_glsl);

	//Set the optimization level.
	shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level::shaderc_optimization_level_performance);

	//Determine the shader kind.
	shaderc_shader_kind shader_kind;

	switch (parameters._Stage)
	{
		case ShaderStage::COMPUTE:
		{
			shader_kind = shaderc_shader_kind::shaderc_compute_shader;

			break;
		}

		case ShaderStage::FRAGMENT:
		{
			shader_kind = shaderc_shader_kind::shaderc_fragment_shader;

			break;
		}

		case ShaderStage::GEOMETRY:
		{
			shader_kind = shaderc_shader_kind::shaderc_geometry_shader;

			break;
		}

		case ShaderStage::RAY_ANY_HIT:
		{
			shader_kind = shaderc_shader_kind::shaderc_anyhit_shader;

			break;
		}

		case ShaderStage::RAY_CLOSEST_HIT:
		{
			shader_kind = shaderc_shader_kind::shaderc_closesthit_shader;

			break;
		}

		case ShaderStage::RAY_GENERATION:
		{
			shader_kind = shaderc_shader_kind::shaderc_raygen_shader;

			break;
		}

		case ShaderStage::RAY_INTERSECTION:
		{
			shader_kind = shaderc_shader_kind::shaderc_intersection_shader;

			break;
		}

		case ShaderStage::RAY_MISS:
		{
			shader_kind = shaderc_shader_kind::shaderc_miss_shader;

			break;
		}

		case ShaderStage::TESSELLATION_CONTROL:
		{
			shader_kind = shaderc_shader_kind::shaderc_tess_control_shader;

			break;
		}

		case ShaderStage::TESSELLATION_EVALUATION:
		{
			shader_kind = shaderc_shader_kind::shaderc_tess_evaluation_shader;

			break;
		}

		case ShaderStage::VERTEX:
		{
			shader_kind = shaderc_shader_kind::shaderc_vertex_shader;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Set up the include resolve function.
	auto include_resolve_function{ [](void* user_data, const char* requested_source, int type, const char* requesting_source, size_t include_depth)
	{
		//Allocate the result.
		shaderc_include_result *const RESTRICT result{static_cast<shaderc_include_result *const RESTRICT>(Memory::Allocate(sizeof(shaderc_include_result)))};

		//Absolute include paths aren't allowed.
		if (type == shaderc_include_type_standard)
		{
			result->source_name = nullptr;
			result->source_name_length = 0;
			result->content = "No absolute paths!";
			result->content_length = strlen("No absolute paths!");

			return result;
		}

		//Fill in the result.
		result->source_name = requested_source;
		result->source_name_length = strlen(requested_source);

		//Get the absolute path of the requesting source.
		std::string absolute_requesting_source{ std::filesystem::absolute(requesting_source).generic_string() };

		//Replace "/" with "\\".
		{
			for (;;)
			{
				size_t position{ absolute_requesting_source.find("/") };

				if (position == std::string::npos)
				{
					break;
				}

				absolute_requesting_source.replace(position, 1, "\\");
			}
		}

		{
			size_t position{ absolute_requesting_source.find_last_of("\\") };
			absolute_requesting_source.erase(position, absolute_requesting_source.size() - position);
		}

		//Change the working directory temporarily.
		std::filesystem::path default_working_directory{ std::filesystem::current_path() };

		//Set the working directory to the requesting source.
		std::filesystem::current_path(absolute_requesting_source);

		//Open the file.
		std::ifstream file{ requested_source };

		std::string include_source;

		file.seekg(0, std::ios::end);   
		include_source.reserve(file.tellg());
		file.seekg(0, std::ios::beg);

		include_source.assign(	std::istreambuf_iterator<char>(file),
								std::istreambuf_iterator<char>());

		//Close the file.
		file.close();

		//Reset the working directory.
		std::filesystem::current_path(default_working_directory);

		//Allocate the necessary data for the include content.
		result->content = static_cast<const char* const RESTRICT>(Memory::Allocate(include_source.size()));
		Memory::Copy(result->content, include_source.data(), include_source.size());
		result->content_length = include_source.size();

		//Return the result.
		return result;
	} };

	//Set up the include result release function.
	auto include_result_release_function{ [](void* user_data, shaderc_include_result* include_result)
	{
		//Free the content.
		Memory::Free(include_result->content);

		//Free the result.
		Memory::Free(include_result);
	} };

	//Set the include callbacks.
	shaderc_compile_options_set_include_callbacks(options, include_resolve_function, include_result_release_function, nullptr);

	//Compile!
	shaderc_compilation_result_t result{ shaderc_compile_into_spv(compiler, shader_source.c_str(), shader_source.size(), shader_kind, parameters._FilePath, "main", options) };

	//Check for errors.
	if (shaderc_result_get_num_errors(result) > 0)
	{
		ASSERT(false, shaderc_result_get_error_message(result));
	}

	//What should the resource be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::SHADER_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

	//Write the stage.
	file.Write(&parameters._Stage, sizeof(ShaderStage));

	//Write the size of the compiled file.
	const uint64 compiled_file_size{ shaderc_result_get_length(result) };
	file.Write(&compiled_file_size, sizeof(uint64));

	//Read the data.
	DynamicArray<byte> data;
	data.Upsize<false>(compiled_file_size);

	Memory::Copy(data.Data(), shaderc_result_get_bytes(result), compiled_file_size);

	//Write the data.
	file.Write(data.Data(), compiled_file_size);

	//Close the file.
	file.Close();

	//Release the result.
	shaderc_result_release(result);

	//Release the options.
	shaderc_compile_options_release(options);

	//Release the compiler.
	shaderc_compiler_release(compiler);
#else
	//Determine the temporary shader file path.
	DynamicString temporary_shader_file_path{ "C:\\Github\\Catalyst-Engine\\Engine\\Shaders\\" };
	temporary_shader_file_path += parameters._ID;
	temporary_shader_file_path += ".glsl";

	//Determine the compiled file path.
	DynamicString compiled_file_path{ parameters._ID };
	compiled_file_path += ".compiled";

	//Determine the temporary batch file path.
	DynamicString temporary_batch_file_path{ parameters._ID };
	temporary_batch_file_path += ".bat";

	//First, compile the shader.
	{
		//Create a compiler-ready version of the file.
		{
			std::ifstream file{ parameters._FilePath };
			std::string file_string;

			file.seekg(0, std::ios::end);   
			file_string.reserve(file.tellg());
			file.seekg(0, std::ios::beg);

			file_string.assign(	(std::istreambuf_iterator<char>(file)),
								std::istreambuf_iterator<char>());

			//Add the defines.
			for (const char* const RESTRICT define : parameters._Defines)
			{
				file_string = std::string("#define ") + define + std::string("\n") + file_string;
			}

			//Add the header data.
			{
				file_string = "#version 460 \n#extension GL_GOOGLE_include_directive : enable \n#include \"CatalystShaderCommon.glsl\" \n" + file_string;
			}

			//Replace "CatalystShaderMain" with "main".
			{
				size_t position{ file_string.find("CatalystShaderMain") };

				if (position != std::string::npos)
				{
					file_string.replace(position, strlen("CatalystShaderMain"), "main");
				}
			}

			//Write the compiler-ready version to the temporary shader file.
			std::ofstream shader_file{ temporary_shader_file_path.Data() };

			shader_file << file_string;

			shader_file.close();
		}

		//Compile the file.
		{
			//Create a temporary batch file that stores all commands.
			std::ofstream batch_file;

			batch_file.open(temporary_batch_file_path.Data(), std::ios::out);

			batch_file << "C:\\Github\\Catalyst-Engine\\Engine\\Binaries\\glslangValidator.exe";
			batch_file << " -V ";
			batch_file << temporary_shader_file_path.Data();
			batch_file << " -o ";
			batch_file << compiled_file_path.Data();

			switch (parameters._Stage)
			{
				case ShaderStage::COMPUTE:
				{
					batch_file << " -S comp";

					break;
				}

				case ShaderStage::FRAGMENT:
				{
					batch_file << " -S frag";

					break;
				}

				case ShaderStage::GEOMETRY:
				{
					batch_file << " -S geom";

					break;
				}

				case ShaderStage::RAY_ANY_HIT:
				{
					batch_file << " -S rahit";

					break;
				}

				case ShaderStage::RAY_CLOSEST_HIT:
				{
					batch_file << " -S rchit";

					break;
				}

				case ShaderStage::RAY_GENERATION:
				{
					batch_file << " -S rgen";

					break;
				}

				case ShaderStage::RAY_INTERSECTION:
				{
					batch_file << " -S rint";

					break;
				}

				case ShaderStage::RAY_MISS:
				{
					batch_file << " -S rmiss";

					break;
				}

				case ShaderStage::TESSELLATION_CONTROL:
				{
					batch_file << " -S tesc";

					break;
				}

				case ShaderStage::TESSELLATION_EVALUATION:
				{
					batch_file << " -S tese";

					break;
				}

				case ShaderStage::VERTEX:
				{
					batch_file << " -S vert";

					break;
				}
			}

			batch_file.close();

			system(temporary_batch_file_path.Data());

			//Delete the temporary batch file.
			File::Delete(temporary_batch_file_path.Data());
		}
	}

	//If the file exists, recreate the temporary batch file with a pause statement in the end to display the error.
	if (!File::Exists(compiled_file_path.Data()))
	{
		std::ofstream batch_file;

		batch_file.open(temporary_batch_file_path.Data(), std::ios::out);

		batch_file << "C:\\Github\\Catalyst-Engine\\Engine\\Binaries\\glslangValidator.exe";
		batch_file << " -V ";
		batch_file << temporary_shader_file_path.Data();
		batch_file << " -o ";
		batch_file << compiled_file_path.Data();

		switch (parameters._Stage)
		{
			case ShaderStage::COMPUTE:
			{
				batch_file << " -S comp";

				break;
			}

			case ShaderStage::FRAGMENT:
			{
				batch_file << " -S frag";

				break;
			}

			case ShaderStage::GEOMETRY:
			{
				batch_file << " -S geom";

				break;
			}

			case ShaderStage::RAY_ANY_HIT:
			{
				batch_file << " -S rahit";

				break;
			}

			case ShaderStage::RAY_CLOSEST_HIT:
			{
				batch_file << " -S rchit";

				break;
			}

			case ShaderStage::RAY_GENERATION:
			{
				batch_file << " -S rgen";

				break;
			}

			case ShaderStage::RAY_INTERSECTION:
			{
				batch_file << " -S rint";

				break;
			}

			case ShaderStage::RAY_MISS:
			{
				batch_file << " -S rmiss";

				break;
			}

			case ShaderStage::TESSELLATION_CONTROL:
			{
				batch_file << " -S tesc";

				break;
			}

			case ShaderStage::TESSELLATION_EVALUATION:
			{
				batch_file << " -S tese";

				break;
			}

			case ShaderStage::VERTEX:
			{
				batch_file << " -S vert";

				break;
			}
		}

		batch_file << std::endl;
		batch_file << "pause";

		batch_file.close();

		system(temporary_batch_file_path.Data());
	}

	//Delete the temporary shader file.
	File::Delete(temporary_shader_file_path.Data());

	//What should the resource be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryOutputFile file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::SHADER_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

	//Write the stage.
	file.Write(&parameters._Stage, sizeof(ShaderStage));

	//Open the compiled file.
	BinaryInputFile compiled_file{ compiled_file_path.Data() };

	//Write the size of the compiled file.
	const uint64 compiled_file_size{ compiled_file.Size() };
	file.Write(&compiled_file_size, sizeof(uint64));

	//Read the data.
	DynamicArray<byte> data;
	data.Upsize<false>(compiled_file_size);

	compiled_file.Read(data.Data(), compiled_file_size);

	//Write the data.
	file.Write(data.Data(), compiled_file_size);

	//Close the compiled file.
	compiled_file.Close();

	//Close the file.
	file.Close();

	//Delete the compiled file.
	File::Delete(compiled_file_path.Data());
#endif
}

/*
*	Builds a sound.
*/
void ResourceBuildingSystem::BuildSound(const SoundBuildParameters &parameters) NOEXCEPT
{
	//Read the file.
	SoundResource resource;

	if (WAVReader::Read(parameters._File, &resource))
	{
		//Resample the sound resource to the desired sample rate.
		if (resource._SampleRate != parameters._DesiredSampleRate)
		{
			const float32 playback_rate{ resource._SampleRate / parameters._DesiredSampleRate };

			DynamicArray<DynamicArray<int16>> temporary_samples{ resource._Samples };

			for (uint64 i{ 0 }, size{ temporary_samples.Size() }; i < size; ++i)
			{
				resource._Samples[i].Clear();

				DynamicArray<int16>& channel{ temporary_samples[i] };

				uint64 current_sample{ 0 };
				float32 current_sample_fraction{ 0.0f };

				while (current_sample < channel.Size())
				{
					const int16 first_sample{ channel[current_sample] };
					const int16 second_sample{ channel[current_sample < channel.Size() - 1 ? current_sample + 1 : current_sample] };

					const int16 interpolated_sample{ BaseMath::Round<int16>(BaseMath::LinearlyInterpolate(static_cast<float32>(first_sample), static_cast<float32>(second_sample), current_sample_fraction)) };
				
					resource._Samples[i].Emplace(interpolated_sample);

					current_sample_fraction += playback_rate;

					while (current_sample_fraction >= 1.0f)
					{
						++current_sample;
						current_sample_fraction -= 1.0f;
					}
				}
			}
		}

		//What should the resource be called?
		DynamicString file_name{ parameters._Output };
		file_name += ".cr";

		//Open the file to be written to.
		BinaryOutputFile file{ file_name.Data() };

		//Write the resource header to the file.
		const ResourceHeader header{ ResourceConstants::SOUND_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
		file.Write(&header, sizeof(ResourceHeader));

		//Write the sample rate to the file.
		file.Write(&parameters._DesiredSampleRate, sizeof(float32));

		//Write the number of channels to the file.
		file.Write(&resource._NumberOfChannels, sizeof(uint8));

		//Write the number of samples.
		const uint64 number_of_samples{ resource._Samples[0].Size() }; //Assume all channels have the same size.
		file.Write(&number_of_samples, sizeof(uint64));

		//Write the samples.
		for (const DynamicArray<int16>& channel : resource._Samples)
		{
			file.Write(channel.Data(), sizeof(int16) * number_of_samples);
		}

		//Close the file.
		file.Close();
	}

	else
	{
		ASSERT(false, "Couldn't build sound with output; %s, ID; %s, file path; %s", parameters._Output, parameters._ID, parameters._File);
	}
}

/*
*	Builds a texture 3D.
*/
void ResourceBuildingSystem::BuildTexture3D(const Texture3DBuildParameters& parameters) NOEXCEPT
{
	//What should the material be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryOutputFile file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::TEXTURE_3D_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

	//Write the number of mipmap levels to the file.
	constexpr uint8 MIPMAP_LEVELS{ 1 };
	file.Write(&MIPMAP_LEVELS, sizeof(uint8));

	//Write the width, height and depth.
	const uint32 width{ parameters._Texture->GetWidth() };
	const uint32 height{ parameters._Texture->GetHeight() };
	const uint32 depth{ parameters._Texture->GetDepth() };

	file.Write(&width, sizeof(uint32));
	file.Write(&height, sizeof(uint32));
	file.Write(&depth, sizeof(uint32));

	//Write the data.
	file.Write(parameters._Texture->Data(), width * height * depth * sizeof(Vector4<byte>));

	//Close the output file.
	file.Close();
}
#endif