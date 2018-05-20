#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Semaphore.h>

//Type aliases.
using TaskFunction = void(*)(void *const RESTRICT);

class Task final
{

public:

	/*
	*	Default constructor.
	*/
	Task() NOEXCEPT = default;

	/*
	*	Constructor taking all values as arguments.
	*
	*	'initialFunction' - The function that this task will exeucte.
	*	'initialArguments' - The arguments that will be sent to the function that this task will execute.
	*	'initialSemaphore' - The semaphore that will be signalled after the function that this task will execute has finished executing.
	*/
	Task(const TaskFunction initialFunction, void *const RESTRICT initialArguments, Semaphore *const RESTRICT initialSemaphore) NOEXCEPT
		:
		function(initialFunction),
		arguments(initialArguments),
		semaphore(initialSemaphore)
	{

	}

	/*
	*	Copy assignment operator overload.
	*/
	void operator=(const Task &otherTask) NOEXCEPT
	{
		function = otherTask.function;
		arguments = otherTask.arguments;
		semaphore = otherTask.semaphore;
	}

	/*
	*	Executes this task.
	*/
	void Execute() NOEXCEPT
	{
		//Execute the function.
		function(arguments);

		//Signal the semaphore.
		semaphore->Signal();
	}

	//The function that this task will exeucte.
	TaskFunction function;

	//The arguments that will be sent to the function that this task will execute.
	void *RESTRICT arguments;

	//The semaphore that will be signalled after the function that this task will execute has finished executing.
	Semaphore *RESTRICT semaphore;

};