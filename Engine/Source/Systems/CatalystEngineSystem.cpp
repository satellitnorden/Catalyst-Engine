//Header file.
#include <Systems/CatalystEngineSystem.h>

//Core.
#include <Core/General/DeltaTimer.h>

//Components.
#include <Components/Core/Component.h>

//Concurrency.
#include <Concurrency/Concurrency.h>

//Profiling.
#include <Profiling/Profiling.h>

//Resources.
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <Resources/Building/CatalystEngineResourceBuilding.h>
#endif

//Systems.
#if defined(CATALYST_EDITOR)
#include <Systems/CatalystEditorSystem.h>
#endif
#include <Systems/ContentSystem.h>
#include <Systems/LogSystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/SaveSystem.h>
#include <Systems/TaskSystem.h>

//Catalyst engine system data.
namespace CatalystEngineSystemData
{

	//The delta timer.
	DeltaTimer<float32> _DeltaTimer;

}

/*
*	Initializes the Catalyst engine system.
*/
void CatalystEngineSystem::Initialize(const CatalystProjectConfiguration &initial_project_configuration) NOEXCEPT
{
	PROFILING_FRAME();
	PROFILING_SCOPE("CatalystEngineSystem::Initialize()");

	//Initialize the current thread's index.
	Concurrency::CurrentThread::InitializeIndex();

	//Set the main thread's index to the current thread's index.
	Concurrency::MainThreadIndex() = Concurrency::CurrentThread::Index();

	//Set the project configuration.
	_ProjectConfiguration = initial_project_configuration;

	//Set whether or not to run the engine single-threaded.
	_SingleThreaded = _ProjectConfiguration._ConcurrencyConfiguration._NumberOfTaskExecutors.Valid() && _ProjectConfiguration._ConcurrencyConfiguration._NumberOfTaskExecutors.Get() == 0;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Build the Catalyst Engine resources.
	CatalystEngineResourceBuilding::BuildResources(_ProjectConfiguration);
#endif

	//Initialize the platform.
	{
		PROFILING_SCOPE("CatalystPlatform::Initialize();");
		CatalystPlatform::Initialize();
	}

	//Initialize the task system here, as subsequent steps might use that.
	{
		PROFILING_SCOPE("TaskSystem::Instance->Initialize()");
		TaskSystem::Instance->Initialize();
	}

	//Initialize all systems.
	Systems::Initialize();
	SaveSystem::Instance->Initialize();
	
#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Compile both for engine and game.
	ContentSystem::Instance->CompileEngine();
	ContentSystem::Instance->CompileGame();
#endif

	//Load assets.
#if !defined(CATALYST_CONFIGURATION_FINAL)
	ContentSystem::Instance->LoadAssetCollection("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Collections\\Base.cac");
	ResourceSystem::Instance->LoadResources("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Intermediate\\Base");
#else
	ContentSystem::Instance->LoadAssetCollection("EngineContent\\Base.cac");
	ResourceSystem::Instance->LoadResources("EngineContent\\Old");
#endif

#if defined(CATALYST_EDITOR)
	ContentSystem::Instance->LoadAssetCollection("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Collections\\Editor.cac");
#endif

	//Post-initialize all systems.
	Systems::PostInitialize();

#if defined(CATALYST_EDITOR)
	//Post-initialize the editor system. It depends on stuff being set up in 'RenderingSystem::PostInitialize()', so call it manually here...
	CatalystEditorSystem::Instance->PostInitialize();
#endif

#if !defined(CATALYST_EDITOR)
	//If this is a pure game build, start the game immediately.
	Components::StartGame();
	_ProjectConfiguration._GeneralConfiguration._StartGameFunction();
#endif

	//Flush the logs after initialization.
	LogSystem::Instance->Flush();

	//Initialize the frame pacer.
	_FramePacer.Initialize();

	//Reset the delta timer right before entering the game loop, so that the first update doesn't get messed up delta times.
	CatalystEngineSystemData::_DeltaTimer.Reset();
}

