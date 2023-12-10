#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class ALIGN(8) RenderingCompiler final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(RenderingCompiler);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE RenderingCompiler() NOEXCEPT
	{

	}

	/*
	*	Runs the rendering compiler.
	*	Returns if new rendering data was compiled.
	*/
	NO_DISCARD bool Run() NOEXCEPT;

private:

	/*
	*	Parses render pipelines in the given directory.
	*	Returns if new rendering data was compiled.
	*/
	NO_DISCARD bool ParseRenderPipelinesInDirectory(const char *const RESTRICT directory_path) NOEXCEPT;

};
#endif