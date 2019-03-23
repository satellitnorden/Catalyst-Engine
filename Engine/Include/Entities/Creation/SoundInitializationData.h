#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Math.
#include <Math/General/Vector3.h>

//Sound.
#include <Sound/Native/SoundCore.h>

class SoundInitializationData final : public EntityInitializationData
{

public:

	//The position.
	Vector3<float> _Position;

	//The description.
	SoundDescriptionHandle _Description;

};