/*
*	Updates the Catalyst engine system.
*/
bool CatalystEngineSystem::Update() NOEXCEPT
{
	PROFILING_FRAME();
	PROFILING_SCOPE("CatalystEngineSystem::Update()");

	//Define constants.
	constexpr float32 MAXIMUM_DELTA_TIME{ 0.1f };

	//Pre-update the platform.
	CatalystPlatform::PlatformPreUpdate();

	//Is the engine paused?
	if (_EnginePaused)
	{
		Concurrency::CurrentThread::SleepFor(500'000'000);

		return true;
	}

	//Update the total frames.
	++_TotalFrames;

	//Update the total time.
	_TotalTime += _DeltaTime;

	//Retrieve the preferred refresh rate.
	float32 preferred_refresh_rate;

	if (CatalystPlatform::IsWindowInFocus())
	{
		preferred_refresh_rate = _ProjectConfiguration._RenderingConfiguration._FocusedRefreshRate;
	}

	else
	{
		preferred_refresh_rate = _ProjectConfiguration._RenderingConfiguration._UnfocusedRefreshRate;
	}

	//Set the delta time.
	if (preferred_refresh_rate > 0.0f)
	{
		_DeltaTime = BaseMath::Minimum(1.0f / preferred_refresh_rate, MAXIMUM_DELTA_TIME) * _UpdateSpeed;
	}

	else
	{
		_DeltaTime = BaseMath::Minimum(CatalystEngineSystemData::_DeltaTimer.Update(), MAXIMUM_DELTA_TIME) * _UpdateSpeed;
	}

	/*
	*	Pre update phase.
	*/
	Systems::Update(UpdatePhase::PRE);
	{
		PROFILING_SCOPE("UpdatePhase_Pre");

		UpdateIndividualPhase(UpdatePhase::PRE);
	}

	/*
	*	Entity update phase.
	*/
	Systems::Update(UpdatePhase::ENTITY);
	{
		PROFILING_SCOPE("UpdatePhase_Entity");

		UpdateIndividualPhase(UpdatePhase::ENTITY);
	}

	/*
	*	Input update phase.
	*/
	Systems::Update(UpdatePhase::INPUT);
	{
		PROFILING_SCOPE("UpdatePhase_Input");

		UpdateIndividualPhase(UpdatePhase::INPUT);
	}

	/*
	*	Gameplay update phase.
	*/
	Systems::Update(UpdatePhase::GAMEPLAY);
	{
		PROFILING_SCOPE("UpdatePhase_Gameplay");

		UpdateIndividualPhase(UpdatePhase::GAMEPLAY);
	}

	/*
	*	User interface update phase.
	*/
	Systems::Update(UpdatePhase::USER_INTERFACE);
	{
		PROFILING_SCOPE("UpdatePhase_UserInterface");

		UpdateIndividualPhase(UpdatePhase::USER_INTERFACE);
	}

	/*
	*	Physics update phase.
	*/
	Systems::Update(UpdatePhase::PHYSICS);
	{
		PROFILING_SCOPE("UpdatePhase_Physics");

		UpdateIndividualPhase(UpdatePhase::PHYSICS);
	}

	/*
	*	Pre-render update phase.
	*/
	Systems::Update(UpdatePhase::PRE_RENDER);
	{
		PROFILING_SCOPE("UpdatePhase_PreRender");

		UpdateIndividualPhase(UpdatePhase::PRE_RENDER);
	}

	/*
	*	Render update phase.
	*/
	Systems::Update(UpdatePhase::RENDER);
	{
		PROFILING_SCOPE("UpdatePhase_Render");

		UpdateIndividualPhase(UpdatePhase::RENDER);

		RenderingSystem::Instance->RenderUpdate();
	}

	/*
	*	Post update phase.
	*/
	Systems::Update(UpdatePhase::POST);
	{
		PROFILING_SCOPE("UpdatePhase_Post");

		UpdateIndividualPhase(UpdatePhase::POST);
	}

	/*
	*	Sequential update phase.
	*/
	{
		PROFILING_SCOPE("UpdatePhase_Sequential");

		UpdateSequentially();
	}

	//Post-update the platform.
	CatalystPlatform::PlatformPostUpdate();

	//Post-update the frame pacer.
	_FramePacer.PostUpdate(preferred_refresh_rate);

	//Return if the game should be terminated.
	return !_ShouldTerminate;
}

