#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Forward declarations.
class TerrainEntity;

class WorldChunk final
{

public:

	//The X grid position.
	int32 gridPositionX{ INT32_MAXIMUM };

	//The Y grid position.
	int32 gridPositionY{ INT32_MAXIMUM };

	//The terrain entity.
	TerrainEntity *RESTRICT terrainEntity{ nullptr };

	/*
	*	Returns whether or not this world chunk is valid.
	*/
	bool IsValid() const NOEXCEPT { return gridPositionX != INT32_MAXIMUM && gridPositionY != INT32_MAXIMUM; }

};