//Header file.
#include <Rendering/Native/MaterialSystem.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Post-initializes the material system.
*/
void MaterialSystem::PostInitialize() NOEXCEPT
{
	//Mark all material slots as free.
	for (uint32 i{ 0 }; i < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS; ++i)
	{
		_MaterialSlots[i] = false;
	}

	//Create all the global material uniform buffers.
	const uint8 number_of_framebuffers{ RenderingSystem::Instance->GetNumberOfFramebuffers() };

	_MaterialUniformBuffers.Upsize<false>(number_of_framebuffers);

	for (BufferHandle &buffer : _MaterialUniformBuffers)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(Material) * CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS,
												BufferUsage::UniformBuffer,
												MemoryProperty::HostCoherent | MemoryProperty::HostVisible,
												&buffer);
	}
}

/*
*	Updates the material system during the render update phase.
*/
void MaterialSystem::RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the current global material uniform buffer.
	const void *const RESTRICT data_chunks[]{ _GlobalMaterials.Data() };
	const uint64 data_sizes[]{ sizeof(Material) * CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS };
	BufferHandle &current_buffer{ _MaterialUniformBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &current_buffer);
}

/*
*	Registers a global material.
*/
void MaterialSystem::RegisterGlobalMaterial(const uint32 index, const Material &material) NOEXCEPT
{
	ASSERT(index < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS, "Index cannot be higher or equal to the maximum amount of global materials!");

	Memory::Copy(&_GlobalMaterials[index], &material, sizeof(Material));
}

/*
*	Returns the global material at the given index.
*/
Material& MaterialSystem::GetGlobalMaterial(const uint32 index) NOEXCEPT
{
	return _GlobalMaterials[index];
}

/*
*	Returns the current material uniform buffer.
*/
BufferHandle MaterialSystem::GetCurrentMaterialUnifomBuffer() NOEXCEPT
{
	return _MaterialUniformBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}