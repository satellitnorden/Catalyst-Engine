//Header file.
#include <Rendering/Native/Pipelines/GraphicsPipelines/GraphicsPipeline.h>

//Rendering.
#include <Rendering/Native/CommandBuffer.h>

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

/*
*	Returns the current command buffer, const.
*/
RESTRICTED const CommandBuffer *const RESTRICT GraphicsPipeline::GetCurrentCommandBuffer() const NOEXCEPT
{
	return _CommandBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}

/*
*	Returns the current command buffer, non-const.
*/
CommandBuffer *const RESTRICT GraphicsPipeline::GetCurrentCommandBuffer() NOEXCEPT
{
	return _CommandBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}