//Header file.
#include <Systems/ResourceSystem.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryInputFile.h>

//Systems.
#include <Systems/MemorySystem.h>

//STL.
#include <filesystem>

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

	if (header._TypeIdentifier == ResourceConstants::TEXTURE_3D_TYPE_IDENTIFIER)
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