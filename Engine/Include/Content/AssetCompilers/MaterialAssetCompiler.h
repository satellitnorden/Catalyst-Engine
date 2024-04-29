#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>

//Content.
#include <Content/Core/AssetCompiler.h>
#include <Content/Assets/MaterialAsset.h>

//Systems.
#include <Systems/System.h>

class MaterialAssetCompiler final : public AssetCompiler
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		MaterialAssetCompiler
	);

	/*
	*	Default constructor.
	*/
	MaterialAssetCompiler() NOEXCEPT;

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

	/*
	*	Runs after load.
	*/
	void PostLoad() NOEXCEPT override;

private:

	/*
	*	Compile data class definition.
	*/
	class CompileData final
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
	*	Load data class definition.
	*/
	class LoadData final
	{

	public:

		//The stream archive position.
		uint64 _StreamArchivePosition;

		//The stream archive.
		StreamArchive *RESTRICT _StreamArchive;

		//The asset.
		MaterialAsset *RESTRICT _Asset;

	};

	/*
	*	Post link data class definition.
	*/
	class PostLinkData final
	{

	public:

		//The texture.
		AssetPointer<Texture2DAsset> *RESTRICT _Asset;

		//The identifier.
		HashString _Identifier;

	};

	//The compile data allocator.
	PoolAllocator<sizeof(CompileData)> _CompileDataAllocator;

	//The load data allocator.
	PoolAllocator<sizeof(LoadData)> _LoadDataAllocator;

	//The asset allocator.
	PoolAllocator<sizeof(MaterialAsset)> _AssetAllocator;

	//The post link queue.
	AtomicQueue<PostLinkData, 4'096, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _PostLinkQueue;

	/*
	*	Compiles internally.
	*/
	void CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT;

	/*
	*	Loads internally.
	*/
	void LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT;

};