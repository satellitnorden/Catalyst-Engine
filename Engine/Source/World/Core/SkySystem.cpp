//Header file.
#include <World/Core/SkySystem.h>

//Resources.
#include <Resources/Data/TextureCubeData.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Post-initializes the sky system.
*/
void SkySystem::PostInitialize() NOEXCEPT
{
	//Initialize the sky textures.
	InitializeSkyTextures();
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