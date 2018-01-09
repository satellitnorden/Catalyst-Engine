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

/*
#include <EngineCore.h>
#include <MicroBenchmarker.h>
#include <DynamicString.h>

void Function1()
{
	DynamicString myString = "Hjalmar satt på en sten och tittade mot en stuga.";
	myString += " Jajuste! Han runkade också.";
	myString += " Plus att han faktiskt kom lite senare.";
	myString += " Men gud vad jag är äcklig";
}

void Function2()
{
	std::string myString = "Hjalmar satt på en sten och tittade mot en stuga.";
	myString += "Jajuste! Han runkade också.";
	myString += " Plus att han faktiskt kom lite senare.";
	myString += " Men gud vad jag är äcklig";
}

int main()
{
	DynamicString myString = "Hjalmar satt po en sten och tittade mot en stuga.";
	myString += " Jajuste! Han runkade ockso.";
	myString += " Plus att han faktiskt kom lite senare.";
	myString += " Men gud vad jag är äcklig";

	std::cout << myString.CString() << std::endl;

	MicroBenchmarker::StartBenchmark(100'000'000, &Function1, &Function2);

	return 0;
}
*/