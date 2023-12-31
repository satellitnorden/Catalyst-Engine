//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

/*
*	Default constructor.
*/
GraphicsPipeline::GraphicsPipeline() NOEXCEPT
{
	//Set the type.
	SetType(Type::GRAPHICS);
}

/*
*	Resetis this graphics pipeline.
*/
void GraphicsPipeline::ResetGraphicsPipeline() NOEXCEPT
{	
	//Reset this pipeline.
	ResetPipeline();

	//Clear the output render targets.
	_OutputRenderTargets.Clear();

	//Clear the vertex input attribute descriptions.
	_VertexInputAttributeDescriptions.Clear();

	//Clear the vertex input binding descriptions.
	_VertexInputBindingDescriptions.Clear();
}