#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/Spinlock.h>

//Profiling.
#include <Profiling/ProfilingEntry.h>
#include <Profiling/ProfilingUserInterfaceScene.h>

class ALIGN(8) ProfilingSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ProfilingSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ProfilingSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the profiling system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Adds a profiling entry.
	*/
	void AddProfilingEntry(const char *const RESTRICT name, const float32 duration) NOEXCEPT;

	/*
	*	Returns and clears the profiling entries.
	*/
	void GetAndClearProfilingEntries(DynamicArray<ProfilingEntry> *const RESTRICT out_entries) NOEXCEPT;

private:

	//The profiling entries.
	DynamicArray<ProfilingEntry> _ProfilingEntries;

	//The profiling entries lock.
	Spinlock _ProfilingEntriesLock;

	//Denotes if the profiling user interface should be shown.
	bool _ShowProfilingUserInterface{ false };

	//The profiling user interface scene.
	ProfilingUserInterfaceScene _ProfilingUserInterfaceScene;

	/*
	*	Updates the profiling system during the PRE update phase.
	*/
	void PreUpdate() NOEXCEPT;

};
#endif