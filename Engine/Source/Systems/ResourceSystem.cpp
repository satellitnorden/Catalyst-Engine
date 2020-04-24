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
				//Read the identifier.
				HashString identifier;
				file.Read(&identifier, sizeof(HashString));

				//Allocate the new resource.
				AnimatedModelResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<AnimatedModelResource>()) AnimatedModelResource() };

				//Load the resource.
				AnimatedModelData data;
				_ResourceLoadingSystem.LoadAnimatedModel(&file, &data);

				//Create the resource.
				_ResourceCreationSystem.CreateAnimatedModel(&data, new_resource);

				//Add the new resource.
				_AnimatedModelResources.Add(identifier, new_resource);

				//Register that the resource is now loaded.
				new_resource->_LoadState = ResourceLoadState::LOADED;

				break;
			}

			case ResourceType::ANIMATION:
			{
				//Read the identifier.
				HashString identifier;
				file.Read(&identifier, sizeof(HashString));

				//Allocate the new resource.
				AnimationResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<AnimationResource>()) AnimationResource() };

				//Load the resource.
				AnimationData data;
				_ResourceLoadingSystem.LoadAnimation(&file, &data);

				//Create the resource.
				_ResourceCreationSystem.CreateAnimation(&data, new_resource);

				//Add the new resource.
				_AnimationResources.Add(identifier, new_resource);

				//Register that the resource is now loaded.
				new_resource->_LoadState = ResourceLoadState::LOADED;

				break;
			}

			case ResourceType::FONT:
			{
				//Read the identifier.
				HashString identifier;
				file.Read(&identifier, sizeof(HashString));

				//Allocate the new resource.
				FontResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<FontResource>()) FontResource() };

				//Load the resource.
				FontData data;
				_ResourceLoadingSystem.LoadFont(&file, &data);

				//Create the resource.
				_ResourceCreationSystem.CreateFont(&data, new_resource);

				//Add the new resource.
				_FontResources.Add(identifier, new_resource);

				//Register that the resource is now loaded.
				new_resource->_LoadState = ResourceLoadState::LOADED;

				break;
			}

			case ResourceType::MODEL:
			{
				_ResourceLoadingSystem.LoadModel(file);

				break;
			}

			case ResourceType::SHADER:
			{
				//Read the identifier.
				HashString identifier;
				file.Read(&identifier, sizeof(HashString));

				//Allocate the new resource.
				ShaderResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<ShaderResource>()) ShaderResource() };

				//Load the resource.
				ShaderData data;
				_ResourceLoadingSystem.LoadShader(&file, &data);

				//Create the resource.
				_ResourceCreationSystem.CreateShader(&data, new_resource);

				//Add the new resource.
				_ShaderResources.Add(identifier, new_resource);

				//Register that the resource is now loaded.
				new_resource->_LoadState = ResourceLoadState::LOADED;

				break;
			}

			case ResourceType::SOUND:
			{
				//Read the identifier.
				HashString identifier;
				file.Read(&identifier, sizeof(HashString));

				//Allocate the new resource.
				SoundResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<SoundResource>()) SoundResource() };

				//Load the resource.
				SoundData data;
				_ResourceLoadingSystem.LoadSound(&file, &data);

				//Create the resource.
				_ResourceCreationSystem.CreateSound(&data, new_resource);

				//Add the new resource.
				_SoundResources.Add(identifier, new_resource);

				//Register that the resource is now loaded.
				new_resource->_LoadState = ResourceLoadState::LOADED;

				break;
			}

			case ResourceType::TEXTURE_CUBE:
			{
				//Read the identifier.
				HashString identifier;
				file.Read(&identifier, sizeof(HashString));

				//Allocate the new resource.
				TextureCubeResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<TextureCubeResource>()) TextureCubeResource() };

				//Load the resource.
				TextureCubeData data;
				_ResourceLoadingSystem.LoadTextureCube(&file, &data);

				//Create the resource.
				_ResourceCreationSystem.CreateTextureCube(&data, new_resource);

				//Add the new resource.
				_TextureCubeResources.Add(identifier, new_resource);

				//Register that the resource is now loaded.
				new_resource->_LoadState = ResourceLoadState::LOADED;

				break;
			}

			case ResourceType::TEXTURE_2D:
			{
				//Read the identifier.
				HashString identifier;
				file.Read(&identifier, sizeof(HashString));

				//Allocate the new resource.
				Texture2DResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<Texture2DResource>()) Texture2DResource() };

				//Load the resource.
				Texture2DData data;
				_ResourceLoadingSystem.LoadTexture2D(&file, &data);

				//Create the resource.
				_ResourceCreationSystem.CreateTexture2D(&data, new_resource);

				//Add the new resource.
				_Texture2DResources.Add(identifier, new_resource);

				//Register that the resource is now loaded.
				new_resource->_LoadState = ResourceLoadState::LOADED;

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