//Header file.
#include <World/Core/SkySystem.h>

//Resources.
#include <Resources/Data/TextureCubeData.h>

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
NO_DISCARD SkyGradient SkySystem::GetCurrentSkyGradient() NOEXCEPT
{
	//Define constants.
	constexpr uint8 SKY_GRADIENT_LOOKUP_SIZE{ 4 };
	constexpr StaticArray<SkyGradient, SKY_GRADIENT_LOOKUP_SIZE> SKY_GRADIENT_LOOKUP
	{
		SkyGradient(Vector3<float>(0.25f, 0.5f, 1.0f) * 0.125f * 0.25f, Vector3<float>(0.25f, 0.5f, 1.0f) * 0.125f * 0.125f),
		SkyGradient(Vector3<float>(0.25f, 0.5f, 1.0f) * 0.5f, Vector3<float>(1.0f, 0.5f, 0.25f) * 0.5f),
		SkyGradient(Vector3<float>(0.25f, 0.5f, 1.0f), Vector3<float>(0.25f, 0.5f, 1.0f)),
		SkyGradient(Vector3<float>(0.25f, 0.5f, 1.0f) * 0.5f, Vector3<float>(1.0f, 0.25f, 0.125f) * 0.5f)
	};

	//Retrieve the current time of day.
	const float current_time_of_day{ WorldSystem::Instance->GetTimeOfDaySystem()->GetCurrentTimeOfDay() };

	//Calculate the indices.
	const uint32 first_index{ static_cast<uint32>(current_time_of_day / 24.0f * static_cast<float>(SKY_GRADIENT_LOOKUP_SIZE - 1)) };
	const uint32 second_index{ first_index + 1 };

	//Calculate the alpha.
	const float alpha{ CatalystBaseMath::Fractional(current_time_of_day / 24.0f * static_cast<float>(SKY_GRADIENT_LOOKUP_SIZE - 1)) };

	//Return the blended sky gradient.
	return CatalystBaseMath::LinearlyInterpolate(SKY_GRADIENT_LOOKUP[first_index], SKY_GRADIENT_LOOKUP[second_index], alpha);
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
			data._Data.UpsizeFast(resolution * resolution * 4 * 6);

			Memory::Set(data._Data.Data(), 0, sizeof(float) * resolution * resolution * 4 * 6);

			RenderingSystem::Instance->CreateTextureCube(data, &_SkyTextures[i]);
		}

		_SkyTexturesInitialized = true;
	}
}