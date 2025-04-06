#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

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
	*	Loads a single asset with the given load context.
	*/
	NO_DISCARD Asset *const RESTRICT Load(const LoadContext &load_context) NOEXCEPT override;

#if !defined (CATALYST_CONFIGURATION_FINAL)
	/*
	*	Statistics. Returns if the retrieval succeeded.
	*/
	FORCE_INLINE NO_DISCARD bool GetStatistics(Statistics *const RESTRICT statistics) NOEXCEPT
	{
		statistics->_AssetTypeName = "Animated Model";
		statistics->_TotalCPUMemory = _TotalCPUMemory.Load();
		statistics->_TotalGPUMemory = _TotalGPUMemory.Load();

		return true;
	}
#endif

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
		AnimatedModelAsset *RESTRICT _Asset;

	};

	//The compile data allocator.
	PoolAllocator<sizeof(CompileData)> _CompileDataAllocator;

	//The load data allocator.
	PoolAllocator<sizeof(LoadData)> _LoadDataAllocator;

	//The asset allocator.
	PoolAllocator<sizeof(AnimatedModelAsset)> _AssetAllocator;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//The total CPU memory.
	Atomic<uint64> _TotalCPUMemory{ 0 };

	//The total GPU memory.
	Atomic<uint64> _TotalGPUMemory{ 0 };
#endif

	/*
	*	Compiles internally.
	*/
	void CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT;

	/*
	*	Loads internally.
	*/
	void LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT;

};