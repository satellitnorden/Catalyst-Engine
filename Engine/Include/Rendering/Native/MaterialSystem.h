#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

//Concurrency.
#include <Concurrency/Spinlock.h>

//Content.
#include <Content/Assets/MaterialAsset.h>

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
	void RenderUpdate() NOEXCEPT;

	/*
	*	Registers a material.
	*/
	NO_DISCARD uint32 RegisterMaterial(const MaterialAsset *const RESTRICT material) NOEXCEPT;

	/*
	*	Returns the material asset at the given index.
	*/
	NO_DISCARD const MaterialAsset *const RESTRICT GetMaterial(const uint32 index) NOEXCEPT;

	/*
	*	Returns the current material uniform buffer.
	*/
	BufferHandle GetCurrentMaterialUnifomBuffer() NOEXCEPT;

private:

	//The materials lock.
	Spinlock _MaterialsLock;

	//Container for all material slots.
	StaticArray<bool, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS> _MaterialSlots;

	//Container for all material assets
	StaticArray<const MaterialAsset *RESTRICT, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS> _Materials;

	//Container for all shader materials.
	StaticArray<ShaderMaterial, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_MATERIALS> _ShaderMaterials;

	//The material uniform buffers.
	DynamicArray<BufferHandle> _MaterialUniformBuffers;

};