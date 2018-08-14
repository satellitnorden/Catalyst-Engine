#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class BloomRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(BloomRenderPass);

	/*
	*	Default constructor.
	*/
	BloomRenderPass() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle renderDataTableLayout;

	//The render data table.
	RenderDataTableHandle renderDataTable;

	/*
	*	Initializes the bloom render pass.
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
	*	Renders the bloom.
	*/
	void RenderInternal() NOEXCEPT;

};