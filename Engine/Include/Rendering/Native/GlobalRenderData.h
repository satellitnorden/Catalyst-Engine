#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Multithreading.
#include <Multithreading/Spinlock.h>

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
	StaticArray<bool, RenderingConstants::MAXIMUM_NUMBER_OF_GLOBAL_TEXTURES> _GlobalTextureSlots;

	//The remove global texture updates.
	DynamicArray<DynamicArray<uint32>> _RemoveGlobalTextureUpdates;

	//The add global texture updates.
	DynamicArray<DynamicArray<Pair<uint32, Texture2DHandle>>> _AddGlobalTextureUpdates;

	//The terrain height texture lock.
	Spinlock _TerrainHeightTexturesLock;

	//The terrain height texture slots.
	StaticArray<bool, RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES> _TerrainHeightTextureSlots;

	//The remove terrain height texture updates.
	DynamicArray<DynamicArray<uint8>> _RemoveTerrainHeightTextureUpdates;

	//The add terrain height texture updates.
	DynamicArray<DynamicArray<Pair<uint8, Texture2DHandle>>> _AddTerrainHeightTextureUpdates;

	//The terrain patch data buffers.
	DynamicArray<BufferHandle> _TerrainPatchDataBuffers;

	//The terrain material data buffers.
	DynamicArray<BufferHandle> _TerrainMaterialDataBuffers;

};