#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Multithreading.
#include <Multithreading/Semaphore.h>

//Type aliases.
using TaskFunction = void(*)(void *const RESTRICT);

class Task final
{

public:

	//The function that this task will exeucte.
	TaskFunction _Function;

	//The arguments that will be sent to the function that this task will execute.
	void *RESTRICT _Arguments;

	//The semaphore that will be signalled after the function that this task will execute has finished executing.
	Semaphore _Semaphore{ SemaphoreCreationFlags::Signalled };

	/*
	*	Default constructor.
	*/
	Task() NOEXCEPT { }

	/*
	*	Constructor taking all values as arguments.
	*
	*	'initialFunction' - The function that this task will exeucte.
	*	'initialArguments' - The arguments that will be sent to the function that this task will execute.
	*	'initialSemaphore' - The semaphore that will be signalled after the function that this task will execute has finished executing.
	*/
	Task(const TaskFunction initialFunction, void *const RESTRICT initialArguments) NOEXCEPT
		:
		_Function(initialFunction),
		_Arguments(initialArguments)
	{

	}

	/*
	*	Copy constructor.
	*/
	Task(const Task &otherTask) NOEXCEPT
		:
		_Function(otherTask._Function),
		_Arguments(otherTask._Arguments)
	{

	}

	/*
	*	Executes this task.
	*/
	void Execute() NOEXCEPT
	{
		//Execute the function.
		_Function(_Arguments);

		//Signal the semaphore.
		_Semaphore.Signal();
	}

	/*
	*	Returns whether or not this task has finished executing.
	*/
	bool IsExecuted() const NOEXCEPT { return _Semaphore.IsSignalled(); }

	/*
	*	Waits for this task to finish executing.
	*/
	void WaitFor() const NOEXCEPT { _Semaphore.WaitFor(); }

};