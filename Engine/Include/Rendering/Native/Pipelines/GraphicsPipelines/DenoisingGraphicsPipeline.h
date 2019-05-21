#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class DenoisingGraphicsPipeline final : public GraphicsPipeline
{

public:

	//Enumeration covering all direction.
	enum class Direction : uint8
	{
		Horizontal,
		Vertical
	};

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const Direction direction, const RenderTargetHandle source1, const RenderTargetHandle source2, const RenderTargetHandle target1, const RenderTargetHandle target2) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The direction.
	Direction _Direction;

	//The current buffer index.
	uint8 _CurrentBufferIndex{ 0 };

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data tables.
	*/
	void CreateRenderDataTables(const RenderTargetHandle source1, const RenderTargetHandle source2) NOEXCEPT;

};