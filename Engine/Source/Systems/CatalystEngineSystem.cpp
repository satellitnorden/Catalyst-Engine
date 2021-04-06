//Header file.
#include <Systems/CatalystEngineSystem.h>

//Core.
#include <Core/General/UpdateContext.h>
#include <Core/General/DeltaTimer.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Concurrency.
#include <Concurrency/ConcurrencyCore.h>

//Profiling.
#include <Profiling/ProfilingCore.h>

//Resources.
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <Resources/Building/CatalystEngineResourceBuilding.h>
#endif

//Systems.
#include <Systems/AnimationSystem.h>
#if defined(CATALYST_EDITOR)
#include <Systems/CatalystEditorSystem.h>
#endif
#include <Systems/CullingSystem.h>
#include <Systems/DistributionSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/InputSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/PlacementSystem.h>
#if defined(CATALYST_CONFIGURATION_PROFILE)
#include <Systems/ProfilingSystem.h>
#endif
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/SaveSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/UserInterfaceSystem.h>
#include <Systems/WorldSystem.h>

//Singleton definition.
DEFINE_SINGLETON(CatalystEngineSystem);

//Catalyst engine system data.
namespace CatalystEngineSystemData
{

	//Enumeration covering all sequential updates.
	enum class SequentialUpdate : uint8
	{
		PlacementSystem,
		SaveSystem,
		TerrainSystem,

		NumberOfSequentialUpdates
	};

	//The delta timer.
	DeltaTimer<float32> _DeltaTimer;

	//The current sequential update.
	SequentialUpdate _CurrentSequentialUpdate{ static_cast<SequentialUpdate>(0) };

}

//Catalyst engine system logic.
namespace CatalystEngineSystemLogic
{

	/*
	*	Executes a sequential update.
	*/
	FORCE_INLINE void ExecuteSequentialUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		//Execute the sequential update.
		switch (CatalystEngineSystemData::_CurrentSequentialUpdate)
		{
			case CatalystEngineSystemData::SequentialUpdate::PlacementSystem:
			{
				PlacementSystem::Instance->SequentialUpdate();

				break;
			}

			case CatalystEngineSystemData::SequentialUpdate::SaveSystem:
			{
				SaveSystem::Instance->SequentialUpdate(context);

				break;
			}

			case CatalystEngineSystemData::SequentialUpdate::TerrainSystem:
			{
				TerrainSystem::Instance->SequentialUpdate(context);

				break;
			}

			default:
			{
				ASSERT(false, "Unhandled case!");

				break;
			}
		}

		//Update the current sequential update.
		CatalystEngineSystemData::_CurrentSequentialUpdate = CatalystEngineSystemData::_CurrentSequentialUpdate == static_cast<CatalystEngineSystemData::SequentialUpdate>(UNDERLYING(CatalystEngineSystemData::SequentialUpdate::NumberOfSequentialUpdates) - 1) ? static_cast<CatalystEngineSystemData::SequentialUpdate>(0) : static_cast<CatalystEngineSystemData::SequentialUpdate>(UNDERLYING(CatalystEngineSystemData::_CurrentSequentialUpdate) + 1);
	}

}

