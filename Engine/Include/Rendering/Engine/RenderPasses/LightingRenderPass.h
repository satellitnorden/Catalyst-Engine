#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class LightingRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LightingRenderPass);

	/*
	*	Default constructor.
	*/
	LightingRenderPass() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle renderDataTableLayout;

	//The render data table.
	RenderDataTableHandle renderDataTable;

	/*
	*	Initializes the lighting render pass.
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
	*	Renders the lighting.
	*/
	void RenderInternal() NOEXCEPT;

};