#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class OceanGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//The ocean texture.
	Texture2DHandle _OceanTexture;

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	/*
	*	Returns the ocean height at the given coordinate.
	*/
	float OceanHeight(const Vector2<float> coordinate) const NOEXCEPT;

	/*
	*	Returns the ocean normal at the given coordinate.
	*/
	void OceanNormal(const Vector2<float> coordinate, Vector3<float> *const RESTRICT normal, float *const RESTRICT height) const NOEXCEPT;

	/*
	*	Creates the ocean texture.
	*/
	void CreateOceanTexture() NOEXCEPT;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

};