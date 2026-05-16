#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/AssetCompiler.h>
#include <Content/Assets/AnimationAsset.h>

//Systems.
#include <Systems/System.h>

class AnimationAssetCompiler final : public AssetCompiler
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		AnimationAssetCompiler
	);

	/*
	*	Default constructor.
	*/
	AnimationAssetCompiler() NOEXCEPT;

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
		return new (_AssetAllocator.Allocate()) AnimationAsset();
	}

	/*
	*	Loads a single asset with the given load context.
	*/
	void Load(const LoadContext &load_context) NOEXCEPT override;

private:

	//The asset allocator.
	PoolAllocator<sizeof(AnimationAsset)> _AssetAllocator;

};