/*
*	Initializes the Catalyst engine system.
*/
void CatalystEngineSystem::Initialize(const CatalystProjectConfiguration &initial_project_configuration) NOEXCEPT
{
	//Initialize the current thread's index.
	Concurrency::CurrentThread::InitializeIndex();

	//Set the main thread's index to the current thread's index.
	Concurrency::MainThreadIndex() = Concurrency::CurrentThread::Index();

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Build the Catalyst Engine resources.
	CatalystEngineResourceBuilding::BuildResources();
#endif

	//Set the project configuration.
	_ProjectConfiguration = initial_project_configuration;

	//Initialize the platform.
	CatalystPlatform::Initialize();

	//Initialize all systems.
#if defined(CATALYST_EDITOR)
	CatalystEditorSystem::Instance->Initialize();
#endif
	CullingSystem::Instance->Initialize();
	DistributionSystem::Instance->Initialize();
	EntitySystem::Instance->Initialize();
	InputSystem::Instance->Initialize(_ProjectConfiguration._InputConfiguration);
	LevelOfDetailSystem::Instance->Initialize();
	MemorySystem::Instance->Initialize();
	SaveSystem::Instance->Initialize();
	RenderingSystem::Instance->Initialize(_ProjectConfiguration._RenderingConfiguration);
	SoundSystem::Instance->Initialize();
	TaskSystem::Instance->Initialize();
	TerrainSystem::Instance->Initialize(_ProjectConfiguration._TerrainConfiguration);
	UserInterfaceSystem::Instance->Initialize();
	WorldSystem::Instance->Initialize(_ProjectConfiguration._WorldConfiguration);

	//Initialize the game system.
	_ProjectConfiguration._GeneralConfiguration._InitializationFunction();

	//Register the Catalyst Engine resource collection. 
#if defined(CATALYST_CONFIGURATION_FINAL)
	ResourceSystem::Instance->LoadResources("EngineResources");
#else
	ResourceSystem::Instance->LoadResources("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate");
#endif

	//Post-initialize all systems.
	AnimationSystem::Instance->PostInitialize();
	RenderingSystem::Instance->PostInitialize();
	TerrainSystem::Instance->PostInitialize(_ProjectConfiguration._TerrainConfiguration);
	WorldSystem::Instance->PostInitialize();
#if defined(CATALYST_EDITOR)
	RenderingSystem::Instance->EditorPostInitialize();
#endif

#if !defined(CATALYST_EDITOR)
	//If this is a pure game build, start the game immediately.
	_ProjectConfiguration._GeneralConfiguration._StartGameFunction();
#endif

	//Reset the delta timer right before entering the game loop, so that the first update doesn't get messed up delta times.
	CatalystEngineSystemData::_DeltaTimer.Reset();
}

