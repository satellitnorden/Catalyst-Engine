#if defined(CATALYST_CONFIGURATION_DEBUG)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class DebugScreenBoxPipeline final : public GraphicsPipeline
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DebugScreenBoxPipeline);

	/*
	*	Default constructor.
	*/
	DebugScreenBoxPipeline() NOEXCEPT;

private:

	/*
	*	Initializes the debug screen boxes pipeline.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the debug screen boxes.
	*/
	void RenderInternal() NOEXCEPT;

};
#endif