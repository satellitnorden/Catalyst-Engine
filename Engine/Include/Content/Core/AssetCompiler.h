#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StreamArchive.h>
#include <Core/General/HashString.h>

//Concurrency.
#include <Concurrency/Task.h>

//Content.
#include <Content/Core/ContentCore.h>

//Memory.
#include <Memory/PoolAllocator.h>

//Forward declarations.
class Asset;
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

		//The collection.
		const char *RESTRICT _Collection;

		//The file path.
		const char *RESTRICT _FilePath;

		//The name.
		const char *RESTRICT _Name;

		//The content cache.
		ContentCache *RESTRICT _ContentCache;

		//The task allocator.
		PoolAllocator<sizeof(Task)> *RESTRICT _TaskAllocator;

		//The tasks.
		DynamicArray<Task *RESTRICT> *RESTRICT _Tasks;

	};

	/*
	*	Load context class definition.
	*/
	class LoadContext final
	{

	public:

		//The stream archive.
		StreamArchive *RESTRICT _StreamArchive;

		//The stream archive position.
		uint64 _StreamArchivePosition;

		//The task allocator.
		PoolAllocator<sizeof(Task)> *RESTRICT _TaskAllocator;

		//The tasks.
		DynamicArray<Task *RESTRICT> *RESTRICT _Tasks;

		//The asset. Only set if the asset is being reloaded.
		Asset *RESTRICT _Asset;

		
	};

	//The flags.
	Flags _Flags{ Flags::NONE };

	/*
	*	Denotes if this asset compiler triggers a recompile when it's run.
	*	This can be set if an asset compiler produces files that should be consumed by other asset compilers.
	*/
	bool _TriggerRecompile{ false };

	/*
	*	Returns the asset type identifier.
	*/
	virtual NO_DISCARD HashString AssetTypeIdentifier() const NOEXCEPT = 0;

	/*
	*	Returns the current version.
	*/
	virtual NO_DISCARD uint64 CurrentVersion() const NOEXCEPT = 0;

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

	/*
	*	Loads a single asset with the given load context.
	*/
	virtual NO_DISCARD Asset *const RESTRICT Load(const LoadContext &load_context) NOEXCEPT = 0;

	/*
	*	Runs after load.
	*/
	FORCE_INLINE virtual void PostLoad() NOEXCEPT
	{

	}

protected:

	/*
	*	Returns the source directory for the given compilation domain.
	*/
	FORCE_INLINE NO_DISCARD const char *const RESTRICT GetSourceDirectoryPath(const CompilationDomain compilation_domain) NOEXCEPT
	{
		switch (compilation_domain)
		{
			case CompilationDomain::ENGINE:
			{
				return "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Source";
			}

			case CompilationDomain::GAME:
			{
				return "..\\..\\..\\Content\\Source";
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return "";
			}
		}
	}

	/*
	*	Returns the assets directory for the given compilation domain.
	*/
	FORCE_INLINE NO_DISCARD const char *const RESTRICT GetAssetsDirectoryPath(const CompilationDomain compilation_domain) NOEXCEPT
	{
		switch (compilation_domain)
		{
			case CompilationDomain::ENGINE:
			{
				return "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Assets";
			}

			case CompilationDomain::GAME:
			{
				return "..\\..\\..\\Content\\Assets";
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return "";
			}
		}
	}

	/*
	*	Returns the compiled directory for the given compilation domain.
	*/
	FORCE_INLINE NO_DISCARD const char *const RESTRICT GetCompiledDirectoryPath(const CompilationDomain compilation_domain) NOEXCEPT
	{
		switch (compilation_domain)
		{
			case CompilationDomain::ENGINE:
			{
				return "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Compiled";
			}

			case CompilationDomain::GAME:
			{
				return "..\\..\\..\\Content\\Compiled";
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return "";
			}
		}
	}

};

ENUMERATION_BIT_OPERATIONS(AssetCompiler::Flags);