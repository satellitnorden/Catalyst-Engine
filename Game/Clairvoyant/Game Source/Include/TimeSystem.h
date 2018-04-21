#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Forward declarations.
class DirectionalLightEntity;

class TimeSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TimeSystem);

	/*
	*	Initializes the time system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the time system.
	*/
	void Update(const float deltaTime) NOEXCEPT;

private:

	//The current time.
	float currentTime{ 0.0f };

	//The current time multiplier.
	float currentTimeMultiplier{ 500.0f };

	//The sun.
	DirectionalLightEntity *RESTRICT sun;

};