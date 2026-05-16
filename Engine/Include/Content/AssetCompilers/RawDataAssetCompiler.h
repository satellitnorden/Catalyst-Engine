#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Atomic.h>

//Content.
#include <Content/Core/AssetCompiler.h>
#include <Content/Assets/RawDataAsset.h>

//Systems.
#include <Systems/System.h>

class RawDataAssetCompiler final : public AssetCompiler
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		RawDataAssetCompiler
	);

	/*
	*	Default constructor.
	*/
	RawDataAssetCompiler() NOEXCEPT;

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
		return new (_AssetAllocator.Allocate()) RawDataAsset();
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
		statistics->_AssetTypeName = "Raw Data";
		statistics->_TotalCPUMemory = _TotalCPUMemory.Load();
		statistics->_TotalGPUMemory = 0;

		return true;
	}
#endif

private:

	//The asset allocator.
	PoolAllocator<sizeof(RawDataAsset)> _AssetAllocator;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//The total CPU memory.
	Atomic<uint64> _TotalCPUMemory{ 0 };
#endif

};