#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//World.
#include <World/Core/WorldPosition.h>

class TerrainInitializationData final : public EntityInitializationData
{

public:

	//The world position.
	WorldPosition _WorldPosition;

	//The height map.
	Texture2D<float32> _HeightMap;

	//The normal map.
	Texture2D<Vector3<float32>> _NormalMap;

};