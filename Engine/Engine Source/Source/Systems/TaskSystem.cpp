//Header file.
#include <Systems/TaskSystem.h>

//Multithreading.
#include <Multithreading\Task.h>

//Systems.
#include <Systems/EngineSystem.h>

//Singleton definition.
DEFINE_SINGLETON(TaskSystem);

/*
*	Default constructor.
*/
TaskSystem::TaskSystem() NOEXCEPT
{

}

/*
*	Default destructor.
*/
TaskSystem::~TaskSystem() NOEXCEPT
{

}

/*
*	Initializes the task system.
*/
void TaskSystem::InitializeSystem() NOEXCEPT
{
	//Find out how many hardware threads there is.
	uint32 numberOfHardwareThreads = std::thread::hardware_concurrency();

	//In the rare case that the number of hardware threads cannot be retrieved, default to 8.
	if (numberOfHardwareThreads == 0)
		numberOfHardwareThreads = 8;

	//Initialize the task queue.
	taskQueue.Initialize(numberOfHardwareThreads + 256);

	//Set the number of task executors.
	numberOfTaskExecutors = numberOfHardwareThreads;

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
	//Join all adventurer threads.
	for (auto &taskExecutorThread : taskExecutorThreads)
	{
		taskExecutorThread.join();
	}
}

/*
*	Executes a task.
*/
void TaskSystem::ExecuteTask(Task &&newTask) NOEXCEPT
{
	//Reset the semaphore.
	if (newTask.semaphore) newTask.semaphore->Reset();

	//Put the task into the task queue.
	taskQueue.Push(newTask);
}

/*
*	Executes a task executor.
*/
void TaskSystem::ExecuteTaskExecutor() NOEXCEPT
{
	while (!EngineSystem::Instance->ShouldTerminate())
	{
		//Try to pop a task from the task queue, and execute it if it succeeds.
		if (Task *const RESTRICT newTask{ taskQueue.Pop() })
		{
			newTask->Execute();
		}

		//Else, sleep for a bit.
		else
		{
			THREAD_SLEEP();
		}
	}
}