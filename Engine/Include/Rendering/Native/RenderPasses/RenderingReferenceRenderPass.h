#if !defined(CATALYST_SIMPLIFIED_RENDERING)
#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/RenderingReferenceGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class RenderingReferenceRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(RenderingReferenceRenderPass);

	/*
	*	Default constructor.
	*/
	RenderingReferenceRenderPass() NOEXCEPT;

	/*
	*	Sets the properties.
	*/
	void SetProperties(const uint32 rendering_reference_texture_index, const uint32 iterations) NOEXCEPT;

private:

	//The rendering reference graphics pipeline.
	RenderingReferenceGraphicsPipeline _RenderingReferenceGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};
#endif
#endif