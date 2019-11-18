#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/DistanceTriggerEntity.h>

//Math.
#include <Math/General/Vector.h>

class DistanceTriggerComponent final
{

public:

	//The position.
	Vector3<float> _Position;

	//The trigger distance.
	float _TriggerDistance;

	//The trigger distance, squared.
	float _TriggerDistanceSquared;

	//The enter function.
	DistanceTriggerEntity::EnterFunction _EnterFunction;

	//The exit function.
	DistanceTriggerEntity::ExitFunction _ExitFunction;

	//The current state.
	DistanceTriggerEntity::State _CurrentState;

};