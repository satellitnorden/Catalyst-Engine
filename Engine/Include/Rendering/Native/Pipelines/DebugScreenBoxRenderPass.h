#if defined(CATALYST_CONFIGURATION_DEBUG)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Pipeline.h>

class DebugScreenBoxRenderPass final : public Pipeline
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DebugScreenBoxRenderPass);

	/*
	*	Default constructor.
	*/
	DebugScreenBoxRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the debug screen boxes render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the debug screen boxes.
	*/
	void RenderInternal() NOEXCEPT;

};
#endif