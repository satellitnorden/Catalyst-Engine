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