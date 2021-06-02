#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Profiling.
#include <Profiling/ProfilingEntry.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

class ProfilingUserInterfaceScene final : public UserInterfaceScene
{

public:

	/*
	*	Callback for when this scene is activated.
	*/
	void OnActivated() NOEXCEPT override;

	/*
	*	Callback for when this user interface scene is activated updated
	*/
	void Update() NOEXCEPT override;

private:

	//The current profiling entries.
	DynamicArray<ProfilingEntry> _CurrentProfilingEntries;

	//The current profiling entry progress bars.
	DynamicArray<UserInterfaceProgressBar *RESTRICT> _ProfilingEntryProgressBars;

	/*
	*	Updates the profiling entries.
	*/
	void UpdateProfilingEntries() NOEXCEPT;

};

#endif