#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Content.
#include <Content/Core/AssetCompiler.h>
#include <Content/Assets/ModelAsset.h>

class ALIGN(8) ModelAssetCompiler final : public AssetCompiler
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ModelAssetCompiler);

	/*
	*	Default constructor.
	*/
	ModelAssetCompiler() NOEXCEPT;

	/*
	*	Returns the asset type identifier.
	*/
	NO_DISCARD HashString AssetTypeIdentifier() const NOEXCEPT override;

	/*
	*	Returns the current version.
	*/
	NO_DISCARD uint64 CurrentVersion() const NOEXCEPT override;

	/*
	*	Compiles a single asset with the given compile context.
	*/
	void Compile(const CompileContext &compile_context) NOEXCEPT override;

	/*
	*	Loads a single asset with the given load context.
	*/
	NO_DISCARD Asset *const RESTRICT Load(const LoadContext &load_context) NOEXCEPT override;

private:

	/*
	*	Model compile data class definition.
	*/
	class ModelCompileData final
	{

	public:

		//The collection.
		DynamicString _Collection;

		//The file path.
		DynamicString _FilePath;

		//The name.
		DynamicString _Name;

		//The compilation domain.
		CompilationDomain _CompilationDomain;

	};

	/*
	*	Model load data class definition.
	*/
	class ModelLoadData final
	{

	public:

		//The stream archive position.
		uint64 _StreamArchivePosition;

		//The stream archive.
		StreamArchive *RESTRICT _StreamArchive;

		//The asset.
		ModelAsset *RESTRICT _Asset;

	};

	//The compile data allocator.
	PoolAllocator<sizeof(ModelCompileData)> _CompileDataAllocator;

	//The load data allocator.
	PoolAllocator<sizeof(ModelLoadData)> _LoadDataAllocator;

	//The asset allocator.
	PoolAllocator<sizeof(ModelAsset)> _AssetAllocator;

	/*
	*	Compiles internally.
	*/
	void CompileInternal(ModelCompileData *const RESTRICT compile_data) NOEXCEPT;

	/*
	*	Loads internally.
	*/
	void LoadInternal(ModelLoadData *const RESTRICT load_data) NOEXCEPT;

};