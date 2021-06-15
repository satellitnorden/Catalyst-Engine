#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//World.
#include <World/Core/WorldTransform.h>

class UserInterfaceInitializationData final : public EntityInitializationData
{

public:

	//The user interface scene identifier.
	HashString _UserInterfaceSceneIdentifier;

	//The initial world transform.
	WorldTransform _InitialWorldTransform;

};