//Header file.
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

/*
*	Resets this pipeline.
*/
void Pipeline::ResetPipeline() NOEXCEPT
{
	//Clear the render data table layouts.
	_RenderDataTableLayouts.Clear();

	//Clear the push constant ranges.
	_PushConstantRanges.Clear();
}