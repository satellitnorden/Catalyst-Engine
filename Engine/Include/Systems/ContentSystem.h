#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>
#include <Core/General/HashString.h>

//Content.
#include <Content/Core/ContentCore.h>
#include <Content/Core/AssetCompiler.h>

//Forward declarations.
class ContentCache;

class ALIGN(8) ContentSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ContentSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ContentSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the content system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Registers an asset compiler.
	*/
	void RegisterAssetCompiler(const HashString asset_identifier, AssetCompiler *const RESTRICT asset_compiler) NOEXCEPT;

	/*
	*	Compiles the content for engine.
	*	Returns if new content was compiled.
	*/
	NO_DISCARD bool CompileEngine() NOEXCEPT;

	/*
	*	Compiles the content for game.
	*	Returns if new content was compiled.
	*/
	NO_DISCARD bool CompileGame() NOEXCEPT;

private:

	//The asset compilers.
	HashTable<HashString, AssetCompiler *const RESTRICT> _AssetCompilers;

	/*
	*	Scans assets in the given directory.
	*	Returns if new content was compiled.
	*/
	NO_DISCARD bool ScanAssetsInDirectory(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const char *const RESTRICT directory_path) NOEXCEPT;

	/*
	*	Parses content definitions in the given directory.
	*	Returns if new content was compiled.
	*/
	NO_DISCARD bool ParseContentDefinitionsInDirectory(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const char *const RESTRICT directory_path) NOEXCEPT;

	/*
	*	Parses a Font from the given file.
	*/
	void ParseFont(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

	/*
	*	Parses a Material from the given file.
	*/
	void ParseMaterial(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

	/*
	*	Parses a Model from the given file.
	*/
	void ParseModel(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

	/*
	*	Parses a Texture2D from the given file.
	*/
	void ParseTexture2D(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

	/*
	*	Parses a TextureCube from the given file.
	*/
	void ParseTextureCube(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

	/*
	*	Parses a procedural tree model from the given file.
	*/
	void ParseProceduralTreeModel(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

	/*
	*	Parses an Impostor Material from the given file.
	*/
	void ParseImpostorMaterial(const CompilationDomain compilation_domain, ContentCache *const RESTRICT content_cache, const std::string &name, const DynamicString &package, std::ifstream &file) NOEXCEPT;

};
#endif