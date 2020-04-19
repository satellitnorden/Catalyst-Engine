//Header file.
#include <Resources/Loading/ResourceLoadingSystem.h>

//Animation.
#include <Animation/AnimatedVertex.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

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
*	Given a file, load a font.
*/
void ResourceLoadingSystem::LoadFont(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Load the font data.
	FontData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read all characters.
	for (int8 i{ 0 }; i < INT8_MAXIMUM; ++i)
	{
		//Read the character description.
		file.Read(&data._CharacterDescriptions[i], sizeof(Font::CharacterDescription));

		//Read the texture dimensions.
		file.Read(&data._CharacterDimensions[i], sizeof(Vector2<float>));

		//Read the texture data.
		data._TextureData[i].Upsize<true>(1);

		for (uint8 j{ 0 }; j < 1; ++j)
		{
			data._TextureData[i][j].Initialize(data._CharacterDimensions[i]._X >> j, data._CharacterDimensions[i]._Y >> j);
			file.Read(data._TextureData[i][j].Data(), (data._CharacterDimensions[i]._X >> j) * (data._CharacterDimensions[i]._Y >> j));
		}
	}

	//Create the font.
	ResourceSystem::Instance->GetResourceCreationSystem()->CreateFont(&data, &_Fonts[resourceID]);
}

/*
*	Given a file, load a model.
*/
void ResourceLoadingSystem::LoadModel(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Load the model data.
	ModelData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the axis-aligned bounding box
	file.Read(&data._AxisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox3));

	//Read the number of meshes.
	file.Read(&data._NumberOfMeshes, sizeof(uint64));

	//Read the number of level of details.
	file.Read(&data._NumberOfLevelfDetails, sizeof(uint64));

	data._Vertices.Upsize<true>(data._NumberOfMeshes);
	data._Indices.Upsize<true>(data._NumberOfMeshes);

	for (uint64 i{ 0 }; i < data._NumberOfMeshes; ++i)
	{
		data._Vertices[i].Upsize<true>(data._NumberOfLevelfDetails);
		data._Indices[i].Upsize<true>(data._NumberOfLevelfDetails);

		for (uint64 j{ 0 }; j < data._NumberOfLevelfDetails; ++j)
		{
			//Read the number of vertices.
			uint64 number_of_vertices;
			file.Read(&number_of_vertices, sizeof(uint64));

			//Read the vertices.
			data._Vertices[i][j].Upsize<false>(number_of_vertices);
			file.Read(data._Vertices[i][j].Data(), sizeof(Vertex) * number_of_vertices);

			//Read the number of indices.
			uint64 number_of_indices;
			file.Read(&number_of_indices, sizeof(uint64));

			//Read the indices.
			data._Indices[i][j].Upsize<false>(number_of_indices);
			file.Read(data._Indices[i][j].Data(), sizeof(uint32) * number_of_indices);
		}
	}

	//Create the model.
	ResourceSystem::Instance->GetResourceCreationSystem()->CreateModel(&data, &_Models[resourceID]);
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
	for (DynamicArray<float32> &channel : data->_Samples)
	{
		channel.Upsize<false>(number_of_samples);

		file->Read(channel.Data(), sizeof(float32) * number_of_samples);
	}
}

/*
*	Given a file, load a texture cube.
*/
void ResourceLoadingSystem::LoadTextureCube(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Load the texture cube data
	TextureCubeData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the resolution.
	file.Read(&data._Resolution, sizeof(uint32));

	//Calculate the data size.
	const uint64 dataSize{ data._Resolution * data._Resolution * 4 * 6 * sizeof(float) };

	//Upsize the data accordingly.
	data._Data.Upsize<false>(data._Resolution * data._Resolution * 4 * 6);

	//Read the data.
	file.Read(data._Data.Data(), dataSize);

	//Create the texture cube.
	ResourceSystem::Instance->GetResourceCreationSystem()->CreateTextureCube(&data, &_TextureCubes[resourceID]);
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
*	Given a file, load a texture 3D.
*/
void ResourceLoadingSystem::LoadTexture3D(BinaryFile<IOMode::In>& file) NOEXCEPT
{
	//Load the texture 3D data.
	Texture3DData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the number of mipmap levels.
	file.Read(&data._MipmapLevels, sizeof(uint8));

	//Read the width.
	file.Read(&data._Width, sizeof(uint32));

	//Read the height.
	file.Read(&data._Height, sizeof(uint32));

	//Read the depth.
	file.Read(&data._Depth, sizeof(uint32));

	//Read the data.
	data._Data.Upsize<true>(data._MipmapLevels);

	for (uint8 i{ 0 }; i < data._MipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._Width >> i) * (data._Height >> i) * (data._Depth >> i) * sizeof(Vector4<byte>) };

		data._Data[i].Reserve(textureSize);

		file.Read(data._Data[i].Data(), textureSize);
	}

	//Create the texture 3D.
	ResourceSystem::Instance->GetResourceCreationSystem()->CreateTexture3D(&data, &_Texture3Ds[resourceID]);
}