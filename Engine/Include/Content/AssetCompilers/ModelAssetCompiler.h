#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>

//Content.
#include <Content/Core/AssetCompiler.h>
#include <Content/Assets/ModelAsset.h>

//Systems.
#include <Systems/System.h>

class ModelAssetCompiler final : public AssetCompiler
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		ModelAssetCompiler
	);

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
	*	Allocates an asset.
	*/
	Asset *const RESTRICT AllocateAsset() NOEXCEPT override
	{
		return new (_AssetAllocator.Allocate()) ModelAsset();
	}

	/*
	*	Loads a single asset with the given load context.
	*/
	void Load(const LoadContext &load_context) NOEXCEPT override;

	/*
	*	Runs after load.
	*/
	void PostLoad() NOEXCEPT override;

#if !defined (CATALYST_CONFIGURATION_FINAL)
	/*
	*	Statistics. Returns if the retrieval succeeded.
	*/
	FORCE_INLINE NO_DISCARD bool GetStatistics(Statistics *const RESTRICT statistics) NOEXCEPT override
	{
		statistics->_AssetTypeName = "Model";
		statistics->_TotalCPUMemory = _TotalCPUMemory.Load();
		statistics->_TotalGPUMemory = _TotalGPUMemory.Load();

		return true;
	}
#endif

private:

	/*
	*	Post link data class definition.
	*/
	class PostLinkData final
	{

	public:

		//The texture.
		ModelAsset *RESTRICT _Asset;

		//The default materials.
		StaticArray<HashString, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _DefaultMaterials;

	};

	//The asset allocator.
	PoolAllocator<sizeof(ModelAsset)> _AssetAllocator;

	//The post link queue.
	AtomicQueue<PostLinkData, 4'096, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _PostLinkQueue;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//The total CPU memory.
	Atomic<uint64> _TotalCPUMemory{ 0 };

	//The total GPU memory.
	Atomic<uint64> _TotalGPUMemory{ 0 };
#endif

};