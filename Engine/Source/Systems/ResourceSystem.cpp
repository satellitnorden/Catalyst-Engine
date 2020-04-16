//Header file.
#include <Systems/ResourceSystem.h>

//Systems.
#include <Systems/MemorySystem.h>

//Singleton definition.
DEFINE_SINGLETON(ResourceSystem);

/*
*	Loads the resource collection with the given file path.
*/
void ResourceSystem::LoadsResourceCollection(const char *const RESTRICT file_path) NOEXCEPT
{
	//Load the file.
	BinaryFile<IOMode::In> file{ file_path };

	//Read the number of resources.
	uint64 number_of_resources;
	file.Read(&number_of_resources, sizeof(uint64));

	//For each resource, load it.
	for (uint64 i{ 0 }; i < number_of_resources; ++i)
	{
		//Read the resource type.
		ResourceType resource_type;
		file.Read(&resource_type, sizeof(ResourceType));

		switch (resource_type)
		{
			case ResourceType::ANIMATED_MODEL:
			{
				_ResourceLoadingSystem.LoadAnimatedModel(file);

				break;
			}

			case ResourceType::ANIMATION:
			{
				_ResourceLoadingSystem.LoadAnimation(file);

				break;
			}

			case ResourceType::FONT:
			{
				_ResourceLoadingSystem.LoadFont(file);

				break;
			}

			case ResourceType::MODEL:
			{
				_ResourceLoadingSystem.LoadModel(file);

				break;
			}

			case ResourceType::SOUND:
			{
				//Read the identifier.
				HashString identifier;
				file.Read(&identifier, sizeof(HashString));

				//Allocate the new resource.
				SoundResource* const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<SoundResource>()) SoundResource() };

				//Load the resource.
				SoundData data;
				_ResourceLoadingSystem.LoadSound(&file, &data);

				//Create the resource.
				_ResourceCreationSystem.CreateSound(&data, new_resource);

				//Add the new resource.
				_SoundResources.Add(identifier, new_resource);

				break;
			}

			case ResourceType::TEXTURE_CUBE:
			{
				_ResourceLoadingSystem.LoadTextureCube(file);

				break;
			}

			case ResourceType::TEXTURE_2D:
			{
				//Read the identifier.
				HashString identifier;
				file.Read(&identifier, sizeof(HashString));

				//Allocate the new resource.
				Texture2DResource* const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<Texture2DResource>()) Texture2DResource() };

				//Load the resource.
				Texture2DData data;
				_ResourceLoadingSystem.LoadTexture2D(&file, &data);

				//Create the resource.
				_ResourceCreationSystem.CreateTexture2D(&data, new_resource);

				//Add the new resource.
				_Texture2DResources.Add(identifier, new_resource);

				break;
			}

			case ResourceType::TEXTURE_3D:
			{
				_ResourceLoadingSystem.LoadTexture3D(file);

				break;
			}

			default:
			{
				ASSERT(false, "Undefined resource type.");

				break;
			}
		}
	}
}