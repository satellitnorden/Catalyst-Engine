#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/VirtualRealityGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class VirtualRealityRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		VirtualRealityRenderPass
	);

	/*
	*	Default constructor.
	*/
	VirtualRealityRenderPass() NOEXCEPT;

private:

	//The virtual reality graphics pipeline.
	VirtualRealityGraphicsPipeline _VirtualRealityGraphicsPipeline;

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