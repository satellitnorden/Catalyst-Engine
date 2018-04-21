//Header file.
#include <ClairvoyantGameSystem.h>

//Clairvoyant.
#include <ClairvoyantPlayer.h>
#include <TimeSystem.h>

//Multithreading.
#include <Multithreading/Semaphore.h>
#include <Multithreading/Task.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ClairvoyantGameSystem);

/*
*	Initializes the game system.
*/
void ClairvoyantGameSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the world architect.
	WorldArchitect::Instance->Initialize();

	//Create the player.
	player = EntitySystem::Instance->CreateEntity<ClairvoyantPlayer>();

	//Initialize the player.
	player->Initialize();

	//Initialize the sound director.
	ClairvoyantSoundDirector::Instance->Initialize();

	//Initialize the time system.
	TimeSystem::Instance->Initialize();
}

/*
*	Updates the game system synchronously.
*/
void ClairvoyantGameSystem::UpdateSystemSynchronous(const float deltaTime) NOEXCEPT
{
	//Create the required semaphores.
	enum class ClairvoyantUpdateSemaphores : uint8
	{
		Player,
		SoundDirector,
		TimeSystem,
		WorldArchitect,
		NumberOfClairvoyantUpdateSemaphores
	};

	static StaticArray<Semaphore, INDEX(ClairvoyantUpdateSemaphores::NumberOfClairvoyantUpdateSemaphores)> clairvoyantUpdateSemaphores;

	//Create the Clairvoyant update context.
	class ClairvoyantUpdateContext final
	{

	public:

		float deltaTime;
		ClairvoyantGameSystem *RESTRICT gameSystem;

	} clairvoyantUpdateContext;

	clairvoyantUpdateContext.deltaTime = deltaTime;
	clairvoyantUpdateContext.gameSystem = this;

	//Update the player.
	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		ClairvoyantUpdateContext *const RESTRICT updateContext = static_cast<ClairvoyantUpdateContext *const RESTRICT>(arguments);

		updateContext->gameSystem->player->Update(updateContext->deltaTime);
	}, &clairvoyantUpdateContext, &clairvoyantUpdateSemaphores[INDEX(ClairvoyantUpdateSemaphores::Player)]));

	//Update the sound director.
	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		ClairvoyantUpdateContext *const RESTRICT updateContext = static_cast<ClairvoyantUpdateContext *const RESTRICT>(arguments);

		//Update the sound director.
		ClairvoyantSoundDirector::Instance->Update();
	}, &clairvoyantUpdateContext, &clairvoyantUpdateSemaphores[INDEX(ClairvoyantUpdateSemaphores::SoundDirector)]));

	//Update the time system.
	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		ClairvoyantUpdateContext *const RESTRICT updateContext = static_cast<ClairvoyantUpdateContext *const RESTRICT>(arguments);

		TimeSystem::Instance->Update(updateContext->deltaTime);
	}, &clairvoyantUpdateContext, &clairvoyantUpdateSemaphores[INDEX(ClairvoyantUpdateSemaphores::TimeSystem)]));

	//Update the world architect.
	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		ClairvoyantUpdateContext *const RESTRICT updateContext = static_cast<ClairvoyantUpdateContext *const RESTRICT>(arguments);

		WorldArchitect::Instance->Update(updateContext->deltaTime);
	}, &clairvoyantUpdateContext, &clairvoyantUpdateSemaphores[INDEX(ClairvoyantUpdateSemaphores::WorldArchitect)]));

	//Wait for all Clairvoyant update tasks to finish.
	for (const Semaphore &clairvoyantUpdateSemaphore : clairvoyantUpdateSemaphores)
	{
		clairvoyantUpdateSemaphore.WaitFor();
	}
}

/*
*	Releases the game system.
*/
void ClairvoyantGameSystem::ReleaseSystem() NOEXCEPT
{

}