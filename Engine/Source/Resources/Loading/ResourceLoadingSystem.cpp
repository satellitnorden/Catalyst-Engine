//Header file.
#include <Resources/Loading/ResourceLoadingSystem.h>

//Animation.
#include <Animation/AnimatedVertex.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Reads a bone from file.
*/
FORCE_INLINE void ReadBoneFromFile(BinaryFile<IOMode::In> *const RESTRICT file, Bone *const RESTRICT bone) NOEXCEPT
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
void ResourceLoadingSystem::LoadAnimatedModel(BinaryFile<IOMode::In> *const RESTRICT file, AnimatedModelData *const RESTRICT data) NOEXCEPT
{
	//Read the axis-aligned bounding box
	file->Read(&data->_AxisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox3));

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
void ResourceLoadingSystem::LoadAnimation(BinaryFile<IOMode::In> *const RESTRICT file, AnimationData *const RESTRICT data) NOEXCEPT
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
void ResourceLoadingSystem::LoadFont(BinaryFile<IOMode::In> *const RESTRICT file, FontData *const RESTRICT data) NOEXCEPT
{
	//Read all characters.
	for (int8 i{ 0 }; i < INT8_MAXIMUM; ++i)
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
*	Given a file, load model data.
*/
void ResourceLoadingSystem::LoadModel(BinaryFile<IOMode::In> *const RESTRICT file, ModelData *const RESTRICT data) NOEXCEPT
{
	//Read the axis-aligned bounding box
	file->Read(&data->_AxisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox3));

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
}

/*
*	Given a file, load shader data.
*/
void ResourceLoadingSystem::LoadShader(BinaryFile<IOMode::In> *const RESTRICT file, ShaderData *const RESTRICT data) NOEXCEPT
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
void ResourceLoadingSystem::LoadSound(BinaryFile<IOMode::In> *const RESTRICT file, SoundData *const RESTRICT data) NOEXCEPT
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
void ResourceLoadingSystem::LoadTexture2D(BinaryFile<IOMode::In> *const RESTRICT file, Texture2DData *const RESTRICT data) NOEXCEPT
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

		data->_Data[i].Reserve(textureSize);

		file->Read(data->_Data[i].Data(), textureSize);
	}
}

/*
*	Given a file, load texture 3D data.
*/
void ResourceLoadingSystem::LoadTexture3D(BinaryFile<IOMode::In> *const RESTRICT file, Texture3DData *const RESTRICT data) NOEXCEPT
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
void ResourceLoadingSystem::LoadTextureCube(BinaryFile<IOMode::In> *const RESTRICT file, TextureCubeData *const RESTRICT data) NOEXCEPT
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