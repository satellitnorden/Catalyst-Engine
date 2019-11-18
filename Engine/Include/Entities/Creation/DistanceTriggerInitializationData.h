#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>
#include <Entities/Types/DistanceTriggerEntity.h>

//Math.
#include <Math/General/Vector.h>

class DistanceTriggerInitializationData final : public EntityInitializationData
{

public:

	//The position.
	Vector3<float> _Position;

	//The trigger distance.
	float _TriggerDistance;

	//The enter function.
	DistanceTriggerEntity::EnterFunction _EnterFunction;

	//The exit function.
	DistanceTriggerEntity::ExitFunction _ExitFunction;

};