#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Multithreading.
#include <Multithreading/Spinlock.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class GlobalRenderData final
{

public:

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The dynamic uniform data buffers.
	DynamicArray<UniformBufferHandle> _DynamicUniformDataBuffers;

	//The terrain height texture lock.
	Spinlock _TerrainHeightTexturesLock;

	//The terrain height texture slots.
	StaticArray<bool, RenderingConstants::MAXIMUM_NUMBER_OF_TERRAIN_HEIGHT_TEXTURES> _TerrainHeightTextureSlots;

	//The remove terrain height texture updates.
	DynamicArray<DynamicArray<uint8>> _RemoveTerrainHeightTextureUpdates;

	//The add terrain height texture updates.
	DynamicArray<DynamicArray<Pair<uint8, Texture2DHandle>>> _AddTerrainHeightTextureUpdates;

};