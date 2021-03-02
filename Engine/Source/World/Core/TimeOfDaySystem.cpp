//Header file.
#include <World/Core/TimeOfDaySystem.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Core/BinaryFile.h>

//Entities.
#include <Entities/Creation/LightInitializationData.h>

//Math.
#include <Math/Core/CatalystCoordinateSpaces.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/WorldSystem.h>

//Time of dat system constants.
namespace TimeOfDaySystemConstants
{
	constexpr uint8 SKY_GRADIENT_LOOKUP_SIZE{ 8 };
	constexpr StaticArray<SkyGradient, SKY_GRADIENT_LOOKUP_SIZE> SKY_GRADIENT_LOOKUP
	{
		//00.00.
		SkyGradient(Vector3<float32>(0.0f, 0.25f, 1.0f) * 0.125f * 0.125f * 0.5f, Vector3<float32>(0.0f, 0.25f, 1.0f) * 0.125f * 0.125f * 0.25f),

		//03.00.
		SkyGradient(Vector3<float32>(0.0f, 0.5f, 1.0f) * 0.125f * 0.25f, Vector3<float32>(1.0f, 1.0f, 0.75f) * 0.125f * 0.25f),

		//06.00.
		SkyGradient(Vector3<float32>(0.0f, 0.5f, 1.0f) * 0.25f, Vector3<float32>(1.0f, 0.5f, 0.25f) * 0.25f),

		//09.00.
		SkyGradient(Vector3<float32>(0.0f, 0.75f, 1.0f) * 0.5f, Vector3<float32>(1.0f, 0.75f, 0.5f) * 0.5f),

		//12.00.
		SkyGradient(Vector3<float32>(0.0f, 0.5f, 1.0f), Vector3<float32>(1.0f, 1.0f, 1.0f)),

		//15.00.
		SkyGradient(Vector3<float32>(0.0f, 0.5f, 1.0f), Vector3<float32>(1.0f, 0.75f, 0.5f)),

		//18.00.
		SkyGradient(Vector3<float32>(0.0f, 0.25f, 1.0f), Vector3<float32>(1.0f, 0.25f, 0.0625f)),

		//21.00.
		SkyGradient(Vector3<float32>(0.0f, 0.25f, 1.0f) * 0.125f * 0.5f, Vector3<float32>(0.25f, 0.5f, 0.75f) * 0.125f * 0.25f)
	};
}

/*
*	Terminates the time of day system.
*/
void TimeOfDaySystem::Terminate() NOEXCEPT
{
	//If the time of day system was enabled, save save data.
	if (_Enabled)
	{
		DynamicString time_of_day_save_data_file_path{ _TimeOfDayParameters._SaveFolder };
		time_of_day_save_data_file_path += "\\TimeOfDaySaveData.save";

		BinaryFile<IOMode::Out> time_of_day_save_data_file{ time_of_day_save_data_file_path.Data() };

		time_of_day_save_data_file.Write(&_CurrentTimeOfDay, sizeof(float32));

		time_of_day_save_data_file.Close();
	}
}

/*
*	Enables the time of day system.
*/
void TimeOfDaySystem::Enable(const TimeOfDayParameters& time_of_day_parameters) NOEXCEPT
{
	ASSERT(!_Enabled, "Enabling the time of day system twice!");

	//Try to load the time of day data, if it exists.
	bool loaded_save_data{ false };

	if (time_of_day_parameters._SaveFolder.Length() > 0)
	{
		DynamicString time_of_day_save_data_file_path{ time_of_day_parameters._SaveFolder };
		time_of_day_save_data_file_path += "\\TimeOfDaySaveData.save";

		if (File::Exists(time_of_day_save_data_file_path.Data()))
		{
			BinaryFile<IOMode::In> time_of_day_save_data_file{ time_of_day_save_data_file_path.Data() };

			time_of_day_save_data_file.Read(&_CurrentTimeOfDay, sizeof(float32));

			time_of_day_save_data_file.Close();

			loaded_save_data = true;
		}
	}

	//Set initial data, if save data wasn't loaded.
	if (!loaded_save_data)
	{
		_CurrentTimeOfDay = time_of_day_parameters._InitialTimeOfDay;
	}

	//Set the time of day parameters.
	_TimeOfDayParameters = time_of_day_parameters;

	//Create the sky light.
	{
		_SkyLight = EntitySystem::Instance->CreateEntity<LightEntity>();
		LightInitializationData* const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<LightInitializationData>() };

		data->_Properties = EntityInitializationData::Property::NONE;
		data->_Direction = CatalystWorldCoordinateSpace::DOWN;
		data->_Color = VectorConstants::ONE;
		data->_LightType = LightType::DIRECTIONAL;
		data->_LightProperties = CatalystShaderConstants::LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT | CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_BIT | CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT;
		data->_Intensity = 1.0f;
		data->_Radius = 0.0f;
		data->_Size = 0.0f;

		EntitySystem::Instance->RequestInitialization(_SkyLight, data, false);
	}

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<TimeOfDaySystem *const RESTRICT>(arguments)->PreUpdate();
	},
	this,
	UpdatePhase::PRE,
	UpdatePhase::RENDER,
	false);

	//The time of day system is now enabled!
	_Enabled = true;
}

/*
*	Updates the time of day system during the pre update phase.
*/
void TimeOfDaySystem::PreUpdate() NOEXCEPT
{
	ASSERT(_Enabled, "The time of day system should not be updated if it's not enabled");

	//Cache the delta time.
	const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

	//Update the current time of day.
	_CurrentTimeOfDay += delta_time / 60.0f / 60.0f * _TimeOfDayParameters._TimeMultiplier;

	//Wrap around.
	while (_CurrentTimeOfDay >= 24.0f)
	{
		_CurrentTimeOfDay -= 24.0f;
	}

	//Update the sky light.
	UpdateSkyLight();

	//Update the sky.
	UpdateSky();
}

