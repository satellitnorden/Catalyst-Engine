//Header file.
#include <Systems/ResourceSystem.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryInputFile.h>

//Systems.
#include <Systems/MemorySystem.h>

//STL.
#include <filesystem>

//Singleton definition.
DEFINE_SINGLETON(ResourceSystem);

/*
*	Terminates the resource system.
*/
void ResourceSystem::Terminate() NOEXCEPT
{
	//Tell resource pointers to stop reference counting.
	ResourcePointerData::_DoReferenceCounting = false;
}

/*
*	Loads the resources contained in the given directory path.
*/
void ResourceSystem::LoadResources(const char *const RESTRICT directory_path) NOEXCEPT
{
	//Iterate over all the files in the directory and load them.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//If this entry is itself a directory, recursively process it.
		if (entry.is_directory())
		{
			LoadResources(entry.path().string().c_str());
		}

		else
		{
			//Get the extension for this file.
			const File::Extension file_extension{ File::GetExtension(entry.path().string().c_str()) };

			//Only load this file if it is actually a Catalyst resource or a Catalyst resource collection.
			if (file_extension == File::Extension::CR)
			{
				//Open the file.
				BinaryInputFile file{ entry.path().string().c_str() };

				//Load the resource.
				LoadResource(&file);

				//Close the file.
				file.Close();
			}

			if (file_extension == File::Extension::CRC)
			{
				//Open the file.
				BinaryInputFile file{ entry.path().string().c_str() };

				//Load the resource collection.
				LoadResourceCollection(&file);

				//Close the file.
				file.Close();
			}
		}
	}
}

/*
*	Loads a resource collection contained in the given file path.
*/
void ResourceSystem::LoadResourceCollection(const char *const RESTRICT file_path) NOEXCEPT
{
	//Open the file.
	BinaryInputFile file{ file_path };

	//Load the resource collection.
	LoadResourceCollection(&file);

	//Close the file.
	file.Close();
}

/*
*	Loads a single resource contained in the given file path.
*/
void ResourceSystem::LoadResource(const char *const RESTRICT file_path) NOEXCEPT
{
	//Open the file.
	BinaryInputFile file{ file_path };

	//Load the resource.
	LoadResource(&file);

	//Close the file.
	file.Close();
}

/*
*	Returns the animated model resource with the given identifier.
*/
NO_DISCARD ResourcePointer<AnimatedModelResource> ResourceSystem::GetAnimatedModelResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	AnimatedModelResource *const RESTRICT *const RESTRICT resource{ _AnimatedModelResources.Find(identifier) };

	ASSERT(resource, "Couldn't find resource!");

	return resource ? ResourcePointer<AnimatedModelResource>(*resource) : ResourcePointer<AnimatedModelResource>();
}

/*
*	Returns or creates the animated model resource with the given identifier.
*/
NO_DISCARD ResourcePointer<AnimatedModelResource> ResourceSystem::FindOrCreateAnimatedModelResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	AnimatedModelResource *const RESTRICT *const RESTRICT resource{ _AnimatedModelResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		AnimatedModelResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<AnimatedModelResource>()) AnimatedModelResource() };
		new_resource->_Header._ResourceIdentifier = identifier;
		_AnimatedModelResources.Add(identifier, new_resource);
		_AllResources.Emplace(new_resource);

		return ResourcePointer<AnimatedModelResource>(new_resource);
	}

	else
	{
		return ResourcePointer<AnimatedModelResource>(*resource);
	}
}

/*
*	Returns the animation resource with the given identifier.
*/
NO_DISCARD ResourcePointer<AnimationResource> ResourceSystem::GetAnimationResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	AnimationResource *const RESTRICT *const RESTRICT resource{ _AnimationResources.Find(identifier) };

	ASSERT(resource, "Couldn't find resource!");

	return resource ? ResourcePointer<AnimationResource>(*resource) : ResourcePointer<AnimationResource>();
}

