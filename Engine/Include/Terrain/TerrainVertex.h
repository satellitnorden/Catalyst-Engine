#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class TerrainVertex final
{

public:

	//The position.
	Vector2<float> _Position;

	//The borders.
	int32 _Borders;

};