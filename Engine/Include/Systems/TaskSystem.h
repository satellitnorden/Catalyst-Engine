#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/AtomicQueue.h>

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
	TaskSystem() NOEXCEPT { }

	/*
	*	Initializes the task system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Releases the task system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Returns whether or not tasks should be executed.
	*/
	bool ExecuteTasks() const NOEXCEPT { return _ExecuteTasks; }

	/*
	*	Executes a task.
	*/
	void ExecuteTask(Task *const RESTRICT newTask) NOEXCEPT;

private:

	//The maximum number of tasks.
	static constexpr uint64 MAXIMUM_NUMBER_OF_TASKS{ 64 };

	//Denotes whether or not tasks should be executed.
	bool _ExecuteTasks{ true };

	//The number of task executors.
	uint32 _NumberOfTaskExecutors;

	//Container for all task executor threads.
	DynamicArray<std::thread> _TaskExecutorThreads;

	//Container for all atomic queues in which to put tasks in.
	DynamicArray<AtomicQueue<Task *RESTRICT, MAXIMUM_NUMBER_OF_TASKS>> _TaskQueues;

};