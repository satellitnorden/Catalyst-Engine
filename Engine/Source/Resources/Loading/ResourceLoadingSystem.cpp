//Header file.
#include <Resources/Loading/ResourceLoadingSystem.h>

//Animation.
#include <Animation/AnimatedVertex.h>

//File.
#include <File/Core/BinaryInputFile.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Given a file, load raw data data.
*/
void ResourceLoadingSystem::LoadRawData(BinaryInputFile *const RESTRICT file, RawDataData* const RESTRICT data) NOEXCEPT
{
	//Read the data size.
	uint64 data_size;
	file->Read(&data_size, sizeof(uint64));

	//Reserve the appropriate amount of memory.
	data->_Data.Upsize<false>(data_size);

	//Read the data.
	file->Read(data->_Data.Data(), data_size);
}

/*
*	Given a file, load render pipeline data.
*/
void ResourceLoadingSystem::LoadRenderPipeline(BinaryInputFile *const RESTRICT file, RenderPipelineData *const RESTRICT data) NOEXCEPT
{
	//Read the compute shader data.
	{
		bool has_data{ false };
		file->Read(&has_data, sizeof(bool));

		if (has_data)
		{
			uint64 data_size{ 0 };
			file->Read(&data_size, sizeof(uint64));
			data->_ComputeShaderData._GLSLData.Upsize<false>(data_size);

			file->Read(data->_ComputeShaderData._GLSLData.Data(), data_size);
		}
	}

	//Read the vertex shader data.
	{
		bool has_data{ false };
		file->Read(&has_data, sizeof(bool));

		if (has_data)
		{
			uint64 data_size{ 0 };
			file->Read(&data_size, sizeof(uint64));
			data->_VertexShaderData._GLSLData.Upsize<false>(data_size);

			file->Read(data->_VertexShaderData._GLSLData.Data(), data_size);
		}
	}

	//Read the fragment shader data.
	{
		bool has_data{ false };
		file->Read(&has_data, sizeof(bool));

		if (has_data)
		{
			uint64 data_size{ 0 };
			file->Read(&data_size, sizeof(uint64));
			data->_FragmentShaderData._GLSLData.Upsize<false>(data_size);

			file->Read(data->_FragmentShaderData._GLSLData.Data(), data_size);
		}
	}

	//Read the ray generation shader data.
	{
		bool has_data{ false };
		file->Read(&has_data, sizeof(bool));

		if (has_data)
		{
			uint64 data_size{ 0 };
			file->Read(&data_size, sizeof(uint64));
			data->_RayGenerationShaderData._GLSLData.Upsize<false>(data_size);

			file->Read(data->_RayGenerationShaderData._GLSLData.Data(), data_size);
		}
	}

	//Read the ray miss shader data.
	{
		uint64 number_of_ray_miss_shaders{ 0 };
		file->Read(&number_of_ray_miss_shaders, sizeof(uint64));

		if (number_of_ray_miss_shaders > 0)
		{
			data->_RayMissShaderData.Upsize<true>(number_of_ray_miss_shaders);

			for (uint64 i{ 0 }; i < number_of_ray_miss_shaders; ++i)
			{
				uint64 data_size{ 0 };
				file->Read(&data_size, sizeof(uint64));
				data->_RayMissShaderData[i]._GLSLData.Upsize<false>(data_size);

				file->Read(data->_RayMissShaderData[i]._GLSLData.Data(), data_size);
			}
		}
	}

	//Read the ray hit group.
	{
		uint64 number_of_ray_hit_groups{ 0 };
		file->Read(&number_of_ray_hit_groups, sizeof(uint64));

		if (number_of_ray_hit_groups > 0)
		{
			data->_RayHitGroupShaderData.Upsize<true>(number_of_ray_hit_groups);

			for (uint64 ray_hit_group_index{ 0 }; ray_hit_group_index < number_of_ray_hit_groups; ++ray_hit_group_index)
			{
				file->Read(&data->_RayHitGroupShaderData[ray_hit_group_index]._Identifier, sizeof(HashString));

				{
					bool has_data{ false };
					file->Read(&has_data, sizeof(bool));

					if (has_data)
					{
						uint64 data_size{ 0 };
						file->Read(&data_size, sizeof(uint64));
						data->_RayHitGroupShaderData[ray_hit_group_index]._RayClosestHitShaderData._GLSLData.Upsize<false>(data_size);

						file->Read(data->_RayHitGroupShaderData[ray_hit_group_index]._RayClosestHitShaderData._GLSLData.Data(), data_size);
					}
				}

				{
					bool has_data{ false };
					file->Read(&has_data, sizeof(bool));

					if (has_data)
					{
						uint64 data_size{ 0 };
						file->Read(&data_size, sizeof(uint64));
						data->_RayHitGroupShaderData[ray_hit_group_index]._RayAnyHitShaderData._GLSLData.Upsize<false>(data_size);

						file->Read(data->_RayHitGroupShaderData[ray_hit_group_index]._RayAnyHitShaderData._GLSLData.Data(), data_size);
					}
				}
			}
		}
	}

	//Read the included uniform buffers.
	{
		uint64 number_of_included_uniform_buffers{ 0 };
		file->Read(&number_of_included_uniform_buffers, sizeof(uint64));

		if (number_of_included_uniform_buffers > 0)
		{
			data->_IncludedUniformBuffers.Upsize<false>(number_of_included_uniform_buffers);
			file->Read(data->_IncludedUniformBuffers.Data(), sizeof(HashString) * number_of_included_uniform_buffers);
		}
	}

	//Read the included storage buffers.
	{
		uint64 number_of_included_storage_buffers{ 0 };
		file->Read(&number_of_included_storage_buffers, sizeof(uint64));

		if (number_of_included_storage_buffers > 0)
		{
			data->_IncludedStorageBuffers.Upsize<false>(number_of_included_storage_buffers);
			file->Read(data->_IncludedStorageBuffers.Data(), sizeof(HashString) * number_of_included_storage_buffers);
		}
	}

	//Read the compute render targets.
	{
		uint64 length{ 0 };
		file->Read(&length, sizeof(uint64));

		if (length > 0)
		{
			data->_ComputeRenderTargets.Upsize<false>(length);
			file->Read(data->_ComputeRenderTargets.Data(), sizeof(HashString) * data->_ComputeRenderTargets.Size());
		}
	}

	//Read the input render targets.
	{
		uint64 length{ 0 };
		file->Read(&length, sizeof(uint64));

		if (length > 0)
		{
			data->_InputRenderTargets.Upsize<false>(length);
			file->Read(data->_InputRenderTargets.Data(), sizeof(Pair<HashString, SamplerProperties>) * data->_InputRenderTargets.Size());
		}
	}

	//Read the output render targets.
	{
		uint64 length{ 0 };
		file->Read(&length, sizeof(uint64));

		if (length > 0)
		{
			data->_OutputRenderTargets.Upsize<false>(length);
			file->Read(data->_OutputRenderTargets.Data(), sizeof(HashString) * data->_OutputRenderTargets.Size());
		}
	}

	//Read the depth/stencil properties.
	file->Read(&data->_DepthTestEnabled, sizeof(bool));
	file->Read(&data->_DepthWriteEnabled, sizeof(bool));
	file->Read(&data->_DepthCompareOperator, sizeof(CompareOperator));
	file->Read(&data->_StencilTestEnabled, sizeof(bool));
	file->Read(&data->_StencilFailOperator, sizeof(StencilOperator));
	file->Read(&data->_StencilPassOperator, sizeof(StencilOperator));
	file->Read(&data->_StencilDepthFailOperator, sizeof(StencilOperator));
	file->Read(&data->_StencilCompareOperator, sizeof(CompareOperator));
	file->Read(&data->_StencilCompareMask, sizeof(uint32));
	file->Read(&data->_StencilWriteMask, sizeof(uint32));
	file->Read(&data->_StencilReferenceMask, sizeof(uint32));

	//Read the topology.
	file->Read(&data->_Topology, sizeof(Topology));

	//Read the sampler properties.
	{
		uint64 number_of_sampler_properties{ 0 };
		file->Read(&number_of_sampler_properties, sizeof(uint64));

		if (number_of_sampler_properties > 0)
		{
			data->_SamplerProperties.Upsize<false>(number_of_sampler_properties);
			file->Read(data->_SamplerProperties.Data(), sizeof(SamplerProperties) * number_of_sampler_properties);
		}
	}

	//Read the number of external textures.
	file->Read(&data->_NumberOfExternalTextures, sizeof(uint32));

	//Read the input stream subscriptions.
	{
		uint64 number_of_input_stream_subscriptions{ 0 };
		file->Read(&number_of_input_stream_subscriptions, sizeof(uint64));

		if (number_of_input_stream_subscriptions > 0)
		{
			data->_InputStreamSubscriptions.Upsize<false>(number_of_input_stream_subscriptions);
			file->Read(data->_InputStreamSubscriptions.Data(), sizeof(HashString) * number_of_input_stream_subscriptions);
		}
	}
}

