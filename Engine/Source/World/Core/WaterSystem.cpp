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
	wind_direction.Rotate(BaseMathConstants::PI);

	//Calculate the offsets.
	Vector2<float32> offsets[4];

	offsets[0] = wind_direction * delta_time * BaseMathConstants::QUARTER_PI * SCALE;
	offsets[1] = wind_direction * delta_time * BaseMathConstants::PHI * SCALE;
	offsets[2] = wind_direction * delta_time * BaseMathConstants::EULERS_NUMBER * SCALE;
	offsets[3] = wind_direction * delta_time * (BaseMathConstants::PI + BaseMathConstants::HALF_PI) * SCALE;

	//Update the water uniform data.
	for (uint8 offset_index{ 0 }; offset_index < 4; ++offset_index)
	{
		_WaterUniformData._Offsets[offset_index]._X += offsets[offset_index]._X;
		_WaterUniformData._Offsets[offset_index]._Y += offsets[offset_index]._Y;
	}
}