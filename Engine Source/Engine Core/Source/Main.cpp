/*
//Engine core.
#include <EngineCore.h>

//Systems.
#include <EngineSystem.h>

int main()
{
	//Initialize the engine system.
	EngineSystem::Instance->InitializeSystem();

	//Update the engine system while it is not indicated that it should terminate.
	while (!EngineSystem::Instance->UpdateSystemSynchronous());

	//Release the engine system.
	EngineSystem::Instance->ReleaseSystem();

	//Return success!
	return 0;
}
*/

#include <EngineCore.h>
#include <MicroBenchmarker.h>
#include <SharedPointer.h>
#include <memory>

void Function1()
{
	
}

void Function2()
{
	
}

int main()
{
	MicroBenchmarker::StartBenchmark(100'000'000, &Function1, &Function2);

	return 0;
}