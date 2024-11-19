//Header file.
#include <Rendering/Native/MaterialSystem.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
MaterialSystem::MaterialSystem() NOEXCEPT
{
	SCOPED_LOCK(_MaterialsLock);

	//Mark all material slots as free.
	for (uint32 i{ 0 }; i < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS; ++i)
	{
		_MaterialSlots[i] = false;
	}

	//Reset all material resources.
	for (uint32 i{ 0 }; i < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS; ++i)
	{
		_Materials[i] = nullptr;
	}
}

/*
*	Post-initializes the material system.
*/
void MaterialSystem::PostInitialize() NOEXCEPT
{
	//Create all the global material uniform buffers.
	const uint8 number_of_framebuffers{ RenderingSystem::Instance->GetNumberOfFramebuffers() };

	_MaterialUniformBuffers.Upsize<false>(number_of_framebuffers);

	for (BufferHandle &buffer : _MaterialUniformBuffers)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(ShaderMaterial) * CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS,
												BufferUsage::UniformBuffer,
												MemoryProperty::HostCoherent | MemoryProperty::HostVisible,
												&buffer);
	}
}

/*
*	Updates the material system during the render update phase.
*/
void MaterialSystem::RenderUpdate() NOEXCEPT
{
	SCOPED_LOCK(_MaterialsLock);

	//Update all shader materials.
	for (uint32 i{ 0 }; i < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS; ++i)
	{
		if (_Materials[i])
		{
			_ShaderMaterials[i] = ShaderMaterial(_Materials[i]);
		}

		else
		{
			break;
		}
	}

	//Update the current global material uniform buffer.
	const void *const RESTRICT data_chunks[]{ _ShaderMaterials.Data() };
	const uint64 data_sizes[]{ sizeof(ShaderMaterial) * CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS };
	BufferHandle &current_buffer{ _MaterialUniformBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &current_buffer);
}

/*
*	Registers a material.
*/
NO_DISCARD uint32 MaterialSystem::RegisterMaterial(const MaterialAsset *const RESTRICT material) NOEXCEPT
{
	SCOPED_LOCK(_MaterialsLock);

	//Find the first free index.
	uint32 index{ UINT32_MAXIMUM };

	for (uint32 i{ 0 }; i < CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS; ++i)
	{
		if (!_MaterialSlots[i])
		{
			_MaterialSlots[i] = true;
			index = i;

			break;
		}
	}

	ASSERT(index != UINT32_MAXIMUM, "Couldn't find a free material slot, increase maximum number of materials!");

	//Add the material.
	_Materials[index] = material;

	//Return the index.
	return index;
}

/*
*	Returns the material asset at the given index.
*/
NO_DISCARD const MaterialAsset *const RESTRICT MaterialSystem::GetMaterial(const uint32 index) NOEXCEPT
{
	SCOPED_LOCK(_MaterialsLock);

	return _Materials[index];
}

/*
*	Returns the current material uniform buffer.
*/
BufferHandle MaterialSystem::GetCurrentMaterialUnifomBuffer() NOEXCEPT
{
	return _MaterialUniformBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}