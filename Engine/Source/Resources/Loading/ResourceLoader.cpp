//Header file.
#include <Resources/Loading/ResourceLoader.h>

//Animation.
#include <Animation/AnimatedVertex.h>

//Multithreading.
#include <Multithreading/Task.h>

//Resources.
#include <Resources/Creation/ResourceCreator.h>
#include <Resources/Data/SoundBankData.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>

//Static variable definitions.
Map<HashString, AnimatedModel> ResourceLoader::_AnimatedModels;
Map<HashString, Animation> ResourceLoader::_Animations;
Map<HashString, Font> ResourceLoader::_Fonts;
Map<HashString, Model> ResourceLoader::_Models;
Map<HashString, SoundBankHandle> ResourceLoader::_SoundBanks;
Map<HashString, TextureCubeHandle> ResourceLoader::_TextureCubes;
Map<HashString, Texture2DResource> ResourceLoader::_Texture2Ds;
Map<HashString, Texture3DHandle> ResourceLoader::_Texture3Ds;

/*
*	Given a file path, load a resource collection.
*/
void ResourceLoader::LoadResourceCollection(const char *RESTRICT filePath) NOEXCEPT
{
	//Load the resource collection.
	LoadResourceCollectionInternal(filePath);
}

/*
*	Loads a resource collection, internal implementation.
*/
void ResourceLoader::LoadResourceCollectionInternal(const char *RESTRICT filePath) NOEXCEPT
{
	//Load the file.
	BinaryFile<IOMode::In> file{ filePath };

	//Read the number of resources.
	uint64 numberOfResources;
	file.Read(&numberOfResources, sizeof(uint64));

	//For each resource, load it.
	for (uint64 i = 0; i < numberOfResources; ++i)
	{
		//Read the resource type.
		ResourceType resourceType;
		file.Read(&resourceType, sizeof(ResourceType));

		switch (resourceType)
		{
#if defined(CATALYST_CONFIGURATION_DEBUG)
			default:
			{
				ASSERT(false, "Undefined resource type.");
			}
#endif
			case ResourceType::AnimatedModel:
			{
				LoadAnimatedModel(file);

				break;
			}

			case ResourceType::Animation:
			{
				LoadAnimation(file);

				break;
			}

			case ResourceType::Font:
			{
				LoadFont(file);

				break;
			}

			case ResourceType::Model:
			{
				LoadModel(file);

				break;
			}

			case ResourceType::SoundBank:
			{
				LoadSoundBank(file);

				break;
			}

			case ResourceType::TextureCube:
			{
				LoadTextureCube(file);

				break;
			}

			case ResourceType::Texture2D:
			{
				LoadTexture2D(file);

				break;
			}

			case ResourceType::Texture3D:
			{
				LoadTexture3D(file);

				break;
			}
		}
	}
}

/*
*	Reads a bone from file.
*/
FORCE_INLINE void ReadBoneFromFile(BinaryFile<IOMode::In> &file, Bone *const RESTRICT bone) NOEXCEPT
{
	file.Read(bone, sizeof(Bone) - sizeof(DynamicArray<Bone>));

	uint64 number_of_child_bones;
	file.Read(&number_of_child_bones, sizeof(uint64));

	bone->_Children.UpsizeSlow(number_of_child_bones);

	for (Bone &child_bone : bone->_Children)
	{
		ReadBoneFromFile(file, &child_bone);
	}
}

/*
*	Given a file, load an animated model.
*/
void ResourceLoader::LoadAnimatedModel(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Load the model data.
	AnimatedModelData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the axis-aligned bounding box
	file.Read(&data._AxisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox));

	//Read the number of vertices.
	uint64 numberOfVertices;
	file.Read(&numberOfVertices, sizeof(uint64));

	//Read the vertices.
	data._Vertices.UpsizeFast(numberOfVertices);
	file.Read(data._Vertices.Data(), sizeof(AnimatedVertex) * numberOfVertices);

	//Read the number of indices.
	uint64 numberOfIndices;
	file.Read(&numberOfIndices, sizeof(uint64));

	//Read the indices.
	data._Indices.UpsizeFast(numberOfIndices);
	file.Read(data._Indices.Data(), sizeof(uint32) * numberOfIndices);

	//Read the skeleton.
	ReadBoneFromFile(file, &data._Skeleton._RootBone);

	//Create the model.
	ResourceCreator::CreateAnimatedModel(&data, &_AnimatedModels[resourceID]);
}

/*
*	Given a file, load an animation.
*/
void ResourceLoader::LoadAnimation(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Load the animation data.
	AnimationData data;

	//Read the resource ID.
	HashString resource_ID;
	file.Read(&resource_ID, sizeof(HashString));

	//Read the duration.
	file.Read(&data._Animation._Duration, sizeof(float));

	//Read the number of animation keyframe channels.
	uint64 number_of_animation_keyframe_channels;
	file.Read(&number_of_animation_keyframe_channels, sizeof(uint64));

	//Read the animation keyframes.
	for (uint64 i{ 0 }; i < number_of_animation_keyframe_channels; ++i)
	{
		//Read the name of the channel.
		HashString name;
		file.Read(&name, sizeof(HashString));

		//Read the number of animation keyframes.
		uint64 number_of_animation_keyframes;
		file.Read(&number_of_animation_keyframes, sizeof(uint64));

		//Read the keyframe.
		data._Animation._Keyframes[name].UpsizeFast(number_of_animation_keyframes);
		file.Read(data._Animation._Keyframes[name].Data(), sizeof(AnimationKeyframe) * number_of_animation_keyframes);
	}

	//Create the animation.
	ResourceCreator::CreateAnimation(&data, &_Animations[resource_ID]);
}

