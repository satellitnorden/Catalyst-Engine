//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
GraphicsPipeline::GraphicsPipeline() NOEXCEPT
{
	//Set the type.
	SetType(Type::Graphics);
}