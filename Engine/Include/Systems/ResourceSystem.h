#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>
#include <Core/General/HashString.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/SoundResource.h>
#include <Resources/Core/Texture2DResource.h>
#include <Resources/Building/ResourceBuildingSystem.h>
#include <Resources/Creation/ResourceCreationSystem.h>
#include <Resources/Loading/ResourceLoadingSystem.h>

class ResourceSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ResourceSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ResourceSystem() NOEXCEPT
	{

	}

	/*
	*	Returns the resource building system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD ResourceBuildingSystem *const RESTRICT GetResourceBuildingSystem() NOEXCEPT
	{
		return &_ResourceBuildingSystem;
	}

	/*
	*	Returns the resource loading system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD ResourceLoadingSystem *const RESTRICT GetResourceLoadingSystem() NOEXCEPT
	{
		return &_ResourceLoadingSystem;
	}

	/*
	*	Returns the resource creation system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD ResourceCreationSystem *const RESTRICT GetResourceCreationSystem() NOEXCEPT
	{
		return &_ResourceCreationSystem;
	}

	/*
	*	Loads the resource collection with the given file path.
	*/
	void LoadsResourceCollection(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Returns the sound resource with the given identifier.
	*/
	FORCE_INLINE NO_DISCARD ResourcePointer<SoundResource> GetSoundResource(const HashString identifier) NOEXCEPT
	{
		//Find the resource.
		SoundResource *const RESTRICT *const RESTRICT resource{ _SoundResources.Find(identifier) };

		ASSERT(resource, "Couldn't find resource!");

		return ResourcePointer<SoundResource>(*resource);
	}

	/*
	*	Returns the texture 2D resource with the given identifier.
	*/
	FORCE_INLINE NO_DISCARD ResourcePointer<Texture2DResource> GetTexture2DResource(const HashString identifier) NOEXCEPT
	{
		//Find the resource.
		Texture2DResource *const RESTRICT *const RESTRICT resource{ _Texture2DResources.Find(identifier) };

		ASSERT(resource, "Couldn't find resource!");

		return ResourcePointer<Texture2DResource>(*resource);
	}

private:

	//The resource building system.
	ResourceBuildingSystem _ResourceBuildingSystem;

	//The resource loading system.
	ResourceLoadingSystem _ResourceLoadingSystem;

	//The resource creation system.
	ResourceCreationSystem _ResourceCreationSystem;

	//Container for all sound resources.
	HashTable<HashString, SoundResource *RESTRICT> _SoundResources;

	//Container for all texture 2D resources.
	HashTable<HashString, Texture2DResource *RESTRICT> _Texture2DResources;

};