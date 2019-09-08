//Header file.
#include <Rendering/Native/MaterialSystem.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Post-initializes the material system.
*/
void MaterialSystem::PostInitialize() NOEXCEPT
{

}

/*
*	Returns the current global material uniform buffer.
*/
BufferHandle MaterialSystem::GetCurrentGlobalMaterialUnifomBuffer() NOEXCEPT
{
	return _GlobalMaterialUniformBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}