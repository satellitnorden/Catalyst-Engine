#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class ALIGN(8) ContentCompiler final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ContentCompiler);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ContentCompiler() NOEXCEPT
	{

	}

	/*
	*	Runs the content compiler for engine.
	*	Returns if new content was compiled.
	*/
	NO_DISCARD bool RunEngine() NOEXCEPT;

	/*
	*	Runs the content compiler for game.
	*	Returns if new content was compiled.
	*/
	NO_DISCARD bool RunGame() NOEXCEPT;

private:

	//Enumeration covering all compilation domains.
	enum class CompilationDomain : uint8
	{
		ENGINE,
		GAME
	};

	/*
	*	Parses content definitions in the given directory.
	*	Returns if new content was compiled.
	*/
	NO_DISCARD bool ParseContentDefinitionsInDirectory(const CompilationDomain compilation_domain, class ContentCache *const RESTRICT content_cache, const char *const RESTRICT directory_path) NOEXCEPT;

	/*
	*	Parses a Material from the given file.
	*/
	void ParseMaterial(const CompilationDomain compilation_domain, class ContentCache *const RESTRICT content_cache, const std::string &name, std::ifstream &file) NOEXCEPT;


	/*
	*	Parses a Texture2D from the given file.
	*/
	void ParseTexture2D(const CompilationDomain compilation_domain, class ContentCache *const RESTRICT content_cache, const std::string &name, std::ifstream &file) NOEXCEPT;

};
#endif