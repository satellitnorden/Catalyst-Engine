#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Profiling/ProfilingUserInterfaceScene.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//Systems.
#include <Systems/ProfilingSystem.h>

//Profiling user interface scene constants.
namespace ProfilingUserInterfaceSceneConstants
{
	constexpr uint32 MAXIMUM_NUMBER_OF_ENTRIES{ 64 };
}

/*
*	Callback for when this scene is activated.
*/
void ProfilingUserInterfaceScene::OnActivated() NOEXCEPT
{
	//Call parent function.
	UserInterfaceScene::OnActivated();

	//Set the horizontal/vertical subdivison.
	SetHorizontalSubdivision(1);
	SetVerticalSubdivision(ProfilingUserInterfaceSceneConstants::MAXIMUM_NUMBER_OF_ENTRIES);

	//Clear the profiling entry progress bars.
	_ProfilingEntryProgressBars.Clear();

	//Update the profiling entries.
	UpdateProfilingEntries();
}

/*
*	Callback for when this user interface scene is activated updated
*/
void ProfilingUserInterfaceScene::Update() NOEXCEPT
{
	//Call parent function.
	UserInterfaceScene::Update();

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

	//Does there exist profiling entries?
	if (_CurrentProfilingEntries.Empty())
	{
		return;
	}

	//Calculate the maximum duration.
	float32 maximum_duration{ -FLOAT32_MAXIMUM };

	for (const ProfilingEntry &current_profiling_entry : _CurrentProfilingEntries)
	{
		maximum_duration = CatalystBaseMath::Maximum<float32>(maximum_duration, current_profiling_entry._AverageDuration);
	}

	//Add the UI for all profiling entries.
	for (uint64 i{ 0 }, size{ _CurrentProfilingEntries.Size() }; i < size; ++i)
	{
		//Cache the current profiling entry.
		const ProfilingEntry &current_profiling_entry{ _CurrentProfilingEntries[_CurrentProfilingEntries.LastIndex() - i] };

		//Calculate the text.
		char buffer[256];
		sprintf_s(buffer, "%s: %.2f milliseconds", current_profiling_entry._Name, current_profiling_entry._AverageDuration);

		//Does a progress bar exist for this entry?
		if (_ProfilingEntryProgressBars.Empty() || _ProfilingEntryProgressBars.Size() - 1 < i)
		{
			_ProfilingEntryProgressBars.Emplace(AddProgressBarByCell(	Vector2<uint32>(0, ProfilingUserInterfaceSceneConstants::MAXIMUM_NUMBER_OF_ENTRIES - 1 - static_cast<uint32>(i)),
																		Vector2<uint32>(0, ProfilingUserInterfaceSceneConstants::MAXIMUM_NUMBER_OF_ENTRIES - 1 - static_cast<uint32>(i)),
																		buffer));
			_ProfilingEntryProgressBars[i]->SetCurrentProgress(current_profiling_entry._AverageDuration / maximum_duration);
		}

		else
		{
			_ProfilingEntryProgressBars[i]->SetText(buffer);
			_ProfilingEntryProgressBars[i]->SetCurrentProgress(current_profiling_entry._AverageDuration / maximum_duration);
		}
	}
}
#endif