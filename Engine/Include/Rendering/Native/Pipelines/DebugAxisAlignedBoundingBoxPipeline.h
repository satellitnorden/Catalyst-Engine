#if defined(CATALYST_CONFIGURATION_DEBUG)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Pipeline.h>

class DebugAxisAlignedBoundingBoxPipeline final : public Pipeline
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DebugAxisAlignedBoundingBoxPipeline);

	/*
	*	Default constructor.
	*/
	DebugAxisAlignedBoundingBoxPipeline() NOEXCEPT;

private:

	/*
	*	Initializes the debug axis-aligned bounding boxes pipeline.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the debug axis-aligned bounding boxes.
	*/
	void RenderInternal() NOEXCEPT;

};
#endif