/*
*	Updates the sky light.
*/
void TimeOfDaySystem::UpdateSkyLight() NOEXCEPT
{
	//Define constants.
	constexpr float32 NIGHT_SKY_INTENSITY{ 2.5f };
	constexpr float32 DAY_SKY_INTENSITY{ 10.0f };
	constexpr Vector3<float32> NIGHT_SKY_LUMINANCE{ 0.8f, 0.9f, 1.0f };
	constexpr Vector3<float32> DAY_SKY_LUMINANCE{ 1.0f, 0.9f, 0.8f };

	//Need that sky light.
	if (!_SkyLight->_Initialized)
	{
		return;
	}

	//Calculate the time of day alpha.
	float32 time_of_day_alpha;

	if (_CurrentTimeOfDay >= 18.0f)
	{
		time_of_day_alpha = (_CurrentTimeOfDay - 18.0f) / 12.0f;
	}

	else if (_CurrentTimeOfDay < 6.0f)
	{
		time_of_day_alpha = (_CurrentTimeOfDay + 6.0f) / 12.0f;
	}

	else
	{
		time_of_day_alpha = (_CurrentTimeOfDay - 6.0f) / 12.0f;
	}

	//Calculate the sky light direction.
	Vector3<float32> sky_light_direction;

	if (_CurrentTimeOfDay >= 18.0f || _CurrentTimeOfDay < 6.0f)
	{
		Vector3<float32> rotation;

		rotation._X = CatalystBaseMath::LinearlyInterpolate(CatalystBaseMath::DegreesToRadians(-100.0f), CatalystBaseMath::DegreesToRadians(100.0f), time_of_day_alpha);
		rotation._Y = 0.0f;
		rotation._Z = CatalystBaseMath::DegreesToRadians(22.5f);

		sky_light_direction = Vector3<float32>::Normalize(CatalystWorldCoordinateSpace::DOWN.Rotated(rotation));
	}
	
	else
	{
		Vector3<float32> rotation;

		rotation._X = CatalystBaseMath::DegreesToRadians(22.5f);
		rotation._Y = 0.0f;
		rotation._Z = CatalystBaseMath::LinearlyInterpolate(CatalystBaseMath::DegreesToRadians(-100.0f), CatalystBaseMath::DegreesToRadians(100.0f), time_of_day_alpha);

		sky_light_direction = Vector3<float32>::Normalize(CatalystWorldCoordinateSpace::DOWN.Rotated(rotation));
	}

	//Set the sky light direction.
	_SkyLight->SetDirection(sky_light_direction);

	//Update the sky light luminance.
	{
		//Calculate the sky luminance alpha.
		float32 sky_luminance_alpha;

		if (_CurrentTimeOfDay < 6.0f)
		{
			sky_luminance_alpha = CatalystBaseMath::SmoothStep<1>(1.0f - (_CurrentTimeOfDay / 6.0f));
		}

		else if (_CurrentTimeOfDay < 12.0f)
		{
			sky_luminance_alpha = CatalystBaseMath::SmoothStep<1>((_CurrentTimeOfDay - 6.0f) / 6.0f);
		}

		else if (_CurrentTimeOfDay < 18.0f)
		{
			sky_luminance_alpha = CatalystBaseMath::SmoothStep<1>(1.0f - ((_CurrentTimeOfDay - 12.0f) / 6.0f));
		}

		else
		{
			sky_luminance_alpha = CatalystBaseMath::SmoothStep<1>((_CurrentTimeOfDay - 18.0f) / 6.0f);
		}

		if (_CurrentTimeOfDay >= 18.0f || _CurrentTimeOfDay < 6.0f)
		{
			_SkyLight->SetColor(NIGHT_SKY_LUMINANCE);
			_SkyLight->SetIntensity(NIGHT_SKY_INTENSITY * sky_luminance_alpha);
		}

		else
		{
			_SkyLight->SetColor(DAY_SKY_LUMINANCE);
			_SkyLight->SetIntensity(DAY_SKY_INTENSITY * sky_luminance_alpha);
		}
	}
}

/*
*	Updates the sky.
*/
void TimeOfDaySystem::UpdateSky() NOEXCEPT
{
	//If the time of day system shouldn't control the sky, just quit already.
	if (!_TimeOfDayParameters._ControlSky)
	{
		return;
	}

	//Calculate the indices and the alpha.
	uint8 first_index;
	uint8 second_index;
	float32 alpha;

	{
		first_index = static_cast<uint8>((_CurrentTimeOfDay / 24.0f) * static_cast<float32>(TimeOfDaySystemConstants::SKY_GRADIENT_LOOKUP_SIZE));
		second_index = first_index == TimeOfDaySystemConstants::SKY_GRADIENT_LOOKUP_SIZE - 1 ? 0 : first_index + 1;
		alpha = CatalystBaseMath::Fractional((_CurrentTimeOfDay / 24.0f) * static_cast<float32>(TimeOfDaySystemConstants::SKY_GRADIENT_LOOKUP_SIZE));
	}

	WorldSystem::Instance->GetSkySystem()->SetSkyGradient(CatalystBaseMath::LinearlyInterpolate(TimeOfDaySystemConstants::SKY_GRADIENT_LOOKUP[first_index], TimeOfDaySystemConstants::SKY_GRADIENT_LOOKUP[second_index], alpha));
}