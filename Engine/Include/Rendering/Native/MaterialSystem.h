#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/UpdateContext.h>

//Rendering.
#include <Rendering/Native/Material.h>
#include <Rendering/Native/RenderingCore.h>

class MaterialSystem final
{

public:

	/*
	*	Post-initializes the material system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Updates the material system during the render update phase.
	*/
	void RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Registers a global material.
	*/
	void RegisterGlobalMaterial(const uint32 index, const Material &material) NOEXCEPT;

	/*
	*	Returns the current global material uniform buffer.
	*/
	BufferHandle GetCurrentGlobalMaterialUnifomBuffer() NOEXCEPT;

private:

	//The global materials.
	StaticArray<Material, RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS> _GlobalMaterials;

	//The global material uniform buffers.
	DynamicArray<BufferHandle> _GlobalMaterialUniformBuffers;

};