/*
*	Returns or creates the animation resource with the given identifier.
*/
NO_DISCARD ResourcePointer<AnimationResource> ResourceSystem::FindOrCreateAnimationResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	AnimationResource *const RESTRICT *const RESTRICT resource{ _AnimationResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		AnimationResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<AnimationResource>()) AnimationResource() };
		new_resource->_Header._ResourceIdentifier = identifier;
		_AnimationResources.Add(identifier, new_resource);
		_AllResources.Emplace(new_resource);

		return ResourcePointer<AnimationResource>(new_resource);
	}

	else
	{
		return ResourcePointer<AnimationResource>(*resource);
	}
}

/*
*	Returns the raw data resource with the given identifier.
*/
NO_DISCARD ResourcePointer<RawDataResource> ResourceSystem::GetRawDataResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	RawDataResource *const RESTRICT *const RESTRICT resource{ _RawDataResources.Find(identifier) };

	ASSERT(resource, "Couldn't find resource!");

	return resource ? ResourcePointer<RawDataResource>(*resource) : ResourcePointer<RawDataResource>();
}

/*
*	Returns or creates the raw data resource with the given identifier.
*/
NO_DISCARD ResourcePointer<RawDataResource> ResourceSystem::FindOrCreateRawDataResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	RawDataResource *const RESTRICT *const RESTRICT resource{ _RawDataResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		RawDataResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<RawDataResource>()) RawDataResource() };
		new_resource->_Header._ResourceIdentifier = identifier;
		_RawDataResources.Add(identifier, new_resource);
		_AllResources.Emplace(new_resource);

		return ResourcePointer<RawDataResource>(new_resource);
	}

	else
	{
		return ResourcePointer<RawDataResource>(*resource);
	}
}

/*
*	Returns the render pipeline resource with the given identifier.
*/
NO_DISCARD ResourcePointer<RenderPipelineResource> ResourceSystem::GetRenderPipelineResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	RenderPipelineResource *const RESTRICT *const RESTRICT resource{ _RenderPipelineResources.Find(identifier) };

	ASSERT(resource, "Couldn't find resource!");

	return resource ? ResourcePointer<RenderPipelineResource>(*resource) : ResourcePointer<RenderPipelineResource>();
}

/*
*	Returns or creates the render pipeline resource with the given identifier.
*/
NO_DISCARD ResourcePointer<RenderPipelineResource> ResourceSystem::FindOrCreateRenderPipelineResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	RenderPipelineResource *const RESTRICT *const RESTRICT resource{ _RenderPipelineResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		RenderPipelineResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<RenderPipelineResource>()) RenderPipelineResource() };
		new_resource->_Header._ResourceIdentifier = identifier;
		_RenderPipelineResources.Add(identifier, new_resource);
		_AllResources.Emplace(new_resource);

		return ResourcePointer<RenderPipelineResource>(new_resource);
	}

	else
	{
		return ResourcePointer<RenderPipelineResource>(*resource);
	}
}

/*
*	Returns the shader resource with the given identifier.
*/
NO_DISCARD ResourcePointer<ShaderResource> ResourceSystem::GetShaderResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	ShaderResource *const RESTRICT *const RESTRICT resource{ _ShaderResources.Find(identifier) };

	ASSERT(resource, "Couldn't find resource!");

	return resource ? ResourcePointer<ShaderResource>(*resource) : ResourcePointer<ShaderResource>();
}

/*
*	Returns or creates the shader resource with the given identifier.
*/
NO_DISCARD ResourcePointer<ShaderResource> ResourceSystem::FindOrCreateShaderResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	ShaderResource *const RESTRICT *const RESTRICT resource{ _ShaderResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		ShaderResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<ShaderResource>()) ShaderResource() };
		new_resource->_Header._ResourceIdentifier = identifier;
		_ShaderResources.Add(identifier, new_resource);
		_AllResources.Emplace(new_resource);

		return ResourcePointer<ShaderResource>(new_resource);
	}

	else
	{
		return ResourcePointer<ShaderResource>(*resource);
	}
}

