#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
//Header file.
#include <Resources/Building/ResourceBuildingSystem.h>

//Core.
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

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
#include <Resources/Core/ResourceHeader.h>

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

	//Write the number of external textures.
	output_file.Write(&parameters._NumberOfExternalTextures, sizeof(uint32));

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