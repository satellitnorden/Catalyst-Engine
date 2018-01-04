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

#define MAP_ITERATIONS 1'000

void Function1()
{
	Map<float, std::string> myMap;

	for (uint16 i = 0; i < MAP_ITERATIONS; ++i)
	{
		myMap.Emplace(GameMath::RandomFloatInRange(0.0f, 100.0f), "Hjalmar");
	}

	for (uint16 i = 0; i < MAP_ITERATIONS; ++i)
	{
		volatile std::string *CATALYST_RESTRICT myString = myMap.Find(GameMath::RandomFloatInRange(0.0f, 100.0f));
	}
}

void Function2()
{
	std::map<float, std::string> myMap;

	for (uint16 i = 0; i < MAP_ITERATIONS; ++i)
	{
		myMap.emplace(GameMath::RandomFloatInRange(0.0f, 100.0f), "Hjalmar");
	}

	for (uint16 i = 0; i < MAP_ITERATIONS; ++i)
	{
		volatile auto myString = myMap.find(GameMath::RandomFloatInRange(0.0f, 100.0f));
	}
}

int main()
{
	MicroBenchmarker::StartBenchmark(100'000, &Function1, &Function2);

	std::cin.ignore();

	return 0;
}