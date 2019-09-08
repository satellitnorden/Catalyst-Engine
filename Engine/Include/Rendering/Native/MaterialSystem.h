#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class MaterialSystem final
{

public:

	/*
	*	Post-initializes the material system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Returns the current global material uniform buffer.
	*/
	BufferHandle GetCurrentGlobalMaterialUnifomBuffer() NOEXCEPT;

private:

	//The global material uniform buffers.
	DynamicArray<BufferHandle> _GlobalMaterialUniformBuffers;

};