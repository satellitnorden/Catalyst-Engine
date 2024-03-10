//Header file.
#include <Resources/Loading/ResourceLoadingSystem.h>

//Animation.
#include <Animation/AnimatedVertex.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Reads a bone from file.
*/
FORCE_INLINE void ReadBoneFromFile(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, Bone *const RESTRICT bone) NOEXCEPT
{
	file->Read(bone, sizeof(Bone) - sizeof(DynamicArray<Bone>));

	uint64 number_of_child_bones;
	file->Read(&number_of_child_bones, sizeof(uint64));

	bone->_Children.Upsize<true>(number_of_child_bones);

	for (Bone &child_bone : bone->_Children)
	{
		ReadBoneFromFile(file, &child_bone);
	}
}

/*
*	Given a file, load an animated model.
*/
void ResourceLoadingSystem::LoadAnimatedModel(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, AnimatedModelData *const RESTRICT data) NOEXCEPT
{
	//Read the axis-aligned bounding box
	file->Read(&data->_AxisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox3D));

	//Read the number of vertices.
	uint64 number_of_vertices;
	file->Read(&number_of_vertices, sizeof(uint64));

	//Read the vertices.
	data->_Vertices.Upsize<false>(number_of_vertices);
	file->Read(data->_Vertices.Data(), sizeof(AnimatedVertex) * number_of_vertices);

	//Read the number of indices.
	uint64 number_of_indices;
	file->Read(&number_of_indices, sizeof(uint64));

	//Read the indices.
	data->_Indices.Upsize<false>(number_of_indices);
	file->Read(data->_Indices.Data(), sizeof(uint32) * number_of_indices);

	//Read the skeleton.
	ReadBoneFromFile(file, &data->_Skeleton._RootBone);
}

/*
*	Given a file, load animation data.
*/
void ResourceLoadingSystem::LoadAnimation(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, AnimationData *const RESTRICT data) NOEXCEPT
{
	//Read the duration.
	file->Read(&data->_Animation._Duration, sizeof(float32));

	//Read the number of animation keyframe channels.
	uint64 number_of_animation_keyframe_channels;
	file->Read(&number_of_animation_keyframe_channels, sizeof(uint64));

	//Read the animation keyframes.
	for (uint64 i{ 0 }; i < number_of_animation_keyframe_channels; ++i)
	{
		//Read the name of the channel.
		HashString name;
		file->Read(&name, sizeof(HashString));

		//Read the number of animation keyframes.
		uint64 number_of_animation_keyframes;
		file->Read(&number_of_animation_keyframes, sizeof(uint64));

		//Read the keyframe.
		data->_Animation._Keyframes[name].Upsize<false>(number_of_animation_keyframes);
		file->Read(data->_Animation._Keyframes[name].Data(), sizeof(AnimationKeyframe) * number_of_animation_keyframes);
	}
}

/*
*	Given a file, load font data.
*/
void ResourceLoadingSystem::LoadFont(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, FontData *const RESTRICT data) NOEXCEPT
{
	//Read all characters.
	for (int8 i{ 0 }; i < FontResource::NUMBER_OF_CHARACTER_DESCRIPTIONS; ++i)
	{
		//Read the character description.
		file->Read(&data->_CharacterDescriptions[i], sizeof(FontResource::CharacterDescription));
	}

	//Read the master texture width.
	file->Read(&data->_MasterTextureWidth, sizeof(uint32));

	//Read the master texture height.
	file->Read(&data->_MasterTextureHeight, sizeof(uint32));

	//Read the number of mipmap levels.
	file->Read(&data->_NumberOfMipmapLevels, sizeof(uint8));

	//Read the master texture data.
	data->_MasterTextureData.Upsize<true>(data->_NumberOfMipmapLevels);

	for (uint8 i{ 0 }; i < data->_NumberOfMipmapLevels; ++i)
	{
		const uint32 mip_width{ data->_MasterTextureWidth >> i };
		const uint32 mip_height{ data->_MasterTextureHeight >> i };

		data->_MasterTextureData[i].Upsize<false>(mip_width * mip_height);

		file->Read(data->_MasterTextureData[i].Data(), mip_width * mip_height);
	}
}

