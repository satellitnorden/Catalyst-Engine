#if defined(CATALYST_ENABLE_RENDER_OVERRIDE)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

class RenderOverridePipeline final : public Pipeline
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(RenderOverridePipeline);

	/*
	*	Default constructor.
	*/
	RenderOverridePipeline() NOEXCEPT;

	/*
	*	Returns if there is an override.
	*/
	bool HasOverride() const NOEXCEPT
	{
		return _Texture != EMPTY_HANDLE;
	}

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
	*	Initializes the render override pipeline.
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