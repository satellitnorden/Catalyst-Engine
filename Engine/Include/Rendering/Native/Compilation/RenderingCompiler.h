#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Systems.
#include <Systems/System.h>

class RenderingCompiler final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		RenderingCompiler,
		SYSTEM_DEFINED_REQUIREMENT(CATALYST_ENABLE_RESOURCE_BUILDING)
	);

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
	NO_DISCARD bool ParseRenderPipelinesInDirectory(const bool is_game, const char *const RESTRICT directory_path) NOEXCEPT;

};
#endif