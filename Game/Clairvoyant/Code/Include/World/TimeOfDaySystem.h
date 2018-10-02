#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/Updateable.h>
#include <Core/Pointers/UniquePointer.h>

//Entities.
#include <Entities/DirectionalLightEntity.h>

class TimeOfDaySystem final : public Updateable
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TimeOfDaySystem);

	/*
	*	Default constructor.
	*/
	TimeOfDaySystem() NOEXCEPT { }

	/*
	*	Initializes the time of day system
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the time of day system asynchronously during the logic update phase.
	*/
	bool LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT final override;

private:

	//The sun.
	DirectionalLightEntity *RESTRICT _Sun;

	//The current time.
	float _CurrentTime{ 0.0f };

};