/*
*	Given a file, load shader data.
*/
void ResourceLoadingSystem::LoadShader(BinaryInputFile *const RESTRICT file, ShaderData *const RESTRICT data) NOEXCEPT
{
	//Read the stage.
	file->Read(&data->_Stage, sizeof(ShaderStage));

	//Read the data size.
	uint64 data_size{ 0 };
	file->Read(&data_size, sizeof(uint64));

	//Reserve the appropriate amount of memory for the data.
	data->_Data.Upsize<false>(data_size);

	//Read the data.
	file->Read(data->_Data.Data(), data_size);
}

/*
*	Given a file, load texture 3D data.
*/
void ResourceLoadingSystem::LoadTexture3D(BinaryInputFile *const RESTRICT file, Texture3DData *const RESTRICT data) NOEXCEPT
{
	//Read the number of mipmap levels.
	file->Read(&data->_MipmapLevels, sizeof(uint8));

	//Read the width.
	file->Read(&data->_Width, sizeof(uint32));

	//Read the height.
	file->Read(&data->_Height, sizeof(uint32));

	//Read the depth.
	file->Read(&data->_Depth, sizeof(uint32));

	//Read the data.
	data->_Data.Upsize<true>(data->_MipmapLevels);

	for (uint8 i{ 0 }; i < data->_MipmapLevels; ++i)
	{
		const uint64 textureSize{ (data->_Width >> i) * (data->_Height >> i) * (data->_Depth >> i) * sizeof(Vector4<byte>) };

		data->_Data[i].Reserve(textureSize);

		file->Read(data->_Data[i].Data(), textureSize);
	}
}