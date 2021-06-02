#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Systems/ProfilingSystem.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/UserInterfaceSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ProfilingSystem);

/*
*	Initializes the profiling system.
*/
void ProfilingSystem::Initialize() NOEXCEPT
{
	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT)
	{
		ProfilingSystem::Instance->PreUpdate();
	},
	nullptr,
	UpdatePhase::PRE,
	UpdatePhase::USER_INTERFACE,
	true,
	false);
}

/*
*	Adds a profiling entry.
*/
void ProfilingSystem::AddProfilingEntry(const char *const RESTRICT name, const float32 duration) NOEXCEPT
{
	SCOPED_LOCK(_ProfilingEntriesLock);

	_ProfilingEntries.Emplace(name, duration);
}

/*
*	Returns and clears the profiling entries.
*/
void ProfilingSystem::GetAndClearProfilingEntries(DynamicArray<ProfilingEntry> *const RESTRICT out_entries) NOEXCEPT
{
	SCOPED_LOCK(_ProfilingEntriesLock);

	*out_entries = _ProfilingEntries;
	_ProfilingEntries.Clear();
}

/*
*	Updates the profiling system during the PRE update phase.
*/
void ProfilingSystem::PreUpdate() NOEXCEPT
{
#if 1
	//Should the profiling user interface scene be activated?
	if (!_ProfilingUserInterfaceScene.GetIsActive())
	{
		UserInterfaceSystem::Instance->ActivateScene(&_ProfilingUserInterfaceScene);
	}
#endif
}
#endif