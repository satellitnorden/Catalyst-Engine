//Header file.
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Post-initializes this pipeline.
*/
void Pipeline::PostInitialize() NOEXCEPT
{
	RenderingSystem::Instance->InitializePipeline(this);
}


/*
*	Returns the current command buffer, non-const.
*/
RESTRICTED NO_DISCARD CommandBuffer *const RESTRICT Pipeline::GetCurrentCommandBuffer() NOEXCEPT
{
	return _CommandBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}