/*
*	Given a file, load a font.
*/
void ResourceLoader::LoadFont(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Load the font data.
	FontData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the number of mipmap levels.
	file.Read(&data._MipmapLevels, sizeof(uint8));

	//Read all characters.
	for (int8 i{ 0 }; i < INT8_MAXIMUM; ++i)
	{
		//Read the character description.
		file.Read(&data._CharacterDescriptions[i], sizeof(Font::CharacterDescription));

		//Read the texture dimensions.
		file.Read(&data._CharacterDimensions[i], sizeof(Vector2<float>));

		//Read the texture data.
		data._TextureData[i].UpsizeSlow(data._MipmapLevels);

		for (uint8 j{ 0 }; j < data._MipmapLevels; ++j)
		{
			data._TextureData[i][j].Initialize(data._CharacterDimensions[i]._X >> j, data._CharacterDimensions[i]._Y >> j);
			file.Read(data._TextureData[i][j].Data(), (data._CharacterDimensions[i]._X >> j) * (data._CharacterDimensions[i]._Y >> j));
		}
	}

	//Create the font.
	ResourceCreator::CreateFont(&data, &_Fonts[resourceID]);
}

/*
*	Given a file, load a model.
*/
void ResourceLoader::LoadModel(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Load the model data.
	ModelData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the axis-aligned bounding box
	file.Read(&data._AxisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox));

	//Read the number of meshes.
	file.Read(&data._NumberOfMeshes, sizeof(uint64));

	//Read the number of level of details.
	file.Read(&data._NumberOfLevelfDetails, sizeof(uint64));

	data._Vertices.UpsizeSlow(data._NumberOfMeshes);
	data._Indices.UpsizeSlow(data._NumberOfMeshes);

	for (uint64 i{ 0 }; i < data._NumberOfMeshes; ++i)
	{
		data._Vertices[i].UpsizeSlow(data._NumberOfLevelfDetails);
		data._Indices[i].UpsizeSlow(data._NumberOfLevelfDetails);

		for (uint64 j{ 0 }; j < data._NumberOfLevelfDetails; ++j)
		{
			//Read the number of vertices.
			uint64 number_of_vertices;
			file.Read(&number_of_vertices, sizeof(uint64));

			//Read the vertices.
			data._Vertices[i][j].UpsizeFast(number_of_vertices);
			file.Read(data._Vertices[i][j].Data(), sizeof(Vertex) * number_of_vertices);

			//Read the number of indices.
			uint64 number_of_indices;
			file.Read(&number_of_indices, sizeof(uint64));

			//Read the indices.
			data._Indices[i][j].UpsizeFast(number_of_indices);
			file.Read(data._Indices[i][j].Data(), sizeof(uint32) * number_of_indices);
		}
	}

	//Create the model.
	ResourceCreator::CreateModel(&data, &_Models[resourceID]);
}

/*
*	Given a file, load a sound bank.
*/
void ResourceLoader::LoadSoundBank(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the sound bank data in the sound bank data structure.
	SoundBankData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the size of the sound bank.
	uint64 soundBankSize{ 0 };
	file.Read(&soundBankSize, sizeof(uint64));

	//Reserve the required amount of memory.
	data._Data.UpsizeFast(soundBankSize);

	//Read the sound bank memory.
	file.Read(data._Data.Data(), soundBankSize);

	//Create the sound bank via the sound system.
	SoundSystem::Instance->CreateSoundBank(data, &_SoundBanks[resourceID]);
}

/*
*	Given a file, load a texture cube.
*/
void ResourceLoader::LoadTextureCube(BinaryFile<IOMode::In> &file) NOEXCEPT
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
	data._Data.UpsizeFast(data._Resolution * data._Resolution * 4 * 6);

	//Read the data.
	file.Read(data._Data.Data(), dataSize);

	//Create the texture cube.
	ResourceCreator::CreateTextureCube(&data, &_TextureCubes[resourceID]);
}

/*
*	Given a file, load a texture 2D
*/
void ResourceLoader::LoadTexture2D(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Load the texture 2D data.
	Texture2DData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the number of mipmap levels.
	file.Read(&data._MipmapLevels, sizeof(uint8));

	//Read the width.
	file.Read(&data._Width, sizeof(uint32));

	//Read the height.
	file.Read(&data._Height, sizeof(uint32));

	//Read the data.
	data._Data.UpsizeSlow(data._MipmapLevels);

	for (uint8 i{ 0 }; i < data._MipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._Width >> i) * (data._Height >> i) * 4 };

		data._Data[i].Reserve(textureSize);

		file.Read(data._Data[i].Data(), textureSize);
	}

	//Create the texture 2D.
	ResourceCreator::CreateTexture2D(&data, &_Texture2Ds[resourceID]);
}

/*
*	Given a file, load a texture 3D.
*/
void ResourceLoader::LoadTexture3D(BinaryFile<IOMode::In>& file) NOEXCEPT
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
	data._Data.UpsizeSlow(data._MipmapLevels);

	for (uint8 i{ 0 }; i < data._MipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._Width >> i) * (data._Height >> i) * (data._Depth >> i) * sizeof(Vector4<byte>) };

		data._Data[i].Reserve(textureSize);

		file.Read(data._Data[i].Data(), textureSize);
	}

	//Create the texture 3D.
	ResourceCreator::CreateTexture3D(&data, &_Texture3Ds[resourceID]);
}