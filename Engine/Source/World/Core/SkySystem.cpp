//Header file.
#include <World/Core/SkySystem.h>

//Resources.
#include <Resources/Loading/TextureCubeData.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Post-initializes the sky system.
*/
void SkySystem::PostInitialize() NOEXCEPT
{
	//Retrieve the default sky texture.
	if (!_SkyTexture)
	{
		_SkyTexture = ResourceSystem::Instance->GetTextureCubeResource(HashString("Catalyst_Engine_Default_TextureCube"));
	}
}