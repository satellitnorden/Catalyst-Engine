#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class IndirectLightingTemporalDenoisingGraphicsPipeline final : public GraphicsPipeline
{

public:
	
	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const uint32 source_render_target_index,
					const RenderTargetHandle target) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The source render target index.
	uint32 _SourceRenderTargetIndex;

};