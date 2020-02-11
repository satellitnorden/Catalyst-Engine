//Header file.
#include <World/SkySystem.h>

//Resources.
#include <Resources/Data/TextureCubeData.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Sky system constants.
namespace SkySystemConstants
{
	constexpr uint32 SKY_TEXTURE_RESOLUTION{ 32 };
}

/*
*	Post-initializes the sky system.
*/
void SkySystem::PostInitialize() NOEXCEPT
{
	//Create the sky texture.
	TextureCubeData data;

	data._Resolution = SkySystemConstants::SKY_TEXTURE_RESOLUTION;
	data._Data.UpsizeFast(SkySystemConstants::SKY_TEXTURE_RESOLUTION * SkySystemConstants::SKY_TEXTURE_RESOLUTION * 6);

	Memory::Set(data._Data.Data(), 0, sizeof(float) * SkySystemConstants::SKY_TEXTURE_RESOLUTION * SkySystemConstants::SKY_TEXTURE_RESOLUTION * 6);

	RenderingSystem::Instance->CreateTextureCube(data, &_SkyTexture);
}