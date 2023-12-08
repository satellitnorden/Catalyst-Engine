//Header file.
#include <Systems/TaskSystem.h>

//Concurrency.
#include <Concurrency/ConcurrencyCore.h>
#include <Concurrency/Task.h>

//Profiling.
#include <Profiling/Profiling.h>

//Singleton definition.
DEFINE_SINGLETON(TaskSystem);

/*
*	Initializes the task system.
*/
void TaskSystem::Initialize(const CatalystProjectConcurrencyConfiguration &configuration) NOEXCEPT
{
	ASSERT(!_IsInitialized, "Calling TaskSystem::Initialize when task system is already initialized!");

	//Calculate the number of task executors.
	uint32 number_of_task_executors;

	if (configuration._NumberOfTaskExecutors.Valid())
	{
		number_of_task_executors = configuration._NumberOfTaskExecutors.Get();
	}

	else
	{
		number_of_task_executors = Concurrency::NumberOfHardwareThreads();

		//Subtract the sound thread.
		if (_NumberOfTaskExecutors > 1)
		{
			--_NumberOfTaskExecutors;
		}

		//Leave one thread for the OS. (:
		if (_NumberOfTaskExecutors > 1)
		{
			--_NumberOfTaskExecutors;
		}
	}

	if (number_of_task_executors > 0)
	{
		//Set the number of task executors.
		_NumberOfTaskExecutors = number_of_task_executors;

		//Task executors should now execute tasks.
		_ExecuteTasks = true;

		//Kick off all task executor threads.
		_TaskExecutorThreads.Upsize<true>(_NumberOfTaskExecutors);

#if !defined(CATALYST_CONFIGURATION_FINAL)
		//Keep track of the task executor number.
		uint32 task_executor_number{ 1 };
#endif

		for (Thread& task_executor_thread : _TaskExecutorThreads)
		{
			//Set the function.
			task_executor_thread.SetFunction([]()
			{
				TaskSystem::Instance->ExecuteTasks();
			});

			//Set the priority.
			task_executor_thread.SetPriority(Thread::Priority::ABOVE_NORMAL);

#if !defined(CATALYST_CONFIGURATION_FINAL)
			//Set the name.
			char buffer[32];
			sprintf_s(buffer, "TASK EXECUTOR %u", task_executor_number++);

			task_executor_thread.SetName(buffer);
#endif

			//Launch the thread!
			task_executor_thread.Launch();
		}

		//The task system is now initialized.
		_IsInitialized = true;
	}

	else
	{
		_NumberOfTaskExecutors = 0;
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
	for (Thread& task_executor_thread : _TaskExecutorThreads)
	{
		task_executor_thread.Join();
	}

	_TaskExecutorThreads.Clear();

	//The task system is no longer initialized.
	_IsInitialized = false;
}

/*
*	Executes a task.
*/
void TaskSystem::ExecuteTask(Task *const RESTRICT task) NOEXCEPT
{
	ASSERT(_TasksInQueue < MAXIMUM_NUMBER_OF_TASKS, "Pushing too many tasks to the task queue, increase maximum number of tasks!");

	//Clear the atomic flag denoting whether or not this task is executed.
	task->_IsExecuted.Clear();

	/*
	*	If the number of tasks in queue is the same as the number of task executors, try to run this task on the same thread.
	*	Also, is the task system isn't initialized (possibly due to the engine running single-threaded), we have no choice but to execute it immediately.
	*/
	if ((_TasksInQueue >= _NumberOfTaskExecutors && task->_ExecutableOnSameThread)
		|| !_IsInitialized)
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
*	Does work on the calling thread.
*/
void TaskSystem::DoWork() NOEXCEPT
{
	if (Task *const RESTRICT *const RESTRICT new_task{ _TaskQueue.Pop() })
	{
		(*new_task)->Execute();

		--_TasksInQueue;
	}
}

/*
*	Waits for all tasks to finish.
*/
void TaskSystem::WaitForAllTasksToFinish() const NOEXCEPT
{
	while (_TasksInQueue > 0)
	{
		Concurrency::CurrentThread::Yield();
	}
}

/*
*	Executes tasks.
*/
void TaskSystem::ExecuteTasks() NOEXCEPT
{
	//Initialize the current thread's index.
	Concurrency::CurrentThread::InitializeIndex();

	while (_ExecuteTasks)
	{
		//Try to pop a task from the task queue, and execute it if it succeeds.
		if (Task* const RESTRICT* const RESTRICT new_task{ _TaskQueue.Pop() })
		{
			PROFILING_SCOPE(TaskExecutor_ExecuteTask);

			(*new_task)->Execute();

			--_TasksInQueue;
		}

		else
		{
			Concurrency::CurrentThread::Yield();
		}
	}
}