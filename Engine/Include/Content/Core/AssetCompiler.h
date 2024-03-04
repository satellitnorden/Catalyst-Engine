#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/ContentCore.h>

//Forward declarations.
class ContentCache;

/*
*	Base class for compilers that handle a specific asset.
*	Needs to be registered to the content system, which oversees handling of all assets.
*/
class AssetCompiler
{

public:

	//Enumeration covering all flags.
	enum class Flags : uint8
	{
		/*
		*	No flags. (:
		*/
		NONE = 0,

		/*
		*	Makes this disregard whether or not a file has changed, and always compile.
		*/
		ALWAYS_COMPILE = BIT(0)
	};

	/*
	*	Compile context class definition.
	*/
	class CompileContext final
	{

	public:

		//The compilation domain.
		CompilationDomain _CompilationDomain;

		//The identifier.
		uint64 _Identifier;

		//The last write time.
		std::filesystem::file_time_type _LastWriteTime;

		//The file path.
		const char *RESTRICT _FilePath;

		//The name.
		const char *RESTRICT _Name;

		//The content cache.
		ContentCache *RESTRICT _ContentCache;

	};

	//The flags.
	Flags _Flags{ Flags::NONE };

	/*
	*	Runs before compilation in the specified domain has finished.
	*/
	FORCE_INLINE virtual void PreCompile(const CompilationDomain compilation_domain) NOEXCEPT
	{

	}

	/*
	*	Compiles a single asset with the given compile context.
	*/
	virtual void Compile(const CompileContext &compile_context) NOEXCEPT = 0;

	/*
	*	Runs after compilation in the specified domain has finished.
	*/
	FORCE_INLINE virtual void PostCompile(const CompilationDomain compilation_domain) NOEXCEPT
	{

	}

};

ENUMERATION_BIT_OPERATIONS(AssetCompiler::Flags);