/*
*	Updates the Catalyst engine system.
*/
bool CatalystEngineSystem::Update() NOEXCEPT
{
	//Define constants.
	constexpr float32 MAXIMUM_DELTA_TIME{ 0.1f };

	//Update the total frames.
	++_TotalFrames;

	//Update the total time.
	_TotalTime += _DeltaTime;

	//Update the delta time.
	_DeltaTime = CatalystBaseMath::Minimum(CatalystEngineSystemData::_DeltaTimer.Update(), MAXIMUM_DELTA_TIME) * _UpdateSpeed;

	//Retrieve the preferred frame time.
	float32 preferred_frame_time;

	if (CatalystPlatform::IsWindowInFocus())
	{
		preferred_frame_time = _ProjectConfiguration._RenderingConfiguration._FocusedFrameTime;
	}

	else
	{
		preferred_frame_time = _ProjectConfiguration._RenderingConfiguration._UnfocusedFrameTime;
	}

	//If the preferred frame time has been set, sleep for the appropriate amount of time to keep that frame time.
	if (preferred_frame_time > 0.0f)
	{
		const float64 seconds_to_sleep{ static_cast<float64>(preferred_frame_time) - static_cast<float64>(_DeltaTime) };

		if (seconds_to_sleep > 0.0)
		{
			Concurrency::CurrentThread::SleepFor(static_cast<uint64>(seconds_to_sleep * 1'000'000'000.0));

			_DeltaTime = CatalystBaseMath::Minimum(_DeltaTime + CatalystEngineSystemData::_DeltaTimer.Update(), MAXIMUM_DELTA_TIME) * _UpdateSpeed;
		}
	}

	//Construct the update context.
	UpdateContext context;

	context._TotalFrames = _TotalFrames;
	context._TotalTime = _TotalTime;
	context._DeltaTime = _DeltaTime;

	/*
	*	Pre update phase.
	*/
	UpdateIndividualPhase(UpdatePhase::PRE);

	/*
	*	Entity update phase.
	*/
	UpdateIndividualPhase(UpdatePhase::ENTITY);

	/*
	*	Input update phase.
	*/
	UpdateIndividualPhase(UpdatePhase::INPUT);

	/*
	*	User interface update phase.
	*/
	UpdateIndividualPhase(UpdatePhase::USER_INTERFACE);

	/*
	*	Logic update phase.
	*/
	UpdateIndividualPhase(UpdatePhase::LOGIC);

	/*
	*	Physics update phase.
	*/
	UpdateIndividualPhase(UpdatePhase::PHYSICS);
	
#if defined(CATALYST_CONFIGURATION_PROFILE)
	ProfilingSystem::PhysicsUpdate(&context);
#endif
	PhysicsSystem::Instance->PhysicsUpdate(&context);

	/*
	*	Render update phase.
	*/
	UpdateIndividualPhase(UpdatePhase::RENDER);

	CullingSystem::Instance->RenderUpdate(&context);
	LevelOfDetailSystem::Instance->RenderUpdate(&context);
	RenderingSystem::Instance->RenderUpdate(&context);

	/*
	*	Post update phase.
	*/
	UpdateIndividualPhase(UpdatePhase::POST);

#if defined(CATALYST_CONFIGURATION_PROFILE)
	ProfilingSystem::PostUpdate(&context);
#endif

	/*
	*	Sequential update phase.
	*/
	CatalystEngineSystemLogic::ExecuteSequentialUpdate(&context);

	//Return if the game should be terminated.
	return !_ShouldTerminate;
}

/*
*	Releases the Catalyst engine system.
*/
void CatalystEngineSystem::Terminate() NOEXCEPT
{
	//Pre-terminate all systems.
	SaveSystem::Instance->PreTerminate();

	//Signal to other systems that the game should terminate.
	_ShouldTerminate = true;

	//Terminate the game system.
	_ProjectConfiguration._GeneralConfiguration._TerminationFunction();

	//Terminate the task system first so that all asynchronous tasks are finished before releasing anything else.
	TaskSystem::Instance->Terminate();

	//Terminate all systems.
#if defined(CATALYST_EDITOR)
	CatalystEditorSystem::Instance->Terminate();
#endif
	DistributionSystem::Instance->Terminate();
	ResourceSystem::Instance->Terminate();
	SoundSystem::Instance->Terminate();
	WorldSystem::Instance->Terminate();

	//Terminate the rendering system last, currently it might cause crashes, so make sure systems that save to disk do that first at least as to not lose data.
	RenderingSystem::Instance->Terminate();
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
											const bool run_on_main_thread) NOEXCEPT
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

	//Set up the update data task.
	new_update_data->_Task._Function = update_function;
	new_update_data->_Task._Arguments = update_arguments;
	new_update_data->_Task._ExecutableOnSameThread = true;

	//Add the update data to the appropriate start/end containers.
	_StartUpdateData[UNDERLYING(start)].Emplace(new_update_data);
	_EndUpdateData[UNDERLYING(end)].Emplace(new_update_data);

	//Return the identifier.
	return new_update_data->_Identifier;
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
*	Updates an individual phase.
*/
void CatalystEngineSystem::UpdateIndividualPhase(const UpdatePhase phase) NOEXCEPT
{
	//Wait for the update data that ends in this update phase.
	for (UpdateData *const RESTRICT update_data : _EndUpdateData[UNDERLYING(phase)])
	{
		update_data->_Task.Wait<WaitMode::PAUSE>();
	}

	//Execute the tasks for the update data that starts in this update phase.
	for (UpdateData *const RESTRICT update_data : _StartUpdateData[UNDERLYING(phase)])
	{
		if (!update_data->_RunOnMainThread)
		{
			TaskSystem::Instance->ExecuteTask(&update_data->_Task);
		}
	}

	//Next, execute the tasks for the update data that starts in this update phase that needs to be run on the main thread.
	for (UpdateData *const RESTRICT update_data : _StartUpdateData[UNDERLYING(phase)])
	{
		if (update_data->_RunOnMainThread)
		{
			update_data->_Task.Execute();
		}
	}
}