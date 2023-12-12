#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class TerrainVertex final
{

public:

	//The position.
	Vector2<float32> _Position;

	//The borders.
	uint32 _Borders;

};