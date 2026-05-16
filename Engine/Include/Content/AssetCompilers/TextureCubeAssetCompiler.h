#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>

//Content.
#include <Content/Core/AssetCompiler.h>
#include <Content/Assets/TextureCubeAsset.h>

//Systems.
#include <Systems/System.h>

class TextureCubeAssetCompiler final : public AssetCompiler
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		TextureCubeAssetCompiler
	);

	/*
	*	Default constructor.
	*/
	TextureCubeAssetCompiler() NOEXCEPT;

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
		return new (_AssetAllocator.Allocate()) TextureCubeAsset();
	}

	/*
	*	Loads a single asset with the given load context.
	*/
	void Load(const LoadContext &load_context) NOEXCEPT override;

private:

	//The asset allocator.
	PoolAllocator<sizeof(TextureCubeAsset)> _AssetAllocator;

};