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
	for (uint64 i{ 0 }; i < number_of_level_entries; ++i)
	{
		//Add the new level entry.
		data->_LevelEntries.Emplace();
		LevelEntry &new_level_entry{ data->_LevelEntries.Back() };

		//Read the type.
		file->Read(&new_level_entry._Type, sizeof(LevelEntry::Type));

		//Read the version.
		file->Read(&new_level_entry._Version, sizeof(uint64));

		//Read the data.
		switch (new_level_entry._Type)
		{
			case LevelEntry::Type::DYNAMIC_MODEL:
			{
				file->Read(&new_level_entry._DynamicModelData._WorldTransform, sizeof(WorldTransform));
				file->Read(&new_level_entry._DynamicModelData._ModelResourceIdentifier, sizeof(HashString));

				{
					uint64 number_of_material_resource_identifiers;
					file->Read(&number_of_material_resource_identifiers, sizeof(uint64));

					new_level_entry._DynamicModelData._MaterialResourceIdentifiers.Upsize<false>(number_of_material_resource_identifiers);
				}

				file->Read(new_level_entry._DynamicModelData._MaterialResourceIdentifiers.Data(), sizeof(HashString) * new_level_entry._DynamicModelData._MaterialResourceIdentifiers.Size());
				file->Read(&new_level_entry._DynamicModelData._ModelCollisionConfiguration, sizeof(ModelCollisionConfiguration));
				file->Read(&new_level_entry._DynamicModelData._ModelSimulationConfiguration, sizeof(ModelSimulationConfiguration));

				break;
			}

			case LevelEntry::Type::LIGHT:
			{
				file->Read(&new_level_entry._LightData, sizeof(LevelEntry::LightData));

				break;
			}

			case LevelEntry::Type::STATIC_MODEL:
			{
				file->Read(&new_level_entry._StaticModelData._WorldTransform, sizeof(WorldTransform));
				file->Read(&new_level_entry._StaticModelData._ModelResourceIdentifier, sizeof(HashString));

				{
					uint64 number_of_material_resource_identifiers;
					file->Read(&number_of_material_resource_identifiers, sizeof(uint64));

					new_level_entry._StaticModelData._MaterialResourceIdentifiers.Upsize<false>(number_of_material_resource_identifiers);
				}

				file->Read(new_level_entry._StaticModelData._MaterialResourceIdentifiers.Data(), sizeof(HashString) * new_level_entry._StaticModelData._MaterialResourceIdentifiers.Size());
				file->Read(&new_level_entry._StaticModelData._ModelCollisionConfiguration, sizeof(ModelCollisionConfiguration));

				break;
			}

			case LevelEntry::Type::USER_INTERFACE:
			{
				file->Read(&new_level_entry._UserInterfaceData, sizeof(LevelEntry::UserInterfaceData));

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}
}

/*
*	Given a file, load material data.
*/
void ResourceLoadingSystem::LoadMaterial(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, MaterialData* const RESTRICT data) NOEXCEPT
{
	//Just read the data. (:
	file->Read(data, sizeof(MaterialData));
}

/*
*	Given a file, load model data.
*/
void ResourceLoadingSystem::LoadModel(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, ModelData *const RESTRICT data) NOEXCEPT
{
	//Read the axis-aligned bounding box
	file->Read(&data->_AxisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox3D));

	//Read the number of meshes.
	file->Read(&data->_NumberOfMeshes, sizeof(uint64));

	//Read the number of level of details.
	file->Read(&data->_NumberOfLevelfDetails, sizeof(uint64));

	data->_Vertices.Upsize<true>(data->_NumberOfMeshes);
	data->_Indices.Upsize<true>(data->_NumberOfMeshes);

	for (uint64 i{ 0 }; i < data->_NumberOfMeshes; ++i)
	{
		data->_Vertices[i].Upsize<true>(data->_NumberOfLevelfDetails);
		data->_Indices[i].Upsize<true>(data->_NumberOfLevelfDetails);

		for (uint64 j{ 0 }; j < data->_NumberOfLevelfDetails; ++j)
		{
			//Read the number of vertices.
			uint64 number_of_vertices;
			file->Read(&number_of_vertices, sizeof(uint64));

			//Read the vertices.
			data->_Vertices[i][j].Upsize<false>(number_of_vertices);
			file->Read(data->_Vertices[i][j].Data(), sizeof(Vertex) * number_of_vertices);

			//Read the number of indices.
			uint64 number_of_indices;
			file->Read(&number_of_indices, sizeof(uint64));

			//Read the indices.
			data->_Indices[i][j].Upsize<false>(number_of_indices);
			file->Read(data->_Indices[i][j].Data(), sizeof(uint32) * number_of_indices);
		}
	}

	//Read if there exists a collision model.
	bool collision_model_exists;
	file->Read(&collision_model_exists, sizeof(bool));

	//Read the collision model data.
	if (collision_model_exists)
	{
		file->Read(&data->_CollisionModelData._Type, sizeof(CollisionModelData::Type));

		uint64 collision_model_data_size;
		file->Read(&collision_model_data_size, sizeof(uint64));

		data->_CollisionModelData._Data.Upsize<false>(collision_model_data_size);

		file->Read(data->_CollisionModelData._Data.Data(), collision_model_data_size);
	}

	else
	{
		data->_CollisionModelData._Type = CollisionModelData::Type::NONE;
	}
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

	//Read the input render targets.
	{
		uint64 length{ 0 };
		file->Read(&length, sizeof(uint64));

		if (length > 0)
		{
			data->_InputRenderTargets.Upsize<false>(length);
			file->Read(data->_InputRenderTargets.Data(), sizeof(HashString) * data->_InputRenderTargets.Size());
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

	//Read the load/store operators.
	file->Read(&data->_ColorLoadOperator, sizeof(AttachmentLoadOperator));
	file->Read(&data->_ColorStoreOperator, sizeof(AttachmentStoreOperator));
	file->Read(&data->_DepthStencilLoadOperator, sizeof(AttachmentLoadOperator));
	file->Read(&data->_DepthStencilStoreOperator, sizeof(AttachmentStoreOperator));

	//Write the blend properties.
	file->Read(&data->_BlendEnabled, sizeof(bool));
	file->Read(&data->_BlendColorSourceFactor, sizeof(BlendFactor));
	file->Read(&data->_BlendColorDestinationFactor, sizeof(BlendFactor));
	file->Read(&data->_BlendColorOperator, sizeof(BlendOperator));
	file->Read(&data->_BlendAlphaSourceFactor, sizeof(BlendFactor));
	file->Read(&data->_BlendAlphaDestinationFactor, sizeof(BlendFactor));
	file->Read(&data->_BlendAlphaOperator, sizeof(BlendOperator));
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
*	Given a file, load texture 2D data.
*/
void ResourceLoadingSystem::LoadTexture2D(BinaryFile<BinaryFileMode::IN> *const RESTRICT file, Texture2DData *const RESTRICT data) NOEXCEPT
{
	//Read the number of mipmap levels.
	file->Read(&data->_MipmapLevels, sizeof(uint8));

	//Read the width.
	file->Read(&data->_Width, sizeof(uint32));

	//Read the height.
	file->Read(&data->_Height, sizeof(uint32));

	//Read the data.
	data->_Data.Upsize<true>(data->_MipmapLevels);

	for (uint8 i{ 0 }; i < data->_MipmapLevels; ++i)
	{
		const uint64 textureSize{ (data->_Width >> i) * (data->_Height >> i) * 4 };

		data->_Data[i].Upsize<false>(textureSize);

		file->Read(data->_Data[i].Data(), textureSize);
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