#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>
#include <Core/General/HashString.h>

//Resources.
#include <Resources/Core/AnimatedModelResource.h>
#include <Resources/Core/AnimationResource.h>
#include <Resources/Core/FontResource.h>
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/ShaderResource.h>
#include <Resources/Core/SoundResource.h>
#include <Resources/Core/Texture2DResource.h>
#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
	#include <Resources/Building/ResourceBuildingSystem.h>
#endif
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

#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
	/*
	*	Returns the resource building system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD ResourceBuildingSystem *const RESTRICT GetResourceBuildingSystem() NOEXCEPT
	{
		return &_ResourceBuildingSystem;
	}
#endif

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
	*	Returns the animated model resource with the given identifier.
	*/
	FORCE_INLINE NO_DISCARD ResourcePointer<AnimatedModelResource> GetAnimatedModelResource(const HashString identifier) NOEXCEPT
	{
		//Find the resource.
		AnimatedModelResource *const RESTRICT *const RESTRICT resource{ _AnimatedModelResources.Find(identifier) };

		ASSERT(resource, "Couldn't find resource!");

		return ResourcePointer<AnimatedModelResource>(*resource);
	}

	/*
	*	Returns the animation resource with the given identifier.
	*/
	FORCE_INLINE NO_DISCARD ResourcePointer<AnimationResource> GetAnimationResource(const HashString identifier) NOEXCEPT
	{
		//Find the resource.
		AnimationResource *const RESTRICT *const RESTRICT resource{ _AnimationResources.Find(identifier) };

		ASSERT(resource, "Couldn't find resource!");

		return ResourcePointer<AnimationResource>(*resource);
	}

	/*
	*	Returns the font resource with the given identifier.
	*/
	FORCE_INLINE NO_DISCARD ResourcePointer<FontResource> GetFontResource(const HashString identifier) NOEXCEPT
	{
		//Find the resource.
		FontResource *const RESTRICT *const RESTRICT resource{ _FontResources.Find(identifier) };

		ASSERT(resource, "Couldn't find resource!");

		return ResourcePointer<FontResource>(*resource);
	}

	/*
	*	Returns the shader resource with the given identifier.
	*/
	FORCE_INLINE NO_DISCARD ResourcePointer<ShaderResource> GetShaderResource(const HashString identifier) NOEXCEPT
	{
		//Find the resource.
		ShaderResource *const RESTRICT *const RESTRICT resource{ _ShaderResources.Find(identifier) };

		ASSERT(resource, "Couldn't find resource!");

		return ResourcePointer<ShaderResource>(*resource);
	}

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

#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
	//The resource building system.
	ResourceBuildingSystem _ResourceBuildingSystem;
#endif

	//The resource loading system.
	ResourceLoadingSystem _ResourceLoadingSystem;

	//The resource creation system.
	ResourceCreationSystem _ResourceCreationSystem;

	//Container for all animated model resources.
	HashTable<HashString, AnimatedModelResource *RESTRICT> _AnimatedModelResources;

	//Container for all animation resources.
	HashTable<HashString, AnimationResource *RESTRICT> _AnimationResources;

	//Container for all font resources.
	HashTable<HashString, FontResource *RESTRICT> _FontResources;

	//Container for all shader resources.
	HashTable<HashString, ShaderResource *RESTRICT> _ShaderResources;

	//Container for all sound resources.
	HashTable<HashString, SoundResource *RESTRICT> _SoundResources;

	//Container for all texture 2D resources.
	HashTable<HashString, Texture2DResource *RESTRICT> _Texture2DResources;

};