/*
*	Returns the sound resource with the given identifier.
*/
NO_DISCARD ResourcePointer<SoundResource> ResourceSystem::GetSoundResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	SoundResource *const RESTRICT *const RESTRICT resource{ _SoundResources.Find(identifier) };

	ASSERT(resource, "Couldn't find resource!");

	return resource ? ResourcePointer<SoundResource>(*resource) : ResourcePointer<SoundResource>();
}

/*
*	Returns or creates the sound resource with the given identifier.
*/
NO_DISCARD ResourcePointer<SoundResource> ResourceSystem::FindOrCreateSoundResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	SoundResource *const RESTRICT *const RESTRICT resource{ _SoundResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		SoundResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<SoundResource>()) SoundResource() };
		new_resource->_Header._ResourceIdentifier = identifier;
		_SoundResources.Add(identifier, new_resource);
		_AllResources.Emplace(new_resource);

		return ResourcePointer<SoundResource>(new_resource);
	}

	else
	{
		return ResourcePointer<SoundResource>(*resource);
	}
}

/*
*	Returns the texture 3D resource with the given identifier.
*/
NO_DISCARD ResourcePointer<Texture3DResource> ResourceSystem::GetTexture3DResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	Texture3DResource *const RESTRICT *const RESTRICT resource{ _Texture3DResources.Find(identifier) };

	ASSERT(resource, "Couldn't find resource!");

	return resource ? ResourcePointer<Texture3DResource>(*resource) : ResourcePointer<Texture3DResource>();
}

/*
*	Returns or creates the texture 3D resource with the given identifier.
*/
NO_DISCARD ResourcePointer<Texture3DResource> ResourceSystem::FindOrCreateTexture3DResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	Texture3DResource *const RESTRICT *const RESTRICT resource{ _Texture3DResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		Texture3DResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<Texture3DResource>()) Texture3DResource() };
		new_resource->_Header._ResourceIdentifier = identifier;
		_Texture3DResources.Add(identifier, new_resource);
		_AllResources.Emplace(new_resource);

		return ResourcePointer<Texture3DResource>(new_resource);
	}

	else
	{
		return ResourcePointer<Texture3DResource>(*resource);
	}
}

/*
*	Loads a resource collection from the given binary file.
*/
void ResourceSystem::LoadResourceCollection(BinaryInputFile *const RESTRICT file) NOEXCEPT
{
	//While the file hasn't reached the end, load the resources in the resource collection.
	while (!file->HasReachedEndOfFile())
	{
		LoadResource(file);
	}
}