/*
*	Releases the Catalyst engine system.
*/
void CatalystEngineSystem::Terminate() NOEXCEPT
{
	PROFILING_SCOPE("CatalystEngineSystem_Terminate");

	//Flush the logs before termination.
	LogSystem::Instance->Flush();

	//Signal to other systems that the game should terminate.
	_ShouldTerminate = true;

	//Terminate the game system.
	_ProjectConfiguration._GeneralConfiguration._TerminateFunction();

	//Terminate the task system first so that all asynchronous tasks are finished before releasing anything else.
	TaskSystem::Instance->Terminate();

	//Terminate all systems.
	Systems::Terminate();

	//Terminate the save system here, since other system's terminations might have queued up saves.
	SaveSystem::Instance->Terminate();

	//Terminate the platform.
	CatalystPlatform::Terminate();

	//Post-terminate all systems.
	Systems::PostTerminate();

	//Flush the logs after termination.
	LogSystem::Instance->Flush();
}

/*
*	Registers an update.
*	Returns a unique identifier for this update,
*	which can later be used to query information about the update and deregister the update.
*/
uint64 CatalystEngineSystem::RegisterUpdate(const UpdateFunction update_function,
											void *const RESTRICT update_arguments,
											const UpdatePhase start,
											const UpdatePhase end,
											const bool run_on_main_thread,
											const bool only_update_in_game) NOEXCEPT
{
	//Allocate the update data.
	UpdateData* const RESTRICT new_update_data{ new (MemorySystem::Instance->TypeAllocate<UpdateData>()) UpdateData() };

	//Set the properties.
	new_update_data->_Identifier = _UpdateDataCounter++;
	new_update_data->_UpdateFunction = update_function;
	new_update_data->_UpdateArguments = update_arguments;
	new_update_data->_Start = start;
	new_update_data->_End = end;
	new_update_data->_RunOnMainThread = run_on_main_thread;
	new_update_data->_OnlyUpdateInGame = only_update_in_game;

	//Set up the update data task.
	new_update_data->_Task._Function = update_function;
	new_update_data->_Task._Arguments = update_arguments;
	new_update_data->_Task._ExecutableOnSameThread = true;

	//Add the update data to the appropriate start/end containers.
	_StartUpdateData[BitIndex(UNDERLYING(start))].Emplace(new_update_data);
	_EndUpdateData[BitIndex(UNDERLYING(end))].Emplace(new_update_data);

	//Return the identifier.
	return new_update_data->_Identifier;
}

/*
*	Registers a  sequential update.
*	Returns a unique identifier for this sequential update,
*	which can later be used to query information about the update and deregister the update.
*	Sequential updates are run sequentially at the end of the frame,
*	e.g. if multiple are registered, it will run once ever few frames or so.
*	Useful for systems that just needs to check a few things, kick off a few tasks or so,
*	but doesn't need to get updated every frame.
*/
uint64 CatalystEngineSystem::RegisterSequentialUpdate(const UpdateFunction update_function, void* const RESTRICT update_arguments) NOEXCEPT
{
	//Construct the new sequential update.
	_SequentialUpdateData.Emplace();
	SequentialUpdateData &new_update{ _SequentialUpdateData.Back() };

	new_update._Identifier = _SequentialUpdateDataCounter++;
	new_update._UpdateFunction = update_function;
	new_update._UpdateArguments = update_arguments;

	//Return the identifier.
	return new_update._Identifier;
}

