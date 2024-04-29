#if defined(CATALYST_PLATFORM_WINDOWS)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class VirtualRealityVisualizationRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		VirtualRealityVisualizationRenderPass
	);

	/*
	*	Default constructor.
	*/
	VirtualRealityVisualizationRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this render pass.
	*/
	void Terminate() NOEXCEPT;

};
#endif