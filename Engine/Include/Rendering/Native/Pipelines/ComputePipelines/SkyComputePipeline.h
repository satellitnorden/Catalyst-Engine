#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/ComputePipeline.h>

class SkyComputePipeline final : public ComputePipeline
{

public:

	/*
	*	Initializes this compute pipeline.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this compute pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable{ EMPTY_HANDLE };

	//The currently bound sky texture.
	TextureCubeHandle _CurrentlyBoundSkyTexture{ EMPTY_HANDLE };

	//Keeps track of the current face index.
	uint32 _CurrentFaceIndex{ 0 };

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

};