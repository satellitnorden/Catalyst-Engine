#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

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
	TaskSystem() NOEXCEPT = default;

	/*
	*	Initializes the task system.
	*/
	void InitializeSystem(const CatalystProjectMultithreadingInformation &multithreadingInformation) NOEXCEPT;

	/*
	*	Releases the task system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Returns the number of task executors.
	*/
	uint32 GetNumberOfTaskExecutors() const NOEXCEPT { return numberOfTaskExecutors; }

	/*
	*	Executes a task. If a valid semaphore is passed in, it will be automatically reset.
	*/
	void ExecuteTask(Task *const RESTRICT newTask) NOEXCEPT;

private:

	//Defines whether or not task executors should execute tasks.
	bool executeTasks{ true };

	//The atomic queue in which to put tasks.
	AtomicQueue<Task *RESTRICT> taskQueue;

	//The number of task executors.
	uint32 numberOfTaskExecutors;

	//Container for all task executor threads.
	DynamicArray<std::thread> taskExecutorThreads;

	//The number of concurrently executing tasks.
	std::atomic<uint32> concurrentlyExecutingTasks;

	/*
	*	Executes a task executor.
	*/
	void ExecuteTaskExecutor() NOEXCEPT;

};