/*
*	Deregisters an update.
*/
void CatalystEngineSystem::DeregisterUpdate(const uint64 identifier) NOEXCEPT
{
	for (DynamicArray<UpdateData *RESTRICT> &update_data : _StartUpdateData)
	{
		for (uint64 i{ 0 }, size{ update_data.Size() }; i < size; ++i)
		{
			if (update_data[i]->_Identifier == identifier)
			{
				MemorySystem::Instance->TypeFree<UpdateData>(update_data[i]);

				update_data.EraseAt<false>(i);

				break;
			}
		}
	}

	for (DynamicArray<UpdateData *RESTRICT> &update_data : _EndUpdateData)
	{
		for (uint64 i{ 0 }, size{ update_data.Size() }; i < size; ++i)
		{
			if (update_data[i]->_Identifier == identifier)
			{
				update_data.EraseAt<false>(i);

				break;
			}
		}
	}
}

/*
*	Deregisters a sequential update.
*/
void CatalystEngineSystem::DeregisterSequentialUpdate(const uint64 identifier) NOEXCEPT
{
	for (uint64 i{ 0 }; i < _SequentialUpdateData.Size(); ++i)
	{
		if (_SequentialUpdateData[i]._Identifier == identifier)
		{
			_SequentialUpdateData.EraseAt<false>(i);

			return;
		}
	}
}

/*
*	Updates an individual phase.
*/
void CatalystEngineSystem::UpdateIndividualPhase(const UpdatePhase phase) NOEXCEPT
{
	if (_SingleThreaded)
	{
		//Execute the tasks for the update data that starts in this update phase.
		for (UpdateData *const RESTRICT update_data : _StartUpdateData[BitIndex(UNDERLYING(phase))])
		{
#if defined(CATALYST_EDITOR)
			if (update_data->_OnlyUpdateInGame && !CatalystEditorSystem::Instance->IsInGame())
			{
				continue;
			}
#endif
			update_data->_Task.Execute();
		}
	}

	else
	{
		//Finish up the work that ends in this update phase.
		bool all_done{ _EndUpdateData[BitIndex(UNDERLYING(phase))].Empty() };

		while (!all_done)
		{
			TaskSystem::Instance->DoWork(Task::Priority::HIGH);

			all_done = true;

			for (UpdateData *const RESTRICT update_data : _EndUpdateData[BitIndex(UNDERLYING(phase))])
			{
				all_done &= update_data->_Task.IsExecuted();
			}
		}

		//Execute the tasks for the update data that starts in this update phase.
		for (UpdateData	*const RESTRICT update_data : _StartUpdateData[BitIndex(UNDERLYING(phase))])
		{
#if defined(CATALYST_EDITOR)
			if (update_data->_OnlyUpdateInGame && !CatalystEditorSystem::Instance->IsInGame())
			{
				continue;
			}
#endif
			if (!update_data->_RunOnMainThread)
			{
				TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &update_data->_Task);
			}
		}

		//Next, execute the tasks for the update data that starts in this update phase that needs to be run on the main thread.
		for (UpdateData *const RESTRICT update_data : _StartUpdateData[BitIndex(UNDERLYING(phase))])
		{
#if defined(CATALYST_EDITOR)
			if (update_data->_OnlyUpdateInGame && !CatalystEditorSystem::Instance->IsInGame())
			{
				continue;
			}
#endif
			if (update_data->_RunOnMainThread)
			{
				update_data->_Task.Execute();
			}
		}
	}
}

/*
*	Updates sequentially.
*/
void CatalystEngineSystem::UpdateSequentially() NOEXCEPT
{
	//If there's no updates to run, then... Don't. ¯\_(ツ)_/¯
	if (_SequentialUpdateData.Empty())
	{
		return;
	}

	//Update the current sequential update index.
	_CurrentSequentialUpdateIndex = (_CurrentSequentialUpdateIndex + 1) % _SequentialUpdateData.Size();

	//Run the update.
	_SequentialUpdateData[_CurrentSequentialUpdateIndex]._UpdateFunction(_SequentialUpdateData[_CurrentSequentialUpdateIndex]._UpdateArguments);
}