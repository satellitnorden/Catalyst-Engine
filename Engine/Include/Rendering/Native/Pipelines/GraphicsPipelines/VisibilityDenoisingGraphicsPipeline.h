#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class VisibilityDenoisingGraphicsPipeline final : public GraphicsPipeline
{

public:
	
	//Enumeration covering all direction.
	enum class Direction : uint8
	{
		Horizontal,
		Vertical
	};

	/*
	*	Initializes this visibility denoising graphics pipeline.
	*/
	void Initialize(const Direction direction, const RenderTargetHandle source, const RenderTargetHandle target) NOEXCEPT;

	/*
	*	Executes this visibility denoising graphics pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The direction.
	Direction _Direction;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable(const RenderTargetHandle source) NOEXCEPT;

};