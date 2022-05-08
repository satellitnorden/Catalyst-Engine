#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Spinlock.h>

//Math.
#include <World/Core/WorldPosition.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Terrain.
#include <Terrain/TerrainCore.h>

class TerrainProperties final
{

public:

	//Type aliases.
	using TerrainHeightFunction = float32(*)(const WorldPosition &world_position);
	using TerrainMaterialFunction = void(*)(const WorldPosition &world_position, Vector4<uint8> *const RESTRICT indices, Vector4<uint8> *const RESTRICT weights);

	//The patch size.
	float32 _PatchSize;

	//The patch resolution.
	uint32 _PatchResolution;

	//The maximum material maps resolution.
	uint32 _MaximumMaterialMapsResolution;

	//The maximum quad tree depth.
	uint8 _MaximumQuadTreeDepth;

	//The terrain maximum height.
	float32 _TerrainMaximumHeight;

	//The terrain height function.
	TerrainHeightFunction _TerrainHeightFunction;

	//The terrain material function.
	TerrainMaterialFunction _TerrainMaterialFunction;

	//The terrain data save folder.
	DynamicString _TerrainDataSaveFolder;

	//The buffer.
	BufferHandle _Buffer;

	//The index offset.
	uint64 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

};