#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class IndirectLightingSpatialDenoisingGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const int32 stride,
					const int32 direction,
					const RenderTargetHandle source_render_target,
					const RenderTargetHandle scene_features_2_render_target,
					const RenderTargetHandle scene_features_3_render_target,
					const RenderTargetHandle target,
					const Resolution render_resolution) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this graphics pipeline.
	*/
	void Terminate() NOEXCEPT;

private:

	//The stride.
	int32 _Stride;

	//The direction.
	int32 _Direction;

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
	void CreateRenderDataTable(	const RenderTargetHandle source_render_target,
								const RenderTargetHandle scene_features_2_render_target,
								const RenderTargetHandle scene_features_3_render_target) NOEXCEPT;

};