/*
*	Loads a resource from the given binary file.
*/
void ResourceSystem::LoadResource(BinaryInputFile *const RESTRICT file) NOEXCEPT
{
	//Read the resource header.
	ResourceHeader header;
	file->Read(&header, sizeof(ResourceHeader));

	if (header._TypeIdentifier == ResourceConstants::ANIMATED_MODEL_TYPE_IDENTIFIER)
	{
		/*
		*	Find or allocate the new resource.
		*	The resource might have been created already by other dependant resources, but not loaded yet.
		*/
		AnimatedModelResource* RESTRICT new_resource;

		if (AnimatedModelResource* const RESTRICT* const RESTRICT found_resource{ _AnimatedModelResources.Find(header._ResourceIdentifier) })
		{
			new_resource = *found_resource;
		}

		else
		{
			new_resource = new (MemorySystem::Instance->TypeAllocate<AnimatedModelResource>()) AnimatedModelResource();
			_AnimatedModelResources.Add(header._ResourceIdentifier, new_resource);
			_AllResources.Emplace(new_resource);
		}

		//Set the resource header.
		new_resource->_Header = header;

		//Set the file path and file offset.
		new_resource->_FilePath = file->GetFilePath();
		new_resource->_FileOffset = file->GetCurrentPosition() - sizeof(ResourceHeader);

		//Load the resource.
		AnimatedModelData data;
		_ResourceLoadingSystem.LoadAnimatedModel(file, &data);

		//Create the resource.
		_ResourceCreationSystem.CreateAnimatedModel(&data, new_resource);

		//Register that the resource is now loaded.
		new_resource->_LoadState = ResourceLoadState::LOADED;
	}

	else if (header._TypeIdentifier == ResourceConstants::ANIMATION_TYPE_IDENTIFIER)
	{
		/*
		*	Find or allocate the new resource.
		*	The resource might have been created already by other dependant resources, but not loaded yet.
		*/
		AnimationResource* RESTRICT new_resource;

		if (AnimationResource* const RESTRICT* const RESTRICT found_resource{ _AnimationResources.Find(header._ResourceIdentifier) })
		{
			new_resource = *found_resource;
		}

		else
		{
			new_resource = new (MemorySystem::Instance->TypeAllocate<AnimationResource>()) AnimationResource();
			_AnimationResources.Add(header._ResourceIdentifier, new_resource);
			_AllResources.Emplace(new_resource);
		}

		//Set the resource header.
		new_resource->_Header = header;

		//Set the file path and file offset.
		new_resource->_FilePath = file->GetFilePath();
		new_resource->_FileOffset = file->GetCurrentPosition() - sizeof(ResourceHeader);

		//Load the resource.
		AnimationData data;
		_ResourceLoadingSystem.LoadAnimation(file, &data);

		//Create the resource.
		_ResourceCreationSystem.CreateAnimation(&data, new_resource);

		//Register that the resource is now loaded.
		new_resource->_LoadState = ResourceLoadState::LOADED;
	}

	else if (header._TypeIdentifier == ResourceConstants::RAW_DATA_TYPE_IDENTIFIER)
	{
		/*
		*	Find or allocate the new resource.
		*	The resource might have been created already by other dependant resources, but not loaded yet.
		*/
		RawDataResource* RESTRICT new_resource;

		if (RawDataResource* const RESTRICT* const RESTRICT found_resource{ _RawDataResources.Find(header._ResourceIdentifier) })
		{
			new_resource = *found_resource;
		}

		else
		{
			new_resource = new (MemorySystem::Instance->TypeAllocate<RawDataResource>()) RawDataResource();
			_RawDataResources.Add(header._ResourceIdentifier, new_resource);
			_AllResources.Emplace(new_resource);
		}

		//Set the resource header.
		new_resource->_Header = header;

		//Set the file path and file offset.
		new_resource->_FilePath = file->GetFilePath();
		new_resource->_FileOffset = file->GetCurrentPosition() - sizeof(ResourceHeader);

		//Load the resource.
		RawDataData data;
		_ResourceLoadingSystem.LoadRawData(file, &data);

		//Create the resource.
		_ResourceCreationSystem.CreateRawData(&data, new_resource);

		//Register that the resource is now loaded.
		new_resource->_LoadState = ResourceLoadState::LOADED;
	}

	else if (header._TypeIdentifier == ResourceConstants::RENDER_PIPELINE_TYPE_IDENTIFIER)
	{
		/*
		*	Find or allocate the new resource.
		*	The resource might have been created already by other dependant resources, but not loaded yet.
		*/
		RenderPipelineResource *RESTRICT new_resource;

		if (RenderPipelineResource *const RESTRICT *const RESTRICT found_resource{ _RenderPipelineResources.Find(header._ResourceIdentifier) })
		{
			new_resource = *found_resource;
		}

		else
		{
			new_resource = new (MemorySystem::Instance->TypeAllocate<RenderPipelineResource>()) RenderPipelineResource();
			_RenderPipelineResources.Add(header._ResourceIdentifier, new_resource);
			_AllResources.Emplace(new_resource);
		}

		//Set the resource header.
		new_resource->_Header = header;

		//Set the file path and file offset.
		new_resource->_FilePath = file->GetFilePath();
		new_resource->_FileOffset = file->GetCurrentPosition() - sizeof(ResourceHeader);

		//Load the resource.
		RenderPipelineData data;
		_ResourceLoadingSystem.LoadRenderPipeline(file, &data);

		//Create the resource.
		_ResourceCreationSystem.CreateRenderPipeline(&data, new_resource);

		//Register that the resource is now loaded.
		new_resource->_LoadState = ResourceLoadState::LOADED;
	}

	else if (header._TypeIdentifier == ResourceConstants::SHADER_TYPE_IDENTIFIER)
	{
		/*
		*	Find or allocate the new resource.
		*	The resource might have been created already by other dependant resources, but not loaded yet.
		*/
		ShaderResource* RESTRICT new_resource;

		if (ShaderResource* const RESTRICT* const RESTRICT found_resource{ _ShaderResources.Find(header._ResourceIdentifier) })
		{
			new_resource = *found_resource;
		}

		else
		{
			new_resource = new (MemorySystem::Instance->TypeAllocate<ShaderResource>()) ShaderResource();
			_ShaderResources.Add(header._ResourceIdentifier, new_resource);
			_AllResources.Emplace(new_resource);
		}

		//Set the resource header.
		new_resource->_Header = header;

		//Set the file path and file offset.
		new_resource->_FilePath = file->GetFilePath();
		new_resource->_FileOffset = file->GetCurrentPosition() - sizeof(ResourceHeader);

		//Load the resource.
		ShaderData data;
		_ResourceLoadingSystem.LoadShader(file, &data);

		//Create the resource.
		_ResourceCreationSystem.CreateShader(&data, new_resource);

		//Register that the resource is now loaded.
		new_resource->_LoadState = ResourceLoadState::LOADED;
	}

	else if (header._TypeIdentifier == ResourceConstants::SOUND_TYPE_IDENTIFIER)
	{
		/*
		*	Find or allocate the new resource.
		*	The resource might have been created already by other dependant resources, but not loaded yet.
		*/
		SoundResource* RESTRICT new_resource;

		if (SoundResource* const RESTRICT* const RESTRICT found_resource{ _SoundResources.Find(header._ResourceIdentifier) })
		{
			new_resource = *found_resource;
		}

		else
		{
			new_resource = new (MemorySystem::Instance->TypeAllocate<SoundResource>()) SoundResource();
			_SoundResources.Add(header._ResourceIdentifier, new_resource);
			_AllResources.Emplace(new_resource);
		}

		//Set the resource header.
		new_resource->_Header = header;

		//Set the file path and file offset.
		new_resource->_FilePath = file->GetFilePath();
		new_resource->_FileOffset = file->GetCurrentPosition() - sizeof(ResourceHeader);

		//Load the resource.
		SoundData data;
		_ResourceLoadingSystem.LoadSound(file, &data);

		//Create the resource.
		_ResourceCreationSystem.CreateSound(&data, new_resource);

		//Register that the resource is now loaded.
		new_resource->_LoadState = ResourceLoadState::LOADED;
	}

	else if (header._TypeIdentifier == ResourceConstants::TEXTURE_3D_TYPE_IDENTIFIER)
	{
		/*
		*	Find or allocate the new resource.
		*	The resource might have been created already by other dependant resources, but not loaded yet.
		*/
		Texture3DResource* RESTRICT new_resource;

		if (Texture3DResource* const RESTRICT* const RESTRICT found_resource{ _Texture3DResources.Find(header._ResourceIdentifier) })
		{
			new_resource = *found_resource;
		}

		else
		{
			new_resource = new (MemorySystem::Instance->TypeAllocate<Texture3DResource>()) Texture3DResource();
			_Texture3DResources.Add(header._ResourceIdentifier, new_resource);
			_AllResources.Emplace(new_resource);
		}

		//Set the resource header.
		new_resource->_Header = header;

		//Set the file path and file offset.
		new_resource->_FilePath = file->GetFilePath();
		new_resource->_FileOffset = file->GetCurrentPosition() - sizeof(ResourceHeader);

		//Load the resource.
		Texture3DData data;
		_ResourceLoadingSystem.LoadTexture3D(file, &data);

		//Create the resource.
		_ResourceCreationSystem.CreateTexture3D(&data, new_resource);

		//Register that the resource is now loaded.
		new_resource->_LoadState = ResourceLoadState::LOADED;
	}
}