#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Concurrency.
#include <Concurrency/Atomic.h>
#include <Concurrency/AtomicQueue.h>
#include <Concurrency/Task.h>
#include <Concurrency/Thread.h>

class ALIGN(8) TaskSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TaskSystem);

	/*
	*	Default constructor.
	*/
	TaskSystem() NOEXCEPT
	{
	}

	/*
	*	Initializes the task system.
	*/
	void Initialize(const CatalystProjectConcurrencyConfiguration &configuration) NOEXCEPT;

	/*
	*	Terminates the task system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns the number of task executors.
	*/
	uint32 GetNumberOfTaskExecutors() const NOEXCEPT
	{
		return _NumberOfTaskExecutors;
	}

	/*
	*	Executes a task.
	*/
	void ExecuteTask(const Task::Priority priority, Task *const RESTRICT task) NOEXCEPT;

	/*
	*	Does work on the calling thread.
	*	Work with be executed with a priority equal to or higher than the given priority.
	*/
	void DoWork(const Task::Priority priority) NOEXCEPT;

	/*
	*	Waits for all tasks to finish.
	*/
	void WaitForAllTasksToFinish() NOEXCEPT;

private:

	//The maximum number of tasks.
	static constexpr uint64 MAXIMUM_NUMBER_OF_TASKS{ 4'096 };

	//Container for all atomic queues in which to put tasks in.
	StaticArray<AtomicQueue<Task *RESTRICT, MAXIMUM_NUMBER_OF_TASKS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::MULTIPLE>, UNDERLYING(Task::Priority::NUMBER_OF_TASK_PRIORITIES)> _TaskQueues;

	//Denotes how many tasks are currently queued.
	StaticArray<Atomic<uint64>, UNDERLYING(Task::Priority::NUMBER_OF_TASK_PRIORITIES)> _TasksInQueue;

	//Denotes whether or not the task system is initialized.
	bool _IsInitialized{ false };

	//Denotes whether or not tasks should be executed.
	bool _ExecuteTasks{ true };

	//The number of task executors.
	uint32 _NumberOfTaskExecutors;

	//Container for all task executor threads.
	DynamicArray<Thread> _TaskExecutorThreads;

	/*
	*	Executes tasks.
	*/
	void ExecuteTasks() NOEXCEPT;

};