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
	//Update the rendering configuration uniform data.
	_RenderingConfigurationUniformData._VolumetricShadowsMode = static_cast<uint32>(_VolumetricShadowsMode);
}