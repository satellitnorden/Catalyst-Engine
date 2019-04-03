#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

class AntiAliasingPipeline final : public Pipeline
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(AntiAliasingPipeline);

	/*
	*	Default constructor.
	*/
	AntiAliasingPipeline() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	/*
	*	Initializes the anti-aliasing pipeline.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

	/*
	*	Renders the anti-aliasing.
	*/
	void RenderInternal() NOEXCEPT;

};