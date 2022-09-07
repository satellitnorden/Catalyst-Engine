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
	void Initialize(const RenderTargetHandle temporal_reprojection_buffer,
					const RenderTargetHandle previous_temporal_buffer,
					const RenderTargetHandle current_temporal_buffer) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this graphics pipeline.
	*/
	void Terminate() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout{ EMPTY_HANDLE };

	//The render data table.
	RenderDataTableHandle _RenderDataTable{ EMPTY_HANDLE };

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable(const RenderTargetHandle temporal_reprojection_buffer, const RenderTargetHandle previous_temporal_buffer) NOEXCEPT;

};