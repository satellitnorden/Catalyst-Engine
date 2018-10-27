//Header file.
#include <World/TimeOfDaySystem.h>

//Systems.
#include <Systems/LightingSystem.h>
#include <Systems/UpdateSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TimeOfDaySystem);

//Time of day system constants.
namespace TimeOfDaySystemConstants
{
	constexpr float STARTING_TIME{ 6.0f };
	constexpr float TIME_MULTIPLIER{ 250.0f };
}

/*
*	Initializes the time of day system.
*/
void TimeOfDaySystem::Initialize() NOEXCEPT
{
	//Register the time of day system for updates.
	UpdateSystem::Instance->RegisterAsynchronousOpeningUpdate(this);

	//Update this once to the starting time.
	UpdateContext context;
	context._DeltaTime = ((TimeOfDaySystemConstants::STARTING_TIME * 60.0f) * 60.0f) / TimeOfDaySystemConstants::TIME_MULTIPLIER;
	OpeningUpdateAsynchronous(&context);
}

/*
*	Updates the time of day system asynchronously during the opening update phase.
*/
bool TimeOfDaySystem::OpeningUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Convert the delta time from seconds, to minutes and then to hours and increment the current time.
	_CurrentTime += ((context->_DeltaTime / 60.0f) / 60.0f) * TimeOfDaySystemConstants::TIME_MULTIPLIER;

	//If the the clock goes over 24.0f, that means the clock has wrapped around to midnight.
	while (_CurrentTime > 24.0f)
	{
		_CurrentTime -= 24.0f;
	}

	//Rotate the sun.
	LightingSystem::Instance->GetDirectionalLight()->SetRotation(Vector3(90.0f + _CurrentTime * 15.0f, 45.0f, 0.0f));

	//Return that the time of day system wants to continue being updated.
	return true;
}