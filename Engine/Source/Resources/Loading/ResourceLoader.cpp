//Header file.
#include <Resources/Loading/ResourceLoader.h>

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
Map<HashString, Model> ResourceLoader::_Models;
Map<HashString, SoundBankHandle> ResourceLoader::_SoundBanks;
Map<HashString, TextureCubeHandle> ResourceLoader::_TextureCubes;
Map<HashString, Texture2D<Vector4<byte>>> ResourceLoader::_Texture2Ds;

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
		}
	}
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

	//Read the number of vertices.
	uint64 numberOfVertices;
	file.Read(&numberOfVertices, sizeof(uint64));

	//Read the vertices.
	data._Vertices.UpsizeFast(numberOfVertices);
	file.Read(data._Vertices.Data(), sizeof(Vertex) * numberOfVertices);

	//Read the number of indices.
	uint64 numberOfIndices;
	file.Read(&numberOfIndices, sizeof(uint64));

	//Read the indices.
	data._Indices.UpsizeFast(numberOfIndices);
	file.Read(data._Indices.Data(), sizeof(uint32) * numberOfIndices);

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