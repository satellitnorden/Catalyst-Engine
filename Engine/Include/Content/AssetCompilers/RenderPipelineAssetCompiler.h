#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Content.
#include <Content/Core/AssetCompiler.h>
#include <Content/Assets/RenderPipelineAsset.h>

//Systems.
#include <Systems/System.h>

class RenderPipelineAssetCompiler final : public AssetCompiler
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		RenderPipelineAssetCompiler
	);

	/*
	*	Default constructor.
	*/
	RenderPipelineAssetCompiler() NOEXCEPT;

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
		RenderPipelineAsset *RESTRICT _Asset;

	};

	//The compile data allocator.
	PoolAllocator<sizeof(CompileData)> _CompileDataAllocator;

	//The load data allocator.
	PoolAllocator<sizeof(LoadData)> _LoadDataAllocator;

	//The asset allocator.
	PoolAllocator<sizeof(RenderPipelineAsset)> _AssetAllocator;

	/*
	*	Compiles internally.
	*/
	void CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT;

	/*
	*	Loads internally.
	*/
	void LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT;

	/*
	*	Retrieves all lines from the given file.
	*/
	void RetrieveLines(const char *const RESTRICT file_path, DynamicArray<DynamicString> *const RESTRICT lines) NOEXCEPT;

	/*
	*	Strips comments out of the given lines.
	*/
	void StripComments(DynamicArray<DynamicString> *const RESTRICT lines) NOEXCEPT;

	/*
	*	Searches for the given file, first in the engine directory, then the game directory.
	*/
	NO_DISCARD bool SearchForFile
	(
		const char *const RESTRICT engine_directory_path,
		const char *const RESTRICT game_directory_path,
		char *const RESTRICT file_path
	) NOEXCEPT;

	/*
	*	Resolves an include.
	*/
	void ResolveInclude
	(
		const DynamicString &current_line,
		const char *const RESTRICT extension,
		const char *const RESTRICT sub_folder,
		DynamicArray<DynamicString> *const RESTRICT lines,
		const uint64 current_line_index,
		const char *const RESTRICT replacement_line = nullptr
	) NOEXCEPT;

	/*
	*	Resolves includes.
	*/
	void ResolveIncludes(DynamicArray<DynamicString> *const RESTRICT lines) NOEXCEPT;

	/*
	*	Consumes settings.
	*/
	void ConsumeSettings(RenderPipelineAsset *const RESTRICT asset, class ExtraData *const RESTRICT extra_data, DynamicArray<DynamicString> *const RESTRICT lines) NOEXCEPT;

	/*
	*	Splits shader stages.
	*/
	void SplitShaderStages(const DynamicArray<DynamicString> &lines, DynamicArray<class ShaderStageLines> *const RESTRICT shader_stages) NOEXCEPT;

	/*
	*	Compiles GLSL shaders.
	*/
	void CompileGLSLShaders
	(
		const CompileData &compile_data,
		class ExtraData *const RESTRICT extra_data,
		const DynamicArray<class ShaderStageLines> &shader_stages
	) NOEXCEPT;

};