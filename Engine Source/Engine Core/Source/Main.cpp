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

//Engine core.
#include <EngineCore.h>

//Math.
#include <GameMath.h>
#include <Vector3.h>

//Tools.
#include <MicroBenchmarker.h>

void Function1()
{
	Vector3 vector;

	vector.NewRotate(Vector3(GameMath::RandomFloatInRange(0.0f, 360.0f), GameMath::RandomFloatInRange(0.0f, 360.0f), GameMath::RandomFloatInRange(0.0f, 360.0f)));
}

void Function2()
{
	Vector3 vector;

	vector.Rotate(Vector3(GameMath::RandomFloatInRange(0.0f, 360.0f), GameMath::RandomFloatInRange(0.0f, 360.0f), GameMath::RandomFloatInRange(0.0f, 360.0f)));
}

int main()
{
	MicroBenchmarker::StartBenchmark(100'000'000, &Function1, &Function2);

	std::cin.ignore();

	return 0;
}