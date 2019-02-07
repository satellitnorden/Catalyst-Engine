#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class SkyRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(SkyRenderPass);

	/*
	*	Default constructor.
	*/
	SkyRenderPass() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	/*
	*	Initializes the sky render pass.
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
	*	Renders the sky.
	*/
	void RenderInternal() NOEXCEPT;

};