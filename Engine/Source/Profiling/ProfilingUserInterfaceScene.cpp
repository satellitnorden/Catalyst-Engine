#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Profiling/ProfilingUserInterfaceScene.h>

//Systems.
#include <Systems/ProfilingSystem.h>

/*
*	Callback for when this scene is activated.
*/
void ProfilingUserInterfaceScene::OnActivated() NOEXCEPT
{
	//Set the horizontal/vertical subdivison.
	SetHorizontalSubdivision(1);
	SetVerticalSubdivision(64);

	//Update the profiling entries.
	UpdateProfilingEntries();
}

/*
*	Callback for when this user interface scene is activated updated
*/
void ProfilingUserInterfaceScene::Update() NOEXCEPT
{
	//Update the profiling entries.
	UpdateProfilingEntries();
}

/*
*	Updates the profiling entries.
*/
void ProfilingUserInterfaceScene::UpdateProfilingEntries() NOEXCEPT
{
	//Get and clear the current profiling entries.
	ProfilingSystem::Instance->GetAndClearProfilingEntries(&_CurrentProfilingEntries);

	//Add the UI for all profiling entries.
	for (uint64 i{ 0 }, size{ _CurrentProfilingEntries.Size() }; i < size; ++i)
	{
		//Cache the current profiling entry.
		const ProfilingEntry &current_profiling_entry{ _CurrentProfilingEntries[i] };

		//Calculate the text.
		char buffer[128];
		sprintf_s(buffer, "%s: %.2f milliseconds", current_profiling_entry._Name, current_profiling_entry._Duration);

		//Does a progress bar exist for this entry?
		if (_ProfilingEntryProgressBars.Empty() || _ProfilingEntryProgressBars.Size() - 1 < i)
		{
			_ProfilingEntryProgressBars.Emplace(AddProgressBar(	Vector2<uint32>(0, 0),
																Vector2<uint32>(0, 0),
																buffer));
		}

		else
		{
			_ProfilingEntryProgressBars[i]->SetText(buffer);
		}
	}
}
#endif