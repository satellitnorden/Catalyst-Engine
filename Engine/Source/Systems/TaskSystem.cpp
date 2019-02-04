//Header file.
#include <Systems/TaskSystem.h>

//Multithreading.
#include <Multithreading/Task.h>

//Singleton definition.
DEFINE_SINGLETON(TaskSystem);

/*
*	Initializes the task system.
*/
void TaskSystem::InitializeSystem() NOEXCEPT
{
	//Find out how many hardware threads there is.
	uint32 numberOfHardwareThreads = std::thread::hardware_concurrency();

	//In the rare case that the number of hardware threads cannot be retrieved, default to 8.
	if (numberOfHardwareThreads == 0)
	{
		numberOfHardwareThreads = 8;
	}

	//Set the number of task executors. Leave one slot open for the main thread and another slot open for the operating system.
	_NumberOfTaskExecutors = numberOfHardwareThreads - 2;

	//Kick off all task executor threads.
	_TaskExecutorThreads.Reserve(_NumberOfTaskExecutors);

	for (uint8 i = 0; i < _NumberOfTaskExecutors; ++i)
	{
		_TaskExecutorThreads.EmplaceFast(std::move(std::thread([](void *const RESTRICT)
		{
			TaskSystem::Instance->ExecuteTasks();
		}, nullptr)));
	}
}

/*
*	Releases the task system.
*/
void TaskSystem::ReleaseSystem() NOEXCEPT
{
	//Stop executing tasks.
	_ExecuteTasks = false;

	//Join all adventurer threads.
	for (std::thread &taskExecutorThread : _TaskExecutorThreads)
	{
		taskExecutorThread.join();
	}
}

/*
*	Executes a task.
*/
void TaskSystem::ExecuteTask(Task *const RESTRICT newTask) NOEXCEPT
{
	ASSERT(_TasksInQueue < MAXIMUM_NUMBER_OF_TASKS, "Pushing too many tasks to the task queue, increase maximum number of tasks!");

	//Reset the semaphore.
	newTask->_Semaphore.Reset();

	//Put the task into the task queue.
	_TaskQueue.Push(newTask);

	//Update the number of tasks in the queue.
	++_TasksInQueue;
}

/*
*	Executes tasks.
*/
void TaskSystem::ExecuteTasks() NOEXCEPT
{
	while (_ExecuteTasks)
	{
		//Try to pop a task from the task queue, and execute it if it succeeds.
		if (Task *const RESTRICT *const RESTRICT newTask{ _TaskQueue.Pop() })
		{
			(*newTask)->Execute();

			--_TasksInQueue;
		}
	}
}