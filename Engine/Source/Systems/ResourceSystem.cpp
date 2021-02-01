//Header file.
#include <Systems/ResourceSystem.h>

//File.
#include <File/Core/FileCore.h>

//Systems.
#include <Systems/MemorySystem.h>

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
			LoadResources(entry.path().generic_u8string().c_str());
		}

		else
		{
			//Get the extension for this file.
			const File::Extension file_extension{ File::GetExtension(entry.path().string().c_str()) };

			//Only load this file if it is actually a Catalyst resource or a Catalyst resource collection.
			if (file_extension == File::Extension::CR)
			{
				//Open the file.
				BinaryFile<IOMode::In> file{ entry.path().string().c_str() };

				//Load the resource.
				LoadResource(&file);

				//Close the file.
				file.Close();
			}

			if (file_extension == File::Extension::CRC)
			{
				//Open the file.
				BinaryFile<IOMode::In> file{ entry.path().string().c_str() };

				//Load the resource collection.
				LoadResourceCollection(&file);

				//Close the file.
				file.Close();
			}
		}
	}
}

/*
*	Loads a single resource conainted in the given file path.
*/
void ResourceSystem::LoadResource(const char *const RESTRICT file_path) NOEXCEPT
{
	//Open the file.
	BinaryFile<IOMode::In> file{ file_path };

	//Load the resource.
	LoadResource(&file);

	//Close the file.
	file.Close();
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
		_AnimatedModelResources.Add(identifier, new_resource);

		return ResourcePointer<AnimatedModelResource>(new_resource);
	}

	else
	{
		return ResourcePointer<AnimatedModelResource>(*resource);
	}
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
		_AnimationResources.Add(identifier, new_resource);

		return ResourcePointer<AnimationResource>(new_resource);
	}

	else
	{
		return ResourcePointer<AnimationResource>(*resource);
	}
}

/*
*	Returns or creates the font resource with the given identifier.
*/
NO_DISCARD ResourcePointer<FontResource> ResourceSystem::FindOrCreateFontResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	FontResource *const RESTRICT *const RESTRICT resource{ _FontResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		FontResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<FontResource>()) FontResource() };
		_FontResources.Add(identifier, new_resource);

		return ResourcePointer<FontResource>(new_resource);
	}

	else
	{
		return ResourcePointer<FontResource>(*resource);
	}
}

/*
*	Returns or creates the level resource with the given identifier.
*/
NO_DISCARD ResourcePointer<LevelResource> ResourceSystem::FindOrCreateLevelResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	LevelResource *const RESTRICT *const RESTRICT resource{ _LevelResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		LevelResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<LevelResource>()) LevelResource() };
		_LevelResources.Add(identifier, new_resource);

		return ResourcePointer<LevelResource>(new_resource);
	}

	else
	{
		return ResourcePointer<LevelResource>(*resource);
	}
}

/*
*	Returns or creates the material resource with the given identifier.
*/
NO_DISCARD ResourcePointer<MaterialResource> ResourceSystem::FindOrCreateMaterialResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	MaterialResource *const RESTRICT *const RESTRICT resource{ _MaterialResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		MaterialResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<MaterialResource>()) MaterialResource() };
		_MaterialResources.Add(identifier, new_resource);

		return ResourcePointer<MaterialResource>(new_resource);
	}

	else
	{
		return ResourcePointer<MaterialResource>(*resource);
	}
}

/*
*	Returns or creates the model resource with the given identifier.
*/
NO_DISCARD ResourcePointer<ModelResource> ResourceSystem::FindOrCreateModelResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	ModelResource *const RESTRICT *const RESTRICT resource{ _ModelResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		ModelResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<ModelResource>()) ModelResource() };
		_ModelResources.Add(identifier, new_resource);

		return ResourcePointer<ModelResource>(new_resource);
	}

	else
	{
		return ResourcePointer<ModelResource>(*resource);
	}
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
		_RawDataResources.Add(identifier, new_resource);

		return ResourcePointer<RawDataResource>(new_resource);
	}

	else
	{
		return ResourcePointer<RawDataResource>(*resource);
	}
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
		_ShaderResources.Add(identifier, new_resource);

		return ResourcePointer<ShaderResource>(new_resource);
	}

	else
	{
		return ResourcePointer<ShaderResource>(*resource);
	}
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
		_SoundResources.Add(identifier, new_resource);

		return ResourcePointer<SoundResource>(new_resource);
	}

	else
	{
		return ResourcePointer<SoundResource>(*resource);
	}
}

