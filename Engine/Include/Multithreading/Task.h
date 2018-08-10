#pragma once

//Core.
#include <Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Semaphore.h>

//Type aliases.
using TaskFunction = void(*)(void *const RESTRICT);

class Task final
{

public:

	//The function that this task will exeucte.
	TaskFunction function;

	//The arguments that will be sent to the function that this task will execute.
	void *RESTRICT arguments;

	//The semaphore that will be signalled after the function that this task will execute has finished executing.
	Semaphore semaphore{ SemaphoreCreationFlags::Signalled };

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
		function(initialFunction),
		arguments(initialArguments)
	{

	}

	/*
	*	Executes this task.
	*/
	void Execute() NOEXCEPT
	{
		//Execute the function.
		function(arguments);

		//Signal the semaphore.
		semaphore.Signal();
	}

	/*
	*	Waits for this task to finish.
	*/
	void WaitFor() const NOEXCEPT { semaphore.WaitFor(); }

};