#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>
#include <Core/General/HashString.h>

//Content.
#include <Content/Core/ContentCore.h>
#include <Content/Core/Asset.h>
#include <Content/Core/AssetCompiler.h>
#include <Content/Core/AssetPointer.h>

//Forward declarations.
class ContentCache;

class ALIGN(8) ContentSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ContentSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ContentSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the content system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Registers an asset compiler.
	*/
	void RegisterAssetCompiler(AssetCompiler *const RESTRICT asset_compiler) NOEXCEPT;

	/*
	*	Compiles the content for engine.
	*	Returns if new content was compiled.
	*/
	NO_DISCARD bool CompileEngine() NOEXCEPT;

	/*
	*	Compiles the content for game.
	*	Returns if new content was compiled.
	*/
	NO_DISCARD bool CompileGame() NOEXCEPT;

	/*
	*	Loads assets from the given directory path.
	*/
	void LoadAssets(const char *const RESTRICT directory_path) NOEXCEPT;

	/*
	*	Returns if the asset with the given asset identifier exists.
	*/
	template <typename TYPE>
	FORCE_INLINE NO_DISCARD bool AssetExists(const HashString asset_identifier) const NOEXCEPT
	{
		HashTable<HashString, Asset* RESTRICT>* const RESTRICT assets{ _Assets.Find(TYPE::TYPE_IDENTIFIER) };

		ASSERT(assets, "Couldn't find asset list for type!");

		return assets->Find(asset_identifier) != nullptr;
	}

	/*
	*	Returns the asset with the given asset identifier.
	*/
	template <typename TYPE>
	FORCE_INLINE NO_DISCARD AssetPointer<TYPE> GetAsset(const HashString asset_identifier) NOEXCEPT
	{
		HashTable<HashString, Asset *RESTRICT> *const RESTRICT assets{ _Assets.Find(TYPE::TYPE_IDENTIFIER) };

		ASSERT(assets, "Couldn't find asset list for type!");
		
		Asset *const RESTRICT *const RESTRICT asset{ assets->Find(asset_identifier) };

		ASSERT(asset, "Couldn't find asset!");

		return AssetPointer<TYPE>(static_cast<TYPE *const RESTRICT>(*asset));
	}

private:

	//The asset compilers.
	HashTable<HashString, AssetCompiler *const RESTRICT> _AssetCompilers;

	//The task allocator.
	PoolAllocator<sizeof(Task)> _TaskAllocator;

	//The tasks.
	DynamicArray<Task *RESTRICT> _Tasks;

	//The assets. Divided up into asset types.
	HashTable<HashString, HashTable<HashString, Asset *RESTRICT>> _Assets;

	/*
	*	Scans assets in the given directory.
	*	Returns if new content was compiled.
	*/
	NO_DISCARD bool ScanAssetsInDirectory
	(
		const CompilationDomain compilation_domain,
		ContentCache *const RESTRICT content_cache,
		const char *const RESTRICT directory_path,
		const char *const RESTRICT collection
	) NOEXCEPT;

	/*
	*	Load a single asset from the given file path.
	*/
	void LoadAsset(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Parses content definitions in the given directory.
	*	Returns if new content was compiled.
	*/
	NO_DISCARD bool ParseContentDefinitionsInDirectory(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const char *const RESTRICT directory_path) NOEXCEPT;

	/*
	*	Parses a Font from the given file.
	*/
	void ParseFont(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

	/*
	*	Parses a Material from the given file.
	*/
	void ParseMaterial(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

	/*
	*	Parses a Model from the given file.
	*/
	void ParseModel(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

	/*
	*	Parses a TextureCube from the given file.
	*/
	void ParseTextureCube(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

	/*
	*	Parses a procedural tree model from the given file.
	*/
	void ParseProceduralTreeModel(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

	/*
	*	Parses an Impostor Material from the given file.
	*/
	void ParseImpostorMaterial(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

};
#endif