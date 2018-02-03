
//Engine core.
#include <EngineCore.h>

//Systems.
#include <EngineSystem.h>

int main() CATALYST_NOEXCEPT
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
#include <SharedPointer.h>
#include <memory>

void Function1()
{
	std::shared_ptr<DynamicString> p1{ new DynamicString };
	std::shared_ptr<DynamicString> p2{ new DynamicString };

	p1 = p2;
}

void Function2()
{
	SharedPointer<DynamicString> p1{ new DynamicString };
	SharedPointer<DynamicString> p2{ new DynamicString };

	p1 = p2;
}

int main()
{
	MicroBenchmarker::StartBenchmark(100'000'000, &Function1, &Function2);

	return 0;
}
*/