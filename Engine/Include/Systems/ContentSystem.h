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

//Math.
#include <Math/General/Vector.h>

//Systems.
#include <Systems/System.h>

//Forward declarations.
class BinaryInputFile;
class BinaryOutputFile;
class ContentCache;

//Type aliases.
using OnAssetCompiledCallback = bool(*)(const HashString asset_type_identifier, const char *const RESTRICT file_path);

class ContentSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		ContentSystem
	);

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
	*	Terminates the content system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Registers an asset compiler.
	*/
	void RegisterAssetCompiler(AssetCompiler *const RESTRICT asset_compiler) NOEXCEPT;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	/*
	*	Registers a callback for when an asset is compiled.
	*	Should return if the compilation should be re-run, for example if one asset being compiled generates other assets that then need to be compiled.
	*/
	void RegisterOnAssetCompiledCallback(OnAssetCompiledCallback callback) NOEXCEPT;

	/*
	*	Compiles the content for engine.
	*	Returns if new content was compiled.
	*/
	bool CompileEngine() NOEXCEPT;

	/*
	*	Compiles the content for game.
	*	Returns if new content was compiled.
	*/
	bool CompileGame() NOEXCEPT;
#endif

	/*
	*	Loads assets from the given directory path.
	*/
	void LoadAssets(const char *const RESTRICT directory_path) NOEXCEPT;

	/*
	*	Load a single asset from the given file path.
	*/
	void LoadAsset(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Load a single asset collection from the given file path.
	*/
	void LoadAssetCollection(const char *const RESTRICT file_path) NOEXCEPT;

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

	/*
	*	Returns all assets of the given asset type.
	*/
	FORCE_INLINE NO_DISCARD const HashTable<HashString, Asset *RESTRICT> &GetAllAssetsOfType(const HashString asset_type_identifier) NOEXCEPT
	{
		HashTable<HashString, Asset *RESTRICT> *const RESTRICT assets{ _Assets.Find(asset_type_identifier) };

		ASSERT(assets, "Couldn't find asset list for type!");

		return *assets;
	}

private:

	/*
	*	Compile result class definition.
	*/
	class CompileResult final
	{

	public:

		//Denotes if new assets were compiled.
		bool _NewAssetsCompiled{ false };

		//Denotes if a recompile should be triggered.
		bool _TriggerRecompile{ false };

	};

	//The asset compilers.
	HashTable<HashString, AssetCompiler *const RESTRICT> _AssetCompilers;

	//The task allocator.
	PoolAllocator<sizeof(Task)> _TaskAllocator;

	//The tasks.
	DynamicArray<Task *RESTRICT> _Tasks;

	//The assets. Divided up into asset types.
	HashTable<HashString, HashTable<HashString, Asset *RESTRICT>> _Assets;

	//The on asset compiled callbacks.
	DynamicArray<OnAssetCompiledCallback> _OnAssetCompiledCallbacks;

	/*
	*	Compiles assets in the given directory.
	*/
	void CompileAssetsInDirectory
	(
		const CompilationDomain compilation_domain,
		ContentCache *const RESTRICT content_cache,
		const char *const RESTRICT directory_path,
		const char *const RESTRICT collection,
		CompileResult *const RESTRICT compile_result
	) NOEXCEPT;

	/*
	*	Creates asset collections from the given directory path.
	*/
	void CreateAssetCollections(const char *const RESTRICT directory_path, BinaryOutputFile *const RESTRICT file) NOEXCEPT;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	/*
	*	The window callback.
	*/
	NO_DISCARD bool WindowCallback(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;
#endif

};