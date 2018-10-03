//Header file.
#include <World/TimeOfDaySystem.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/UpdateSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TimeOfDaySystem);

//Time of day system constants.
namespace TimeOfDaySystemConstants
{
	constexpr float MAXIMUM_SUN_INTENSITY{ 25.0f };
	constexpr float STARTING_TIME{ 6.0f };
	constexpr float TIME_MULTIPLIER{ 100.0f };
}

/*
*	Initializes the time of day system.
*/
void TimeOfDaySystem::Initialize() NOEXCEPT
{
	//Create the sun.
	_Sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();

	//Register the time of day system for updates.
	UpdateSystem::Instance->RegisterAsynchronousLogicUpdate(this);

	//Update this once to the starting time.
	UpdateContext context;
	context._DeltaTime = ((TimeOfDaySystemConstants::STARTING_TIME * 60.0f) * 60.0f) / TimeOfDaySystemConstants::TIME_MULTIPLIER;
	LogicUpdateAsynchronous(&context);
}

/*
*	Updates the time of day system asynchronously during the logic update phase.
*/
bool TimeOfDaySystem::LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Convert the delta time from seconds, to minutes and then to hours and increment the current time.
	_CurrentTime += ((context->_DeltaTime / 60.0f) / 60.0f) * TimeOfDaySystemConstants::TIME_MULTIPLIER;

	//If the the clock goes over 24.0f, that means the clock has wrapped around to midnight.
	while (_CurrentTime > 24.0f)
	{
		_CurrentTime -= 24.0f;
	}

	//Determine the environment blend to be able to interpolate between day and night effects. 0.0f means it's night and 1.0f means it's day.
	float environmentBlend;

	//Determine of the time is heading towards midday or midnight.
	if (_CurrentTime < 12.0f)
	{
		//We are heading towards midday.
		environmentBlend = _CurrentTime / 12.0f;
	}

	else
	{
		//We are heading towards midnight.
		environmentBlend = 1.0f - ((_CurrentTime - 12.0f) / 12.0f);
	}

	//Smooth out the blend a biot.
	environmentBlend = CatalystBaseMath::SmoothStep<1>(environmentBlend);

	//Set the environment blend.
	EnvironmentManager::Instance->SetEnvironmentBlend(environmentBlend);

	//Continously rotate the sun.
	float sunBlend;

	if (_CurrentTime < 6.0f || _CurrentTime > 18.0f)
	{
		sunBlend = 0.0f;
	}
		

	else
	{
		if (_CurrentTime < 12.0f)
		{
			sunBlend = (_CurrentTime - 6.0f) / 6.0f;
		}

		else
		{
			sunBlend = 1.0f - (_CurrentTime - 12.0f) / 6.0f;
		}
	}

	//Smooth out the sun blend a bit.
	sunBlend = CatalystBaseMath::SmoothStep<1>(sunBlend);

	//Rotate the sun.
	_Sun->SetRotation(Vector3(90.0f + _CurrentTime * 15.0f, 45.0f, 0.0f));

	//Smoothly interpolate the intensity of the sun.
	_Sun->SetIntensity(CatalystBaseMath::LinearlyInterpolate(0.0f, TimeOfDaySystemConstants::MAXIMUM_SUN_INTENSITY, sunBlend));

	//Return that the time of day system wants to continue being updated.
	return true;
}