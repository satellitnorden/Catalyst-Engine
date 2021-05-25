//Header file.
#include <Systems/NetworkSystem.h>

//Singleton definition.
DEFINE_SINGLETON(NetworkSystem);

/*
*	Initializes the network system.
*/
void NetworkSystem::Initialize() NOEXCEPT
{
	//Initialize the subsystem.
	SubInitialize();
}