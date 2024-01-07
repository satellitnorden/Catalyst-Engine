//Header file.
#include <Rendering/Native/PostProcessingSystem.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Initializes the post processing system.
*/
void PostProcessingSystem::Initialize() NOEXCEPT
{
	//Register the uniform data.
	RenderingSystem::Instance->GetBufferManager()->RegisterUniformBuffer
	(
		HashString("PostProcessing"),
		&_PostProcessingUniformData,
		sizeof(PostProcessingUniformData)
	);
}

/*
*	Updates the post processing system during the render update phase.
*/
void PostProcessingSystem::RenderUpdate() NOEXCEPT
{
	//Update the post processing uniform data.
	_PostProcessingUniformData._BloomThreshold = _BloomThreshold;
	_PostProcessingUniformData._BloomIntensity = _BloomIntensity;
	_PostProcessingUniformData._MotionBlurIntensity = _MotionBlurIntensity;
}