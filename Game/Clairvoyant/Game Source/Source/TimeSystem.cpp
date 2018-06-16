//Header file.
#include <TimeSystem.h>

//Entities.
#include <Entities/DirectionalLightEntity.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/CatalystMath.h>

//Systems.
#include <Systems/EntitySystem.h>

//Singleton definition.
DEFINE_SINGLETON(TimeSystem);

//Time system constants.
namespace TimeSystemConstants
{
	constexpr float INVERSE_MIDDAY{ 1.0f / 12.0f };
	constexpr float STARTING_TIME{ 5.0f };
	constexpr float SUN_MINIMUM_INTENSITY{ 0.0f };
	constexpr float SUN_MAXIMUM_INTENSITY{ 100.0f };
	constexpr float TIME_MULTIPLIER{ 250.0f };
}

/*
*	Initializes the time system.
*/
void TimeSystem::Initialize() NOEXCEPT
{
	//Create the sun!
	sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	sun->Rotate(Vector3(90.0f, 45.0f, 0.0f));

	//Update the time system once to set it to it's starting time.
	Update(TimeSystemConstants::STARTING_TIME * 60.0f * 60.0f / TimeSystemConstants::TIME_MULTIPLIER);
}

/*
*	Updates the time system.
*/
void TimeSystem::Update(const float deltaTime) NOEXCEPT
{
	//Convert the delta time from seconds, to minutes and then to hours and increment the current time.
	const float timeIncrease = ((deltaTime / 60.0f) / 60.0f) * TimeSystemConstants::TIME_MULTIPLIER;

	//Add it to the current time.
	currentTime += timeIncrease;

	//If the the clock goes over 24.0f, that means the clock has wrapped around to midnight.
	while (currentTime >= 24.0f)
	{
		currentTime -= 24.0f;
	}

	//Determine the day alpha to be able to interpolate between day and night effects. 0.0f means it's night and 1.0f means it's day.
	float dayAlpha;

	//Determine of we are heading towards midday or midnight.
	if (currentTime < 12.0f)
	{
		//We are heading towards midday.
		dayAlpha = currentTime * TimeSystemConstants::INVERSE_MIDDAY;
	}

	else
	{
		//We are heading towards midnight.
		dayAlpha = 1.0f - ((currentTime - 12.0f) * TimeSystemConstants::INVERSE_MIDDAY);
	}

	dayAlpha = CatalystMath::SmoothStep<4>(dayAlpha);

	//Continously rotate the sun.
	float sunAlpha;

	if (currentTime < 5.0f || currentTime > 19.0f)
	{
		sunAlpha = 0.0f;
	}

	else
	{
		if (currentTime < 12.0f)
		{
			sunAlpha = (currentTime - 5.0f) / 7.0f;
		}

		else
		{
			sunAlpha = 1.0f - (currentTime - 12.0f) / 7.0f;
		}	
	}

	sunAlpha = CatalystMath::SmoothStep<4>(sunAlpha);

	sun->Rotate(Vector3(15.0f * timeIncrease, 0.0f, 0.0f));

	//Smoothly interpolate the intensity of the sun.
	sun->SetIntensity(CatalystMath::LinearlyInterpolate(TimeSystemConstants::SUN_MINIMUM_INTENSITY, TimeSystemConstants::SUN_MAXIMUM_INTENSITY, sunAlpha));

	//Set the environment blend.
	EnvironmentManager::Instance->SetEnvironmentBlend(dayAlpha);
}