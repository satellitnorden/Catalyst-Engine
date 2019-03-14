#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Vector3.h>

//Sound.
#include <Sound/Native/SoundCore.h>

class SoundComponent final
{

public:

	//The position.
	Vector3<float> _Position;

	//The sound instance handle.
	SoundInstanceHandle _Instance;

};