#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

class VegetationSceneFeaturesGraphicsPipeline final : public GraphicsPipeline
{

public:

	/*
	*	Initializes this graphics pipeline.
	*/
	void Initialize(const DepthBufferHandle depth_buffer, const bool draw_double_sided_materials) NOEXCEPT;

	/*
	*	Executes this graphics pipeline.
	*/
	void Execute() NOEXCEPT;

private:

	//Denotes if this graphics pipeline should draw double-sided materials
	bool _DrawDoubleSidedMaterials{ false };

};