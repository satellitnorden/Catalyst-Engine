//Header file.
#include <World/Core/SkySystem.h>

//Resources.
#include <Resources/Loading/TextureCubeData.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Post-initializes the sky system.
*/
void SkySystem::PostInitialize() NOEXCEPT
{
	//Initialize the sky textures.
	InitializeSkyTextures();
}

/*
*	Returns the current sky gradient.
*/
NO_DISCARD SkyGradient SkySystem::GetCurrentSkyGradient() const NOEXCEPT
{
	//Define constants.
	constexpr uint8 SKY_GRADIENT_LOOKUP_SIZE{ 8 };
	constexpr StaticArray<SkyGradient, SKY_GRADIENT_LOOKUP_SIZE> SKY_GRADIENT_LOOKUP
	{
		//00.00.
		SkyGradient(Vector3<float>(0.0f, 0.25f, 1.0f) * 0.125f * 0.125f * 0.5f, Vector3<float>(0.0f, 0.25f, 1.0f) * 0.125f * 0.125f * 0.25f),

		//03.00.
		SkyGradient(Vector3<float>(0.0f, 0.5f, 1.0f) * 0.125f * 0.25f, Vector3<float>(1.0f, 1.0f, 0.75f) * 0.125f * 0.25f),

		//06.00.
		SkyGradient(Vector3<float>(0.0f, 0.5f, 1.0f) * 0.25f, Vector3<float>(1.0f, 0.5f, 0.25f) * 0.25f),

		//09.00.
		SkyGradient(Vector3<float>(0.0f, 0.75f, 1.0f) * 0.5f, Vector3<float>(1.0f, 0.75f, 0.5f) * 0.5f),

		//12.00.
		SkyGradient(Vector3<float>(0.0f, 0.5f, 1.0f), Vector3<float>(1.0f, 1.0f, 1.0f)),

		//15.00.
		SkyGradient(Vector3<float>(0.0f, 0.5f, 1.0f), Vector3<float>(1.0f, 0.75f, 0.5f)),

		//18.00.
		SkyGradient(Vector3<float>(0.0f, 0.25f, 1.0f), Vector3<float>(1.0f, 0.25f, 0.0625f)),

		//21.00.
		SkyGradient(Vector3<float>(0.0f, 0.25f, 1.0f) * 0.125f * 0.5f, Vector3<float>(0.25f, 0.5f, 0.75f) * 0.125f * 0.25f)
	};

	//Check if there is a sky gradient override.
	if (_SkyGradientOverride)
	{
		return _SkyGradientOverride;
	}

	else
	{
		//Retrieve the current time of day.
		const float current_time_of_day{ WorldSystem::Instance->GetTimeOfDaySystem()->GetCurrentTimeOfDay() };

		//Calculate the time of day alpha.
		const float time_of_day_alpha{ current_time_of_day / 24.0f * static_cast<float>(SKY_GRADIENT_LOOKUP_SIZE) };

		//Calculate the indices.
		const uint32 first_index{ static_cast<uint32>(time_of_day_alpha) };
		const uint32 second_index{ first_index < SKY_GRADIENT_LOOKUP_SIZE - 1 ? first_index + 1 : 0 };

		//Calculate the alpha.
		const float alpha{ CatalystBaseMath::Fractional(time_of_day_alpha) };

		//Return the blended sky gradient.
		return CatalystBaseMath::LinearlyInterpolate(SKY_GRADIENT_LOOKUP[first_index], SKY_GRADIENT_LOOKUP[second_index], alpha);
	}
}

/*
*	Returns the current star intensity.
*/
NO_DISCARD float SkySystem::GetCurrentStarIntensity() const NOEXCEPT
{
	//Define constants.
	constexpr uint8 STAR_INTENSITY_LOOKUP_SIZE{ 8 };
	constexpr StaticArray<float, STAR_INTENSITY_LOOKUP_SIZE> STAR_STRENGTH_LOOKUP
	{
		1.0f, //00.00
		0.5f, //03.00
		0.0f, //06.00
		0.0f, //09.00
		0.0f, //12.00
		0.0f, //15.00
		0.0f, //18.00
		0.5f  //32.00
	};

	//Check if there is a star strength override.
	if (_StarStrengthOverride)
	{
		return _StarStrengthOverride;
	}

	else
	{
		//Retrieve the current time of day.
		const float current_time_of_day{ WorldSystem::Instance->GetTimeOfDaySystem()->GetCurrentTimeOfDay() };

		//Calculate the time of day alpha.
		const float time_of_day_alpha{ current_time_of_day / 24.0f * static_cast<float>(STAR_INTENSITY_LOOKUP_SIZE) };

		//Calculate the indices.
		const uint32 first_index{ static_cast<uint32>(time_of_day_alpha) };
		const uint32 second_index{ first_index < STAR_INTENSITY_LOOKUP_SIZE - 1 ? first_index + 1 : 0 };

		//Calculate the alpha.
		const float alpha{ CatalystBaseMath::Fractional(time_of_day_alpha) };

		//Return the blended star intensity.
		return CatalystBaseMath::LinearlyInterpolate(STAR_STRENGTH_LOOKUP[first_index], STAR_STRENGTH_LOOKUP[second_index], alpha);
	}
}

/*
*	Initializes the sky textures.
*/
void SkySystem::InitializeSkyTextures() NOEXCEPT
{
	if (!_SkyTexturesInitialized)
	{
		for (uint32 i{ 0 }; i < CatalystShaderConstants::NUMBER_OF_SKY_TEXTURES; ++i)
		{
			//Calculate the resolution.
			const uint32 resolution{ CatalystShaderConstants::SKY_TEXTURE_BASE_RESOLUTION >> i };

			//Create the sky texture.
			TextureCubeData data;

			data._Resolution = resolution;
			data._Data.Upsize<false>(resolution * resolution * 4 * 6);

			Memory::Set(data._Data.Data(), 0, sizeof(float) * resolution * resolution * 4 * 6);

			RenderingSystem::Instance->CreateTextureCube(data, &_SkyTextures[i]);
		}

		_SkyTexturesInitialized = true;
	}
}