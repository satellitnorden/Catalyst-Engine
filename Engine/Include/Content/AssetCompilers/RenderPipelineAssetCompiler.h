#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

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
	*	Allocates an asset.
	*/
	Asset *const RESTRICT AllocateAsset() NOEXCEPT override
	{
		return new (_AssetAllocator.Allocate()) RenderPipelineAsset();
	}

	/*
	*	Loads a single asset with the given load context.
	*/
	void Load(const LoadContext &load_context) NOEXCEPT override;

private:

	//The asset allocator.
	PoolAllocator<sizeof(RenderPipelineAsset)> _AssetAllocator;

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
		const CompileContext &compile_context,
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
	void ResolveIncludes(const CompileContext &compile_context, DynamicArray<DynamicString> *const RESTRICT lines) NOEXCEPT;

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
		const CompileContext &compile_context,
		class ExtraData *const RESTRICT extra_data,
		const DynamicArray<class ShaderStageLines> &shader_stages
	) NOEXCEPT;

};