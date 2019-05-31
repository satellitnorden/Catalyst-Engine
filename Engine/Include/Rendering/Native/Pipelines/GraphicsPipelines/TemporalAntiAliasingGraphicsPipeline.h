#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class TemporalAntiAliasingGraphicsPipeline final : public GraphicsPipeline
{

public:
	
	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const StaticArray<RenderTargetHandle, RenderingConstants::TEMPORAL_ANTI_ALIASING_SAMPLES> &sources,
					const RenderTargetHandle target) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable(const StaticArray<RenderTargetHandle, RenderingConstants::TEMPORAL_ANTI_ALIASING_SAMPLES> &sources) NOEXCEPT;

};