/*
*	Returns or creates the texture cube resource with the given identifier.
*/
NO_DISCARD ResourcePointer<TextureCubeResource> ResourceSystem::FindOrCreateTextureCubeResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	TextureCubeResource *const RESTRICT *const RESTRICT resource{ _TextureCubeResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		TextureCubeResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<TextureCubeResource>()) TextureCubeResource() };
		_TextureCubeResources.Add(identifier, new_resource);

		return ResourcePointer<TextureCubeResource>(new_resource);
	}

	else
	{
		return ResourcePointer<TextureCubeResource>(*resource);
	}
}

/*
*	Returns or creates the texture 2D resource with the given identifier.
*/
NO_DISCARD ResourcePointer<Texture2DResource> ResourceSystem::FindOrCreateTexture2DResource(const HashString identifier) NOEXCEPT
{
	//Find the resource.
	Texture2DResource *const RESTRICT *const RESTRICT resource{ _Texture2DResources.Find(identifier) };

	if (!resource)
	{
		//If the resource couldn't be found, create it.
		Texture2DResource *const RESTRICT new_resource{ new (MemorySystem::Instance->TypeAllocate<Texture2DResource>()) Texture2DResource() };
		_Texture2DResources.Add(identifier, new_resource);

		return ResourcePointer<Texture2DResource>(new_resource);
	}

	else
	{
		return ResourcePointer<Texture2DResource>(*resource);
	}
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
		_Texture3DResources.Add(identifier, new_resource);

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
void ResourceSystem::LoadResourceCollection(BinaryFile<IOMode::In> *const RESTRICT file) NOEXCEPT
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
void ResourceSystem::LoadResource(BinaryFile<IOMode::In> *const RESTRICT file) NOEXCEPT
{
	//Read the resource header.
	ResourceHeader header;
	file->Read(&header, sizeof(ResourceHeader));

	switch (header._TypeIdentifier)
	{
		case ResourceConstants::ANIMATED_MODEL_TYPE_IDENTIFIER:
		{
			/*
			*	Find or allocate the new resource.
			*	The resource might have been created already by other dependant resources, but not loaded yet.
			*/
			AnimatedModelResource *RESTRICT new_resource;

			if (AnimatedModelResource *const RESTRICT *const RESTRICT found_resource{ _AnimatedModelResources.Find(header._ResourceIdentifier) })
			{
				new_resource = *found_resource;
			}

			else
			{
				new_resource = new (MemorySystem::Instance->TypeAllocate<AnimatedModelResource>()) AnimatedModelResource();
				_AnimatedModelResources.Add(header._ResourceIdentifier, new_resource);
			}

			//Set the resource header.
			new_resource->_Header = header;

			//Load the resource.
			AnimatedModelData data;
			_ResourceLoadingSystem.LoadAnimatedModel(file, &data);

			//Create the resource.
			_ResourceCreationSystem.CreateAnimatedModel(&data, new_resource);

			//Register that the resource is now loaded.
			new_resource->_LoadState = ResourceLoadState::LOADED;

			break;
		}

		case ResourceConstants::ANIMATION_TYPE_IDENTIFIER:
		{
			/*
			*	Find or allocate the new resource.
			*	The resource might have been created already by other dependant resources, but not loaded yet.
			*/
			AnimationResource *RESTRICT new_resource;

			if (AnimationResource *const RESTRICT *const RESTRICT found_resource{ _AnimationResources.Find(header._ResourceIdentifier) })
			{
				new_resource = *found_resource;
			}

			else
			{
				new_resource = new (MemorySystem::Instance->TypeAllocate<AnimationResource>()) AnimationResource();
				_AnimationResources.Add(header._ResourceIdentifier, new_resource);
			}

			//Set the resource header.
			new_resource->_Header = header;

			//Load the resource.
			AnimationData data;
			_ResourceLoadingSystem.LoadAnimation(file, &data);

			//Create the resource.
			_ResourceCreationSystem.CreateAnimation(&data, new_resource);

			//Register that the resource is now loaded.
			new_resource->_LoadState = ResourceLoadState::LOADED;

			break;
		}

		case ResourceConstants::FONT_TYPE_IDENTIFIER:
		{
			/*
			*	Find or allocate the new resource.
			*	The resource might have been created already by other dependant resources, but not loaded yet.
			*/
			FontResource *RESTRICT new_resource;

			if (FontResource *const RESTRICT *const RESTRICT found_resource{ _FontResources.Find(header._ResourceIdentifier) })
			{
				new_resource = *found_resource;
			}

			else
			{
				new_resource = new (MemorySystem::Instance->TypeAllocate<FontResource>()) FontResource();
				_FontResources.Add(header._ResourceIdentifier, new_resource);
			}

			//Set the resource header.
			new_resource->_Header = header;

			//Load the resource.
			FontData data;
			_ResourceLoadingSystem.LoadFont(file, &data);

			//Create the resource.
			_ResourceCreationSystem.CreateFont(&data, new_resource);

			//Register that the resource is now loaded.
			new_resource->_LoadState = ResourceLoadState::LOADED;

			break;
		}

		case ResourceConstants::LEVEL_TYPE_IDENTIFIER:
		{
			/*
			*	Find or allocate the new resource.
			*	The resource might have been created already by other dependant resources, but not loaded yet.
			*/
			LevelResource *RESTRICT new_resource;

			if (LevelResource *const RESTRICT *const RESTRICT found_resource{ _LevelResources.Find(header._ResourceIdentifier) })
			{
				new_resource = *found_resource;
			}

			else
			{
				new_resource = new (MemorySystem::Instance->TypeAllocate<LevelResource>()) LevelResource();
				_LevelResources.Add(header._ResourceIdentifier, new_resource);
			}

			//Set the resource header.
			new_resource->_Header = header;

			//Load the resource.
			LevelData data;
			_ResourceLoadingSystem.LoadLevel(file, &data);

			//Create the resource.
			_ResourceCreationSystem.CreateLevel(&data, new_resource);

			//Register that the resource is now loaded.
			new_resource->_LoadState = ResourceLoadState::LOADED;

			break;
		}

		case ResourceConstants::MATERIAL_TYPE_IDENTIFIER:
		{
			/*
			*	Find or allocate the new resource.
			*	The resource might have been created already by other dependant resources, but not loaded yet.
			*/
			MaterialResource *RESTRICT new_resource;

			if (MaterialResource *const RESTRICT *const RESTRICT found_resource{ _MaterialResources.Find(header._ResourceIdentifier) })
			{
				new_resource = *found_resource;
			}

			else
			{
				new_resource = new (MemorySystem::Instance->TypeAllocate<MaterialResource>()) MaterialResource();
				_MaterialResources.Add(header._ResourceIdentifier, new_resource);
			}

			//Set the resource header.
			new_resource->_Header = header;

			//Load the resource.
			MaterialData data;
			_ResourceLoadingSystem.LoadMaterial(file, &data);

			//Create the resource.
			_ResourceCreationSystem.CreateMaterial(&data, new_resource);

			//Register that the resource is now loaded.
			new_resource->_LoadState = ResourceLoadState::LOADED;

			break;
		}

		case ResourceConstants::MODEL_TYPE_IDENTIFIER:
		{
			/*
			*	Find or allocate the new resource.
			*	The resource might have been created already by other dependant resources, but not loaded yet.
			*/
			ModelResource *RESTRICT new_resource;

			if (ModelResource *const RESTRICT *const RESTRICT found_resource{ _ModelResources.Find(header._ResourceIdentifier) })
			{
				new_resource = *found_resource;
			}

			else
			{
				new_resource = new (MemorySystem::Instance->TypeAllocate<ModelResource>()) ModelResource();
				_ModelResources.Add(header._ResourceIdentifier, new_resource);
			}

			//Set the resource header.
			new_resource->_Header = header;

			//Load the resource.
			ModelData data;
			_ResourceLoadingSystem.LoadModel(file, &data);

			//Create the resource.
			_ResourceCreationSystem.CreateModel(&data, new_resource);

			//Register that the resource is now loaded.
			new_resource->_LoadState = ResourceLoadState::LOADED;

			break;
		}

		case ResourceConstants::RAW_DATA_TYPE_IDENTIFIER:
		{
			/*
			*	Find or allocate the new resource.
			*	The resource might have been created already by other dependant resources, but not loaded yet.
			*/
			RawDataResource *RESTRICT new_resource;

			if (RawDataResource *const RESTRICT *const RESTRICT found_resource{ _RawDataResources.Find(header._ResourceIdentifier) })
			{
				new_resource = *found_resource;
			}

			else
			{
				new_resource = new (MemorySystem::Instance->TypeAllocate<RawDataResource>()) RawDataResource();
				_RawDataResources.Add(header._ResourceIdentifier, new_resource);
			}

			//Set the resource header.
			new_resource->_Header = header;

			//Load the resource.
			RawDataData data;
			_ResourceLoadingSystem.LoadRawData(file, &data);

			//Create the resource.
			_ResourceCreationSystem.CreateRawData(&data, new_resource);

			//Register that the resource is now loaded.
			new_resource->_LoadState = ResourceLoadState::LOADED;

			break;
		}

		case ResourceConstants::SHADER_TYPE_IDENTIFIER:
		{
			/*
			*	Find or allocate the new resource.
			*	The resource might have been created already by other dependant resources, but not loaded yet.
			*/
			ShaderResource *RESTRICT new_resource;

			if (ShaderResource *const RESTRICT *const RESTRICT found_resource{ _ShaderResources.Find(header._ResourceIdentifier) })
			{
				new_resource = *found_resource;
			}

			else
			{
				new_resource = new (MemorySystem::Instance->TypeAllocate<ShaderResource>()) ShaderResource();
				_ShaderResources.Add(header._ResourceIdentifier, new_resource);
			}

			//Set the resource header.
			new_resource->_Header = header;

			//Load the resource.
			ShaderData data;
			_ResourceLoadingSystem.LoadShader(file, &data);

			//Create the resource.
			_ResourceCreationSystem.CreateShader(&data, new_resource);

			//Register that the resource is now loaded.
			new_resource->_LoadState = ResourceLoadState::LOADED;

			break;
		}

		case ResourceConstants::SOUND_TYPE_IDENTIFIER:
		{
			/*
			*	Find or allocate the new resource.
			*	The resource might have been created already by other dependant resources, but not loaded yet.
			*/
			SoundResource *RESTRICT new_resource;

			if (SoundResource *const RESTRICT *const RESTRICT found_resource{ _SoundResources.Find(header._ResourceIdentifier) })
			{
				new_resource = *found_resource;
			}

			else
			{
				new_resource = new (MemorySystem::Instance->TypeAllocate<SoundResource>()) SoundResource();
				_SoundResources.Add(header._ResourceIdentifier, new_resource);
			}

			//Set the resource header.
			new_resource->_Header = header;

			//Load the resource.
			SoundData data;
			_ResourceLoadingSystem.LoadSound(file, &data);

			//Create the resource.
			_ResourceCreationSystem.CreateSound(&data, new_resource);

			//Register that the resource is now loaded.
			new_resource->_LoadState = ResourceLoadState::LOADED;

			break;
		}

		case ResourceConstants::TEXTURE_CUBE_TYPE_IDENTIFIER:
		{
			/*
			*	Find or allocate the new resource.
			*	The resource might have been created already by other dependant resources, but not loaded yet.
			*/
			TextureCubeResource *RESTRICT new_resource;

			if (TextureCubeResource *const RESTRICT *const RESTRICT found_resource{ _TextureCubeResources.Find(header._ResourceIdentifier) })
			{
				new_resource = *found_resource;
			}

			else
			{
				new_resource = new (MemorySystem::Instance->TypeAllocate<TextureCubeResource>()) TextureCubeResource();
				_TextureCubeResources.Add(header._ResourceIdentifier, new_resource);
			}

			//Set the resource header.
			new_resource->_Header = header;

			//Load the resource.
			TextureCubeData data;
			_ResourceLoadingSystem.LoadTextureCube(file, &data);

			//Create the resource.
			_ResourceCreationSystem.CreateTextureCube(&data, new_resource);

			//Register that the resource is now loaded.
			new_resource->_LoadState = ResourceLoadState::LOADED;

			break;
		}

		case ResourceConstants::TEXTURE_2D_TYPE_IDENTIFIER:
		{
			/*
			*	Find or allocate the new resource.
			*	The resource might have been created already by other dependant resources, but not loaded yet.
			*/
			Texture2DResource *RESTRICT new_resource;

			if (Texture2DResource *const RESTRICT *const RESTRICT found_resource{ _Texture2DResources.Find(header._ResourceIdentifier) })
			{
				new_resource = *found_resource;
			}

			else
			{
				new_resource = new (MemorySystem::Instance->TypeAllocate<Texture2DResource>()) Texture2DResource();
				_Texture2DResources.Add(header._ResourceIdentifier, new_resource);
			}

			//Set the resource header.
			new_resource->_Header = header;

			//Load the resource.
			Texture2DData data;
			_ResourceLoadingSystem.LoadTexture2D(file, &data);

			//Create the resource.
			_ResourceCreationSystem.CreateTexture2D(&data, new_resource);

			//Register that the resource is now loaded.
			new_resource->_LoadState = ResourceLoadState::LOADED;

			break;
		}

		case ResourceConstants::TEXTURE_3D_TYPE_IDENTIFIER:
		{
			/*
			*	Find or allocate the new resource.
			*	The resource might have been created already by other dependant resources, but not loaded yet.
			*/
			Texture3DResource *RESTRICT new_resource;

			if (Texture3DResource *const RESTRICT *const RESTRICT found_resource{ _Texture3DResources.Find(header._ResourceIdentifier) })
			{
				new_resource = *found_resource;
			}

			else
			{
				new_resource = new (MemorySystem::Instance->TypeAllocate<Texture3DResource>()) Texture3DResource();
				_Texture3DResources.Add(header._ResourceIdentifier, new_resource);
			}

			//Set the resource header.
			new_resource->_Header = header;

			//Load the resource.
			Texture3DData data;
			_ResourceLoadingSystem.LoadTexture3D(file, &data);

			//Create the resource.
			_ResourceCreationSystem.CreateTexture3D(&data, new_resource);

			//Register that the resource is now loaded.
			new_resource->_LoadState = ResourceLoadState::LOADED;

			break;
		}

		default:
		{
			ASSERT(false, "Undefined resource type.");

			break;
		}
	}
}