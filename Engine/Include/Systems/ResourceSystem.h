#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>
#include <Core/General/HashString.h>

//Resources.
#include <Resources/Core/RawDataResource.h>
#include <Resources/Core/RenderPipelineResource.h>
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/ShaderResource.h>
#include <Resources/Core/Texture3DResource.h>
#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
	#include <Resources/Building/ResourceBuildingSystem.h>
#endif
#include <Resources/Creation/ResourceCreationSystem.h>
#include <Resources/Loading/ResourceLoadingSystem.h>

//Systems.
#include <Systems/System.h>

//Forward declarations.
class BinaryInputFile;

class ResourceSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		ResourceSystem
	);

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

	//Container for all raw data resources.
	HashTable<HashString, RawDataResource *RESTRICT> _RawDataResources;

	//Container for all render pipeline resources.
	HashTable<HashString, RenderPipelineResource *RESTRICT> _RenderPipelineResources;

	//Container for all shader resources.
	HashTable<HashString, ShaderResource *RESTRICT> _ShaderResources;

	//Container for all texture 3D resources.
	HashTable<HashString, Texture3DResource *RESTRICT> _Texture3DResources;

	//Container for all resources.
	DynamicArray<Resource *RESTRICT> _AllResources;

	/*
	*	Loads a resource collection from the given binary file.
	*/
	void LoadResourceCollection(BinaryInputFile *const RESTRICT file) NOEXCEPT;

	/*
	*	Loads a resource from the given binary file.
	*/
	void LoadResource(BinaryInputFile *const RESTRICT file) NOEXCEPT;

};