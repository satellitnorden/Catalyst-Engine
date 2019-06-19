#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class TemporalAccumulationGraphicsPipeline final : public GraphicsPipeline
{

public:
	
	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const RenderTargetHandle source1,
					const RenderTargetHandle source2,
					const RenderTargetHandle target1,
					const RenderTargetHandle target2,
					const float feedbackFactor,
					const Resolution resolution) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The feedback factor.
	float _FeedbackFactor;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable(const RenderTargetHandle source1, const RenderTargetHandle source2) NOEXCEPT;

};