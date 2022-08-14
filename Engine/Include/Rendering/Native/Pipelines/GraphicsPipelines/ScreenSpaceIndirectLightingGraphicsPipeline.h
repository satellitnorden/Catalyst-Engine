#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingConfiguration.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class ScreenSpaceIndirectLightingGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const RenderingConfiguration::IndirectLightingQuality quality) NOEXCEPT;

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
	void CreateRenderDataTable(const RenderingConfiguration::IndirectLightingQuality quality) NOEXCEPT;

};