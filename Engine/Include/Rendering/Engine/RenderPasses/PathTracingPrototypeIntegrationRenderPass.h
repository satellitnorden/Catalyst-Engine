#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class PathTracingPrototypeIntegrationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PathTracingPrototypeIntegrationRenderPass);

	/*
	*	Default constructor.
	*/
	PathTracingPrototypeIntegrationRenderPass() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The number of accumulations.
	int32 _Accumulations{ 0 };

	//The previous perceiver matrix.
	Matrix4 _PreviousPerceiverMatrix;

	/*
	*	Initializes the path tracing prototype integration render pass.
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
	*	Renders the path tracing prototype integration.
	*/
	void RenderInternal() NOEXCEPT;

};