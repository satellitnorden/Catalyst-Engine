
//Engine core.
#include <EngineCore.h>

//Systems.
#include <EngineSystem.h>

//Preprocessor defines.
#define EXIT_SUCCESS 0

int main() NOEXCEPT
{
	//Initialize the engine system.
	EngineSystem::Instance->InitializeSystem();

	//Update the engine system while it is not indicated that it should terminate.
	while (!EngineSystem::Instance->UpdateSystemSynchronous());

	//Release the engine system.
	EngineSystem::Instance->ReleaseSystem();

	//Return success!
	return EXIT_SUCCESS;
}