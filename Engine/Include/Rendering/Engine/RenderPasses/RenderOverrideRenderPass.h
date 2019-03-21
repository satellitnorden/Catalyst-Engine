#if defined(CATALYST_ENABLE_RENDER_OVERRIDE)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class RenderOverrideRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(RenderOverrideRenderPass);

	/*
	*	Default constructor.
	*/
	RenderOverrideRenderPass() NOEXCEPT;

	/*
	*	Sets the texture.
	*/
	void SetTexture(const Texture2DHandle texture) NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The texture.
	Texture2DHandle _Texture{ EMPTY_HANDLE };

	/*
	*	Initializes the render override render pass.
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
	*	Renders the render override.
	*/
	void RenderInternal() NOEXCEPT;

};
#endif