#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class RenderingReferenceGraphicsPipeline final : public GraphicsPipeline
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

	/*
	*	Sets the properties.
	*/
	void SetProperties(const uint32 rendering_reference_texture_index, const uint32 iterations) NOEXCEPT;

private:

	//The rendering reference texture index.
	uint32 _RenderingReferenceTextureIndex{ 0 };

	//The iterations.
	float _Iterations{ 1.0f };

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

};
#endif