/*
*	Given a file, load level data.
*/
void ResourceLoadingSystem::LoadLevel(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, LevelData *const RESTRICT data) NOEXCEPT
{
	//Read the number of level entries.
	uint64 number_of_level_entries;
	file->Read(&number_of_level_entries, sizeof(uint64));

	//Reserve the appropriate amount of memory.
	data->_LevelEntries.Reserve(number_of_level_entries);

	//Read the level entries.
	for (uint64 level_entry_index{ 0 }; level_entry_index < number_of_level_entries; ++level_entry_index)
	{
		//Add the new level entry.
		data->_LevelEntries.Emplace();
		LevelData::LevelEntry &new_level_entry{ data->_LevelEntries.Back() };

		//Read the name.
		file->Read(new_level_entry._Name.Data(), new_level_entry._Name.Size());

		//Read the number of component entries.
		uint64 number_of_component_entries;
		file->Read(&number_of_component_entries, sizeof(uint64));

		//Reserve the appropriate amount of memory.
		new_level_entry._ComponentEntries.Reserve(number_of_component_entries);

		for (uint64 component_entry_index{ 0 }; component_entry_index < number_of_component_entries; ++component_entry_index)
		{
			//Add the new component entry.
			new_level_entry._ComponentEntries.Emplace();
			LevelData::LevelEntry::ComponentEntry &new_component_entry{ new_level_entry._ComponentEntries.Back() };

			//Read the component identifier.
			file->Read(&new_component_entry._ComponentIdentifier, sizeof(HashString));

			//Read the number of editable fields.
			file->Read(&new_component_entry._NumberOfEditableFields, sizeof(uint64));

			//Read the field data position.
			file->Read(&new_component_entry._FieldDataPosition, sizeof(uint64));
		}
	}

	//Read the stream archive size.
	uint64 stream_archive_size;
	file->Read(&stream_archive_size, sizeof(uint64));

	//Read into the stream archive.
	data->_StreamArchive.Resize(stream_archive_size);
	file->Read(data->_StreamArchive.Data(), stream_archive_size);
}

