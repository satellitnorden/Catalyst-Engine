#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/PointLightComponent.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>


class PointLightInitializationData final : public EntityInitializationData
{

public:

	//The component.
	PointLightComponent _Component;

};