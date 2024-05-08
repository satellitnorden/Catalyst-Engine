//Header file.
#include <Rendering/Native/RenderingConfiguration.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Initializes the rendering configuration.
*/
void RenderingConfiguration::Initialize() NOEXCEPT
{
	//Register the uniform buffer.
	RenderingSystem::Instance->GetBufferManager()->RegisterUniformBuffer
	(
		HashString("RenderingConfiguration"),
		&_RenderingConfigurationUniformData,
		sizeof(RenderingConfigurationUniformData)
	);
}

/*
*	Updates the rendering configuration.
*/
void RenderingConfiguration::Update() NOEXCEPT
{
	//Update the rendering configuration uniform data
	if (_DiffuseIrradianceMode == DiffuseIrradianceMode::RAY_TRACED && !RenderingSystem::Instance->IsRayTracingPossible())
	{
		_RenderingConfigurationUniformData._DiffuseIrradianceMode = static_cast<uint32>(DiffuseIrradianceMode::NONE);
	}

	else
	{
		_RenderingConfigurationUniformData._DiffuseIrradianceMode = static_cast<uint32>(_DiffuseIrradianceMode);
	}

	_RenderingConfigurationUniformData._SpecularIrradianceMode = static_cast<uint32>(_SpecularIrradianceMode);
	_RenderingConfigurationUniformData._VolumetricShadowsMode = static_cast<uint32>(_VolumetricShadowsMode);
}