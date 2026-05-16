#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Atomic.h>

//Content.
#include <Content/Core/AssetCompiler.h>
#include <Content/Assets/AnimatedModelAsset.h>

//Systems.
#include <Systems/System.h>

class AnimatedModelAssetCompiler final : public AssetCompiler
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		AnimatedModelAssetCompiler
	);

	/*
	*	Default constructor.
	*/
	AnimatedModelAssetCompiler() NOEXCEPT;

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
		return new (_AssetAllocator.Allocate()) AnimatedModelAsset();
	}

	/*
	*	Loads a single asset with the given load context.
	*/
	void Load(const LoadContext &load_context) NOEXCEPT override;

#if !defined (CATALYST_CONFIGURATION_FINAL)
	/*
	*	Statistics. Returns if the retrieval succeeded.
	*/
	FORCE_INLINE NO_DISCARD bool GetStatistics(Statistics *const RESTRICT statistics) NOEXCEPT override
	{
		statistics->_AssetTypeName = "Animated Model";
		statistics->_TotalCPUMemory = _TotalCPUMemory.Load();
		statistics->_TotalGPUMemory = _TotalGPUMemory.Load();

		return true;
	}
#endif

private:

	//The asset allocator.
	PoolAllocator<sizeof(AnimatedModelAsset)> _AssetAllocator;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//The total CPU memory.
	Atomic<uint64> _TotalCPUMemory{ 0 };

	//The total GPU memory.
	Atomic<uint64> _TotalGPUMemory{ 0 };
#endif

};