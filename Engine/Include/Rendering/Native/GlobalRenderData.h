#pragma once

//Concurrency.
#include <Concurrency/Spinlock.h>

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class GlobalRenderData final
{

public:

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The dynamic uniform data buffers.
	DynamicArray<BufferHandle> _DynamicUniformDataBuffers;

	//The global texture lock.
	Spinlock _GlobalTexturesLock;

	//Theglobal texture slots.
	StaticArray<bool, CatalystShaderConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES> _GlobalTextureSlots;

	//The remove global texture updates.
	DynamicArray<DynamicArray<uint32>> _RemoveGlobalTextureUpdates;

	//The add global texture updates.
	DynamicArray<DynamicArray<Pair<uint32, Texture2DHandle>>> _AddGlobalTextureUpdates;

};