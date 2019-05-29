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
	void Initialize(const Direction direction, const float stride, const RenderTargetHandle source, const RenderTargetHandle target) NOEXCEPT;

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

	//The stride.
	float _Stride;

	//The current buffer index.
	uint8 _CurrentBufferIndex{ 0 };

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data tables.
	*/
	void CreateRenderDataTables(const RenderTargetHandle source) NOEXCEPT;

};