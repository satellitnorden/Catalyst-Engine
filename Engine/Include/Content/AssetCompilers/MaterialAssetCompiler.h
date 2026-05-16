#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

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
	*	Allocates an asset.
	*/
	Asset *const RESTRICT AllocateAsset() NOEXCEPT override
	{
		return new (_AssetAllocator.Allocate()) MaterialAsset();
	}

	/*
	*	Loads a single asset with the given load context.
	*/
	void Load(const LoadContext &load_context) NOEXCEPT override;

	/*
	*	Runs after load.
	*/
	void PostLoad() NOEXCEPT override;

private:

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

	//The asset allocator.
	PoolAllocator<sizeof(MaterialAsset)> _AssetAllocator;

	//The post link queue.
	AtomicQueue<PostLinkData, 4'096, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _PostLinkQueue;

};