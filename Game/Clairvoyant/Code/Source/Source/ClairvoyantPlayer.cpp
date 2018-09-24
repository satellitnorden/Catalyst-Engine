//Header file.
#include <Player/ClairvoyantPlayer.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ClairvoyantPlayer);

/*
*	Initializes the Clairvoyant player.
*/
void ClairvoyantPlayer::Initialize() NOEXCEPT
{
	//Create the camera and set it as the active one..
	_Camera = EntitySystem::Instance->CreateEntity<CameraEntity>();
	RenderingSystem::Instance->SetActiveCamera(_Camera);
}