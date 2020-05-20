#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/UpdateContext.h>

//Rendering.
#include <Rendering/Native/Material.h>
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/ShaderMaterial.h>

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
	*	Returns the global material at the given index.
	*/
	Material& GetGlobalMaterial(const uint32 index) NOEXCEPT;

	/*
	*	Returns the current material uniform buffer.
	*/
	BufferHandle GetCurrentMaterialUnifomBuffer() NOEXCEPT;

private:

	//Container for all material slots.
	StaticArray<bool, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS> _MaterialSlots;

	//Container for all shader materials.
	StaticArray<ShaderMaterial, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS> _ShaderMaterials;

	//The material uniform buffers.
	DynamicArray<BufferHandle> _MaterialUniformBuffers;

	//The global materials.
	StaticArray<Material, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS> _GlobalMaterials;

};