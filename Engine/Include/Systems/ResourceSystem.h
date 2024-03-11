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
#include <Resources/Core/RawDataResource.h>
#include <Resources/Core/RenderPipelineResource.h>
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/ShaderResource.h>
#include <Resources/Core/SoundResource.h>
#include <Resources/Core/Texture3DResource.h>
#include <Resources/Core/VideoResource.h>
#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
	#include <Resources/Building/ResourceBuildingSystem.h>
#endif
#include <Resources/Creation/ResourceCreationSystem.h>
#include <Resources/Loading/ResourceLoadingSystem.h>

class ALIGN(8) ResourceSystem final
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
	*	Terminates the resource system.
	*/
	void Terminate() NOEXCEPT;

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
	*	Loads the resources contained in the given directory path. Can be a mix between resource collections and resources.
	*/
	void LoadResources(const char *const RESTRICT directory_path) NOEXCEPT;

	/*
	*	Loads a resource collection contained in the given file path.
	*/
	void LoadResourceCollection(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Loads a single resource contained in the given file path.
	*/
	void LoadResource(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Returns the animated model resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<AnimatedModelResource> GetAnimatedModelResource(const HashString identifier) NOEXCEPT;

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
	NO_DISCARD ResourcePointer<AnimationResource> GetAnimationResource(const HashString identifier) NOEXCEPT;

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
	NO_DISCARD ResourcePointer<FontResource> GetFontResource(const HashString identifier) NOEXCEPT;

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
	NO_DISCARD ResourcePointer<LevelResource> GetLevelResource(const HashString identifier) NOEXCEPT;

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
	*	Returns the raw data resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<RawDataResource> GetRawDataResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns or creates the raw data resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<RawDataResource> FindOrCreateRawDataResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns the render pipeline resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<RenderPipelineResource> GetRenderPipelineResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns or creates the render pipeline resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<RenderPipelineResource> FindOrCreateRenderPipelineResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns the shader resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<ShaderResource> GetShaderResource(const HashString identifier) NOEXCEPT;

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
	NO_DISCARD ResourcePointer<SoundResource> GetSoundResource(const HashString identifier) NOEXCEPT;

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
	*	Returns the video resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<VideoResource> GetVideoResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns or creates the video resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<VideoResource> FindOrCreateVideoResource(const HashString identifier) NOEXCEPT;

	/*
	*	Returns all video resources.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, VideoResource *RESTRICT> &GetAllVideoResources() const NOEXCEPT
	{
		return _VideoResources;
	}

	/*
	*	Returns the texture 3D resource with the given identifier.
	*/
	NO_DISCARD ResourcePointer<Texture3DResource> GetTexture3DResource(const HashString identifier) NOEXCEPT;

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

	/*
	*	Returns all resources.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<Resource *RESTRICT> &GetAllResources() const NOEXCEPT
	{
		return _AllResources;
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

	//Container for all level resources.
	HashTable<HashString, LevelResource *RESTRICT> _LevelResources;

	//Container for all raw data resources.
	HashTable<HashString, RawDataResource *RESTRICT> _RawDataResources;

	//Container for all render pipeline resources.
	HashTable<HashString, RenderPipelineResource *RESTRICT> _RenderPipelineResources;

	//Container for all shader resources.
	HashTable<HashString, ShaderResource *RESTRICT> _ShaderResources;

	//Container for all sound resources.
	HashTable<HashString, SoundResource *RESTRICT> _SoundResources;

	//Container for all video resources.
	HashTable<HashString, VideoResource *RESTRICT> _VideoResources;

	//Container for all texture 3D resources.
	HashTable<HashString, Texture3DResource *RESTRICT> _Texture3DResources;

	//Container for all resources.
	DynamicArray<Resource *RESTRICT> _AllResources;

	/*
	*	Loads a resource collection from the given binary file.
	*/
	void LoadResourceCollection(BinaryFile<BinaryFileMode::IN> *const RESTRICT file) NOEXCEPT;

	/*
	*	Loads a resource from the given binary file.
	*/
	void LoadResource(BinaryFile<BinaryFileMode::IN> *const RESTRICT file) NOEXCEPT;

};