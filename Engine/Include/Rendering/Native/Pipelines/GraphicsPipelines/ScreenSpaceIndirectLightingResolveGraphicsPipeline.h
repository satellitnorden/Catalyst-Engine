#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class ScreenSpaceIndirectLightingResolveGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const RenderTargetHandle source_render_target,
					const Resolution source_render_resolution,
					const RenderTargetHandle temporal_reprojection_buffer) NOEXCEPT;

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

	//The source render resolution.
	Resolution _SourceRenderResolution;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable(const RenderTargetHandle source_render_target) NOEXCEPT;

};