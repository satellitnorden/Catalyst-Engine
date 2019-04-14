#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class RadianceIntegrationGraphicsPipeline final : public GraphicsPipeline
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(RadianceIntegrationGraphicsPipeline);

	/*
	*	Default constructor.
	*/
	RadianceIntegrationGraphicsPipeline() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The current perceiver matrix.
	Matrix4 _CurrentPerceiverMatrix;

	//The number of accumulations.
	int32 _Accumulations{ 0 };

	/*
	*	Initializes the radiance integration graphics pipeline.
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
	*	Renders the radiance integration.
	*/
	void RenderInternal() NOEXCEPT;

};