/*
*	Given a file, load raw data data.
*/
void ResourceLoadingSystem::LoadRawData(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, RawDataData* const RESTRICT data) NOEXCEPT
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
void ResourceLoadingSystem::LoadRenderPipeline(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, RenderPipelineData *const RESTRICT data) NOEXCEPT
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
		bool has_data{ false };
		file->Read(&has_data, sizeof(bool));

		if (has_data)
		{
			uint64 data_size{ 0 };
			file->Read(&data_size, sizeof(uint64));
			data->_RayMissShaderData._GLSLData.Upsize<false>(data_size);

			file->Read(data->_RayMissShaderData._GLSLData.Data(), data_size);
		}
	}

	//Read the ray hit group
	{
		uint64 number_of_ray_hit_groups{ 0 };
		file->Read(&number_of_ray_hit_groups, sizeof(uint64));

		if (number_of_ray_hit_groups > 0)
		{
			data->_RayHitGroupShaderData.Upsize<true>(number_of_ray_hit_groups);

			for (uint64 ray_hit_group_index{ 0 }; ray_hit_group_index < number_of_ray_hit_groups; ++ray_hit_group_index)
			{
				file->Read(&data->_RayHitGroupShaderData[ray_hit_group_index]._Identifier, sizeof(HashString));

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

	//Read the output depth buffer.
	file->Read(&data->_OutputDepthBuffer, sizeof(HashString));

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

	//Read the render resolution.
	file->Read(&data->_RenderResolution, sizeof(HashString));

	//Read the load/store operators.
	file->Read(&data->_ColorLoadOperator, sizeof(AttachmentLoadOperator));
	file->Read(&data->_ColorStoreOperator, sizeof(AttachmentStoreOperator));
	file->Read(&data->_DepthStencilLoadOperator, sizeof(AttachmentLoadOperator));
	file->Read(&data->_DepthStencilStoreOperator, sizeof(AttachmentStoreOperator));

	//Read the blend properties.
	file->Read(&data->_BlendEnabled, sizeof(bool));
	file->Read(&data->_BlendColorSourceFactor, sizeof(BlendFactor));
	file->Read(&data->_BlendColorDestinationFactor, sizeof(BlendFactor));
	file->Read(&data->_BlendColorOperator, sizeof(BlendOperator));
	file->Read(&data->_BlendAlphaSourceFactor, sizeof(BlendFactor));
	file->Read(&data->_BlendAlphaDestinationFactor, sizeof(BlendFactor));
	file->Read(&data->_BlendAlphaOperator, sizeof(BlendOperator));

	//Read the cull mode.
	file->Read(&data->_CullMode, sizeof(CullMode));

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
void ResourceLoadingSystem::LoadShader(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, ShaderData *const RESTRICT data) NOEXCEPT
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
*	Given a file, load sound data.
*/
void ResourceLoadingSystem::LoadSound(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, SoundData *const RESTRICT data) NOEXCEPT
{
	//Read the sample rate.
	file->Read(&data->_SampleRate, sizeof(float32));

	//Read the number of channels.
	file->Read(&data->_NumberOfChannels, sizeof(uint8));

	//Reserve the appropriate size for the channels.
	data->_Samples.Upsize<true>(data->_NumberOfChannels);

	//Read the number of samples.
	uint64 number_of_samples;
	file->Read(&number_of_samples, sizeof(uint64));

	//Read the samples.
	for (DynamicArray<int16> &channel : data->_Samples)
	{
		channel.Upsize<false>(number_of_samples);

		file->Read(channel.Data(), sizeof(int16) * number_of_samples);
	}
}

/*
*	Given a file, load texture 3D data.
*/
void ResourceLoadingSystem::LoadTexture3D(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, Texture3DData *const RESTRICT data) NOEXCEPT
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

/*
*	Given a file, load texture cube data.
*/
void ResourceLoadingSystem::LoadTextureCube(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, TextureCubeData *const RESTRICT data) NOEXCEPT
{
	//Read the resolution.
	file->Read(&data->_Resolution, sizeof(uint32));

	//Read the number of mipmap levels.
	file->Read(&data->_MipmapLevels, sizeof(uint8));

	//Upsize the data accordingly.
	data->_Data.Upsize<true>(data->_MipmapLevels);

	for (uint8 i{ 0 }; i < data->_MipmapLevels; ++i)
	{	
		//Read the data.
		data->_Data[i].Upsize<false>((data->_Resolution >> i) * (data->_Resolution >> i) * 4 * 6);
		file->Read(data->_Data[i].Data(), (data->_Resolution >> i) * (data->_Resolution >> i) * 4 * 6 * sizeof(float32));
	}
}

/*
*	Given a file, load video data.
*/
void ResourceLoadingSystem::LoadVideo(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, VideoData *const RESTRICT data) NOEXCEPT
{
	//Read the width.
	file->Read(&data->_Width, sizeof(uint32));

	//Read the height.
	file->Read(&data->_Height, sizeof(uint32));

	//Read the frames per second.
	file->Read(&data->_FramesPerSecond, sizeof(float32));

	//Read the frames per second reciprocal.
	file->Read(&data->_FramesPerSecondReciprocal, sizeof(float32));

	//Read the number of frames.
	uint64 number_of_frames;
	file->Read(&number_of_frames, sizeof(uint64));

	//Upsize the frame data accordingly.
	data->_Frames.Upsize<true>(number_of_frames);

	//Read all frames.
	for (uint64 i{ 0 }; i < number_of_frames; ++i)
	{	
		//Initialize the frame.
		data->_Frames[i]._DataX.Initialize(data->_Width, data->_Height);
		data->_Frames[i]._DataY.Initialize(data->_Width, data->_Height);
		data->_Frames[i]._DataZ.Initialize(data->_Width, data->_Height);

		//Read the frame.
		file->Read(data->_Frames[i]._DataX.Data(), sizeof(uint8) * data->_Width * data->_Height);
		file->Read(data->_Frames[i]._DataY.Data(), sizeof(uint8) * data->_Width * data->_Height);
		file->Read(data->_Frames[i]._DataZ.Data(), sizeof(uint8) * data->_Width * data->_Height);
	}
}