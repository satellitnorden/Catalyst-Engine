#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/UpdateContext.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/ShaderMaterial.h>

class MaterialSystem final
{

public:

	/*
	*	Default constructor.
	*/
	MaterialSystem() NOEXCEPT;

	/*
	*	Post-initializes the material system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Updates the material system during the render update phase.
	*/
	void RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Registers a material.
	*/
	NO_DISCARD uint32 RegisterMaterial(const MaterialResource *const RESTRICT resource) NOEXCEPT;

	/*
	*	Returns the current material uniform buffer.
	*/
	BufferHandle GetCurrentMaterialUnifomBuffer() NOEXCEPT;

private:

	//Container for all material slots.
	StaticArray<bool, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS> _MaterialSlots;

	//Container for all material resources
	StaticArray<const MaterialResource *RESTRICT, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS> _MaterialResources;

	//Container for all shader materials.
	StaticArray<ShaderMaterial, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS> _ShaderMaterials;

	//The material uniform buffers.
	DynamicArray<BufferHandle> _MaterialUniformBuffers;

};