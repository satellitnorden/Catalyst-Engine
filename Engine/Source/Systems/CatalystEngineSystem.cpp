//Header file.
#include <Systems/CatalystEngineSystem.h>

//Core.
#include <Core/General/UpdateContext.h>
#include <Core/General/DeltaTimer.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Profiling.
#include <Profiling/ProfilingCore.h>

//Resources.
#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
#include <Resources/Building/CatalystEngineResourceBuilding.h>
#endif
#include <Resources/Loading/ResourceLoader.h>

//Systems.
#include <Systems/AnimationSystem.h>
#include <Systems/CullingSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/EntityPlacementSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/PhysicsSystem.h>
#if defined(CATALYST_CONFIGURATION_PROFILE)
#include <Systems/ProfilingSystem.h>
#endif
#include <Systems/RenderingSystem.h>
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
		EntityPlacementSystem,
		SaveSystem,
		TerrainSystem,

		NumberOfSequentialUpdates
	};

	//The delta timer.
	DeltaTimer _DeltaTimer;

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
			case CatalystEngineSystemData::SequentialUpdate::EntityPlacementSystem:
			{
				EntityPlacementSystem::Instance->SequentialUpdate(context);

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
#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
	//Build the Catalyst Engine resources.
	CatalystEngineResourceBuilding::BuildResources();
#endif

	//Set the project configuration.
	_ProjectConfiguration = initial_project_configuration;

	//Initialize the platform.
	CatalystPlatform::Initialize();

	//Initialize all systems.
	CullingSystem::Instance->Initialize();
	EntityPlacementSystem::Instance->Initialize();
	InputSystem::Instance->Initialize();
	LevelOfDetailSystem::Instance->Initialize();
	SaveSystem::Instance->Initialize();
	RenderingSystem::Instance->Initialize(_ProjectConfiguration._RenderingConfiguration);
	SoundSystem::Instance->Initialize(_ProjectConfiguration._SoundConfiguration);
	TaskSystem::Instance->Initialize();
	TerrainSystem::Instance->Initialize(_ProjectConfiguration._TerrainConfiguration);
	UserInterfaceSystem::Instance->Initialize();

	//Load the Catalyst Engine resource collection. 
	ResourceLoader::LoadResourceCollection("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineResourceCollection.crc");

	//Post-initialize all systems.
	AnimationSystem::Instance->PostInitialize();
	RenderingSystem::Instance->PostInitializeSystem();
	WorldSystem::Instance->PostInitialize();

	//Post-initialize the platform.
	CatalystPlatform::PostInitialize();

	//Initialize the game system.
	_ProjectConfiguration._GeneralConfiguration._InitializationFunction();

	//Reset the delta timer right before entering the game loop, so that the first update doesn't get messed up delta times.
	CatalystEngineSystemData::_DeltaTimer.Reset();
}

/*
*	Updates the Catalyst engine system.
*/
bool CatalystEngineSystem::Update() NOEXCEPT
{
	//Update the total frames.
	++_TotalFrames;

	//Update the total time.
	_TotalTime += _DeltaTime;

	//Update the delta time.
	_DeltaTime = CatalystEngineSystemData::_DeltaTimer.Update() * _UpdateSpeed;

	//Construct the update context.
	UpdateContext context;

	context._TotalFrames = _TotalFrames;
	context._TotalTime = _TotalTime;
	context._DeltaTime = _DeltaTime;

	/*
	*	Input update phase.
	*/
	UpdateIndividualPhase(UpdatePhase::INPUT);

	CatalystPlatform::PreUpdate(&context);

	RenderingSystem::Instance->PreUpdate(&context);
	WorldSystem::Instance->PreUpdate(&context);

	/*
	*	User interface update phase.
	*/
	UpdateIndividualPhase(UpdatePhase::USER_INTERFACE);

	/*
	*	Logic update phase.
	*/
	UpdateIndividualPhase(UpdatePhase::LOGIC);

	WorldSystem::Instance->LogicUpdate(&context);

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
	*	Post-update phase.
	*/
	UpdateIndividualPhase(UpdatePhase::POST);

	CatalystPlatform::PostUpdate(&context);

	EntitySystem::Instance->PostUpdate(&context);
	SoundSystem::Instance->PostUpdate(&context);
#if defined(CATALYST_CONFIGURATION_PROFILE)
	ProfilingSystem::PostUpdate(&context);
#endif

	/*
	*	Sequential update phase.
	*/
	CatalystEngineSystemLogic::ExecuteSequentialUpdate(&context);

	//Return if the game should be terminated.
	return !_ShouldTerminate && InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::Escape) != ButtonState::Pressed;
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

	//Release the platform.
	CatalystPlatform::Release();

	//Release all systems.
	RenderingSystem::Instance->Release();
	SoundSystem::Instance->Terminate();
	UserInterfaceSystem::Instance->Terminate();
}

/*
*	Registers an update.
*/
void CatalystEngineSystem::RegisterUpdate(	const UpdateFunction update_function,
											void *const RESTRICT update_arguments,
											const UpdatePhase start,
											const UpdatePhase end) NOEXCEPT
{
	//Allocate the update data.
	UpdateData* const RESTRICT new_update_data{ static_cast<UpdateData *const RESTRICT>(_UpdateDataAllocator.Allocate()) };

	//Set the properties.
	new_update_data->_UpdateFunction = update_function;
	new_update_data->_UpdateArguments = update_arguments;
	new_update_data->_Start = start;
	new_update_data->_End = end;

	//Set up the update data task.
	new_update_data->_Task._Function = update_function;
	new_update_data->_Task._Arguments = update_arguments;
	new_update_data->_Task._ExecutableOnSameThread = true;

	//Add the update data to the appropriate start/end containers.
	_StartUpdateData[UNDERLYING(start)].Emplace(new_update_data);
	_EndUpdateData[UNDERLYING(end)].Emplace(new_update_data);
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

	//Execute the update data that starts in this update phase.
	for (UpdateData *const RESTRICT update_data : _StartUpdateData[UNDERLYING(phase)])
	{
		TaskSystem::Instance->ExecuteTask(&update_data->_Task);
	}
}