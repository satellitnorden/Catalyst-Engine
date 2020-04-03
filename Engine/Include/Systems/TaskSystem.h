#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/Atomic.h>
#include <Concurrency/AtomicQueue.h>
#include <Concurrency/Thread.h>

//Forward declarations.
class Task;

class TaskSystem final
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
	void Initialize() NOEXCEPT;

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
	void ExecuteTask(Task *const RESTRICT task) NOEXCEPT;

private:

	//The maximum number of tasks.
	static constexpr uint64 MAXIMUM_NUMBER_OF_TASKS{ 1'024 };

	//Denotes whether or not tasks should be executed.
	bool _ExecuteTasks{ true };

	//The number of task executors.
	uint32 _NumberOfTaskExecutors;

	//Container for all task executor threads.
	DynamicArray<Thread> _TaskExecutorThreads;

	//Container for all atomic queues in which to put tasks in.
	AtomicQueue<Task *RESTRICT, MAXIMUM_NUMBER_OF_TASKS> _TaskQueue;

	//Denotes how many tasks are currently queued.
	Atomic<uint64> _TasksInQueue{ 0 };

	/*
	*	Executes tasks.
	*/
	void ExecuteTasks() NOEXCEPT;

};