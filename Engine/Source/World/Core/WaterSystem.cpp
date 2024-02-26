//Header file.
#include <World/Core/WaterSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Initializes the water system.
*/
void WaterSystem::Initialize() NOEXCEPT
{
	//Reset the water uniform data.
	Memory::Set(&_WaterUniformData, 0, sizeof(WaterUniformData));

	//Register the uniform data.
	RenderingSystem::Instance->GetBufferManager()->RegisterUniformBuffer
	(
		HashString("Water"),
		&_WaterUniformData,
		sizeof(WaterUniformData)
	);
}

/*
*	Updates the water system during render.
*/
void WaterSystem::RenderUpdate() NOEXCEPT
{
	//Define constants.
	constexpr float32 SCALE{ 0.125f * 0.5f };

	//Cache the delta time.
	const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

	//Cache the wind direction.
	Vector2<float32> wind_direction{ WorldSystem::Instance->GetWindSystem()->GetWindDirection()._X , WorldSystem::Instance->GetWindSystem()->GetWindDirection()._Z };
	wind_direction.Rotate(CatalystBaseMathConstants::PI);

	//Update the water uniform data.
	_WaterUniformData._Offsets[0] += wind_direction * delta_time * CatalystBaseMathConstants::QUARTER_PI * SCALE;
	_WaterUniformData._Offsets[1] += wind_direction * delta_time * CatalystBaseMathConstants::PHI * SCALE;
	_WaterUniformData._Offsets[2] += wind_direction * delta_time * CatalystBaseMathConstants::EULERS_NUMBER * SCALE;
	_WaterUniformData._Offsets[3] += wind_direction * delta_time * (CatalystBaseMathConstants::PI + CatalystBaseMathConstants::HALF_PI) * SCALE;
}