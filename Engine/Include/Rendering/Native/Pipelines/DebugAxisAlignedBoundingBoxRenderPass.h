#if defined(CATALYST_CONFIGURATION_DEBUG)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Pipeline.h>

class DebugAxisAlignedBoundingBoxRenderPass final : public Pipeline
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DebugAxisAlignedBoundingBoxRenderPass);

	/*
	*	Default constructor.
	*/
	DebugAxisAlignedBoundingBoxRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the debug axis-aligned bounding boxes render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the debug axis-aligned bounding boxes.
	*/
	void RenderInternal() NOEXCEPT;

};
#endif