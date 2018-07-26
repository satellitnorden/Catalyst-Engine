//Header file.
#include <Systems/TaskSystem.h>

//Multithreading.
#include <Multithreading/Task.h>

//Systems.
#include <Systems/EngineSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TaskSystem);

//Task system constants.
namespace TaskSystemConstants
{
	//Denotes the maximum number of tasks that the engine system will itself fire off at any given time.
	static constexpr uint32 NUMBER_OF_ENGINE_TASKS{ 32 };
}

/*
*	Initializes the task system.
*/
void TaskSystem::InitializeSystem(const CatalystProjectMultithreadingInformation &multithreadingInformation) NOEXCEPT
{
	//Find out how many hardware threads there is.
	uint32 numberOfHardwareThreads = std::thread::hardware_concurrency();

	//In the rare case that the number of hardware threads cannot be retrieved, default to 8.
	if (numberOfHardwareThreads == 0)
		numberOfHardwareThreads = 8;

	//Set the number of task executors. Leave one slot open for the main thread.
	numberOfTaskExecutors = numberOfHardwareThreads - 1;

	//Initialize the task queue.
	taskQueue.Initialize(numberOfTaskExecutors + TaskSystemConstants::NUMBER_OF_ENGINE_TASKS + multithreadingInformation.numberOfGameTasks);

	//Kick off all task executor threads.
	taskExecutorThreads.Reserve(numberOfTaskExecutors);

	for (uint8 i = 0; i < numberOfTaskExecutors; ++i)
	{
		taskExecutorThreads.EmplaceFast(std::move(std::thread(&TaskSystem::ExecuteTaskExecutor, this)));
	}
}

/*
*	Releases the task system.
*/
void TaskSystem::ReleaseSystem() NOEXCEPT
{
	//Stop executing tasks.
	executeTasks = false;

	//Join all adventurer threads.
	for (auto &taskExecutorThread : taskExecutorThreads)
	{
		taskExecutorThread.join();
	}
}

/*
*	Executes a task.
*/
void TaskSystem::ExecuteTask(Task *const RESTRICT newTask) NOEXCEPT
{
	//Reset the semaphore.
	newTask->semaphore->Reset();

	//Put the task into the task queue.
	taskQueue.Push(newTask);
}

/*
*	Executes a task executor.
*/
void TaskSystem::ExecuteTaskExecutor() NOEXCEPT
{
	while (executeTasks)
	{
		//Try to pop a task from the task queue, and execute it if it succeeds.
		if (Task *const RESTRICT *const RESTRICT newTask{ taskQueue.Pop() })
		{
			(*newTask)->Execute();
		}
	}
}