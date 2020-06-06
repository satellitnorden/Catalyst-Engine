#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>
#include <Core/General/HashString.h>

//Resources.
#include <Resources/Core/AnimatedModelResource.h>
#include <Resources/Core/AnimationResource.h>
#include <Resources/Core/FontResource.h>
#include <Resources/Core/LevelResource.h>
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/ShaderResource.h>
#include <Resources/Core/SoundResource.h>
#include <Resources/Core/Texture2DResource.h>
#include <Resources/Core/Texture3DResource.h>
#include <Resources/Core/TextureCubeResource.h>
#if !defined(CATALYST_CONFIGURATION_FINAL)
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

#if !defined(CATALYST_CONFIGURATION_FINAL)
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
	*	Loads the resources contained in the given directory path.
	*/
	void LoadResources(const char *const RESTRICT directory_path) NOEXCEPT;

	/*
	*	Loads a single resource conainted in the given file path.
	*/
	void LoadResource(const char *const RESTRICT file_path) NOEXCEPT;

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
	*	Returns or creates the animated model resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<AnimatedModelResource> FindOrCreateAnimatedModelResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns all animated model resources.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, AnimatedModelResource *RESTRICT> &GetAllAnimatedModelResources() const NOEXCEPT
	{
		return _AnimatedModelResources;
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
	*	Returns or creates the animation resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<AnimationResource> FindOrCreateAnimationResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns all animation resources.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, AnimationResource *RESTRICT> &GetAllAnimationResources() const NOEXCEPT
	{
		return _AnimationResources;
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
	*	Returns or creates the font resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<FontResource> FindOrCreateFontResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns all font resources.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, FontResource *RESTRICT> &GetAllFontResources() const NOEXCEPT
	{
		return _FontResources;
	}

	/*
	*	Returns the level resource with the given identifier.
	*/
	FORCE_INLINE NO_DISCARD ResourcePointer<LevelResource> GetLevelResource(const HashString identifier) NOEXCEPT
	{
		//Find the resource.
		LevelResource *const RESTRICT *const RESTRICT resource{ _LevelResources.Find(identifier) };

		ASSERT(resource, "Couldn't find resource!");

		return ResourcePointer<LevelResource>(*resource);
	}

	/*
	*	Returns or creates the level resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<LevelResource> FindOrCreateLevelResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns all level resources.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, LevelResource *RESTRICT> &GetAllLevelResources() const NOEXCEPT
	{
		return _LevelResources;
	}

	/*
	*	Returns the material resource with the given identifier.
	*/
	FORCE_INLINE NO_DISCARD ResourcePointer<MaterialResource> GetMaterialResource(const HashString identifier) NOEXCEPT
	{
		//Find the resource.
		MaterialResource *const RESTRICT *const RESTRICT resource{ _MaterialResources.Find(identifier) };

		ASSERT(resource, "Couldn't find resource!");

		return ResourcePointer<MaterialResource>(*resource);
	}

	/*
	*	Returns or creates the material resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<MaterialResource> FindOrCreateMaterialResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns all material resources.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, MaterialResource *RESTRICT> &GetAllMaterialResources() const NOEXCEPT
	{
		return _MaterialResources;
	}

	/*
	*	Returns the model resource with the given identifier.
	*/
	FORCE_INLINE NO_DISCARD ResourcePointer<ModelResource> GetModelResource(const HashString identifier) NOEXCEPT
	{
		//Find the resource.
		ModelResource *const RESTRICT *const RESTRICT resource{ _ModelResources.Find(identifier) };

		ASSERT(resource, "Couldn't find resource!");

		return ResourcePointer<ModelResource>(*resource);
	}

	/*
	*	Returns or creates the model resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<ModelResource> FindOrCreateModelResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns all model resources.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, ModelResource *RESTRICT> &GetAllModelResources() const NOEXCEPT
	{
		return _ModelResources;
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
	*	Returns or creates the shader resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<ShaderResource> FindOrCreateShaderResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns all shader resources.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, ShaderResource *RESTRICT> &GetAllShaderResources() const NOEXCEPT
	{
		return _ShaderResources;
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
	*	Returns or creates the sound resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<SoundResource> FindOrCreateSoundResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns all sound resources.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, SoundResource *RESTRICT> &GetAllSoundResources() const NOEXCEPT
	{
		return _SoundResources;
	}

	/*
	*	Returns the texture cube resource with the given identifier.
	*/
	FORCE_INLINE NO_DISCARD ResourcePointer<TextureCubeResource> GetTextureCubeResource(const HashString identifier) NOEXCEPT
	{
		//Find the resource.
		TextureCubeResource *const RESTRICT *const RESTRICT resource{ _TextureCubeResources.Find(identifier) };

		ASSERT(resource, "Couldn't find resource!");

		return ResourcePointer<TextureCubeResource>(*resource);
	}

	/*
	*	Returns or creates the texture cube resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<TextureCubeResource> FindOrCreateTextureCubeResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns all texture cube resources.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, TextureCubeResource *RESTRICT> &GetAllTextureCubeResources() const NOEXCEPT
	{
		return _TextureCubeResources;
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

	/*
	*	Returns or creates the texture 2D resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<Texture2DResource> FindOrCreateTexture2DResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns all texture cube 2D.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, Texture2DResource *RESTRICT> &GetAllTexture2DResources() const NOEXCEPT
	{
		return _Texture2DResources;
	}

	/*
	*	Returns the texture 3D resource with the given identifier.
	*/
	FORCE_INLINE NO_DISCARD ResourcePointer<Texture3DResource> GetTexture3DResource(const HashString identifier) NOEXCEPT
	{
		//Find the resource.
		Texture3DResource *const RESTRICT *const RESTRICT resource{ _Texture3DResources.Find(identifier) };

		ASSERT(resource, "Couldn't find resource!");

		return ResourcePointer<Texture3DResource>(*resource);
	}

	/*
	*	Returns or creates the texture 3D resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<Texture3DResource> FindOrCreateTexture3DResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns all texture cube 3D.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, Texture3DResource *RESTRICT> &GetAllTexture3DResources() const NOEXCEPT
	{
		return _Texture3DResources;
	}

private:

#if !defined(CATALYST_CONFIGURATION_FINAL)
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

	//Container for all level resources.
	HashTable<HashString, LevelResource *RESTRICT> _LevelResources;

	//Container for all material resources.
	HashTable<HashString, MaterialResource *RESTRICT> _MaterialResources;

	//Container for all model resources.
	HashTable<HashString, ModelResource *RESTRICT> _ModelResources;

	//Container for all shader resources.
	HashTable<HashString, ShaderResource *RESTRICT> _ShaderResources;

	//Container for all sound resources.
	HashTable<HashString, SoundResource *RESTRICT> _SoundResources;

	//Container for all texture cube resources.
	HashTable<HashString, TextureCubeResource *RESTRICT> _TextureCubeResources;

	//Container for all texture 2D resources.
	HashTable<HashString, Texture2DResource *RESTRICT> _Texture2DResources;

	//Container for all texture 3D resources.
	HashTable<HashString, Texture3DResource *RESTRICT> _Texture3DResources;

	/*
	*	Loads a resource collection from the given binary file.
	*/
	void LoadResourceCollection(BinaryFile<IOMode::In> *const RESTRICT file) NOEXCEPT;

	/*
	*	Loads a resource from the given binary file.
	*/
	void LoadResource(BinaryFile<IOMode::In> *const RESTRICT file) NOEXCEPT;

};