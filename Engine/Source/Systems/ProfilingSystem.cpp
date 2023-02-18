#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Systems/ProfilingSystem.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
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
void ProfilingSystem::AddProfilingEntry(const char* const RESTRICT name, const float32 frame_duration, const float32 average_duration, const float32 maximum_duration) NOEXCEPT
{
	SCOPED_LOCK(_ProfilingEntriesLock);

	_ProfilingEntries.Emplace(name, frame_duration, average_duration, maximum_duration);
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
	//Toggle if the profiling user interface should be shown.
	if (InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::P) == ButtonState::PRESSED)
	{
		_ShowProfilingUserInterface = !_ShowProfilingUserInterface;
	}

	if (_ShowProfilingUserInterface)
	{
		if (!_ProfilingUserInterfaceScene.GetIsActive())
		{
			UserInterfaceSystem::Instance->ActivateScene(&_ProfilingUserInterfaceScene);

			SCOPED_LOCK(_ProfilingEntriesLock);
			_ProfilingEntries.Clear();
		}
	}
	
	else
	{
		if (_ProfilingUserInterfaceScene.GetIsActive())
		{
			UserInterfaceSystem::Instance->DeactivateScene(&_ProfilingUserInterfaceScene);
		}

		SCOPED_LOCK(_ProfilingEntriesLock);
		_ProfilingEntries.Clear();
	}
}
#endif