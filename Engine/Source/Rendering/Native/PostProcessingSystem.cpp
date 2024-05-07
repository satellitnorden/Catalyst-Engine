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
	_PostProcessingUniformData._Tint = Vector4<float32>(_TintColor, _TintIntensity);
	_PostProcessingUniformData._BloomIntensity = _BloomIntensity;
	_PostProcessingUniformData._Brightness = _Brightness;
	_PostProcessingUniformData._Contrast = _Contrast;
	_PostProcessingUniformData._ChromaticAberrationIntensity = _ChromaticAberrationIntensity;
	_PostProcessingUniformData._Exposure = _Exposure;
	_PostProcessingUniformData._FilmGrainIntensity = _FilmGrainIntensity;
	_PostProcessingUniformData._HorizontalBorder = _HorizontalBorder;
	_PostProcessingUniformData._MotionBlurIntensity = _MotionBlurIntensity;
	_PostProcessingUniformData._Saturation = _Saturation;
}