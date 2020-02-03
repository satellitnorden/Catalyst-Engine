//Header file.
#include <Systems/TaskSystem.h>

//Multithreading.
#include <Multithreading/Task.h>

//Singleton definition.
DEFINE_SINGLETON(TaskSystem);

/*
*	Initializes the task system.
*/
void TaskSystem::Initialize() NOEXCEPT
{
	//Find out how many hardware threads there is.
	uint32 numberOfHardwareThreads = std::thread::hardware_concurrency();

	//In the rare case that the number of hardware threads cannot be retrieved, default to 8.
	if (numberOfHardwareThreads == 0)
	{
		numberOfHardwareThreads = 8;
	}

	//Set the number of task executors. Leave one slot open for the main thread.
	_NumberOfTaskExecutors = numberOfHardwareThreads - 1;

	//Kick off all task executor threads.
	_TaskExecutorThreads.Reserve(_NumberOfTaskExecutors);

	for (uint8 i = 0; i < _NumberOfTaskExecutors; ++i)
	{
		_TaskExecutorThreads.Emplace(std::move(std::thread([](void *const RESTRICT)
		{
			TaskSystem::Instance->ExecuteTasks();
		}, nullptr)));
	}
}

/*
*	Terminates the task system.
*/
void TaskSystem::Terminate() NOEXCEPT
{
	//Wait for there to be no tasks left in the queue, since there might be tasks depending on other tasks.
	while (_TasksInQueue > 0);

	//Tell the task executor threads to stop executing tasks.
	_ExecuteTasks = false;

	//Join all task executor threads.
	for (std::thread &taskExecutorThread : _TaskExecutorThreads)
	{
		taskExecutorThread.join();
	}
}

/*
*	Executes a task.
*/
void TaskSystem::ExecuteTask(Task *const RESTRICT task) NOEXCEPT
{
	ASSERT(_TasksInQueue < MAXIMUM_NUMBER_OF_TASKS, "Pushing too many tasks to the task queue, increase maximum number of tasks!");

	//Reset the semaphore.
	task->_Semaphore.Reset();

	//If the number of tasks in queue is the same as the number of task executors, try to run this task on the same thread.
	if (_TasksInQueue >= _NumberOfTaskExecutors && task->_ExecutableOnSameThread)
	{
		task->Execute();
	}

	else
	{
		//Push the task into the task queue.
		_TaskQueue.Push(task);

		//Update the number of tasks in the queue.
		++_TasksInQueue;
	}
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