#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Content.
#include <Content/Core/AssetCompiler.h>
#include <Content/Assets/Texture2DAsset.h>

class ALIGN(8) Texture2DAssetCompiler final : public AssetCompiler
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(Texture2DAssetCompiler);

	/*
	*	Default constructor.
	*/
	Texture2DAssetCompiler() NOEXCEPT;

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
	*	Texture 2D compile data class definition.
	*/
	class Texture2DCompileData final
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

	//The compile data allocator.
	PoolAllocator<sizeof(Texture2DCompileData)> _CompileDataAllocator;

	//The asset allocator.
	PoolAllocator<sizeof(Texture2DAsset)> _AssetAllocator;

	/*
	*	Compiles internally.
	*/
	void CompileInternal(Texture2DCompileData *const RESTRICT compile_data) NOEXCEPT;

};