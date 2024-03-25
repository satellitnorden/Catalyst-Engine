#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Content.
#include <Content/Core/AssetCompiler.h>

class ALIGN(8) ImpostorMaterialAssetCompiler final : public AssetCompiler
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ImpostorMaterialAssetCompiler);

	/*
	*	Default constructor.
	*/
	ImpostorMaterialAssetCompiler() NOEXCEPT;

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

	//The compile data allocator.
	PoolAllocator<sizeof(CompileData)> _CompileDataAllocator;

	/*
	*	Compiles internally.
	*/
	void CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT;

};