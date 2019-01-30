#if defined(CATALYST_CONFIGURATION_DEBUG)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DebugAxisAlignedBoundingBoxRenderPass final : public RenderPass
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