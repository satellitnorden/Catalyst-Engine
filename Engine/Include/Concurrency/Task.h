#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/AtomicFlag.h>

//Type aliases.
using TaskFunction = void(*)(void *const RESTRICT /*arguments*/);

class Task final
{

public:

	//The function that this task will exeucte.
	TaskFunction _Function;

	//The arguments that will be sent to the function that this task will execute.
	void *RESTRICT _Arguments;

	//Denotes if this task is executable on the same thread as it was queued from.
	bool _ExecutableOnSameThread;

	//Atomic flag denoting if this task is executed.
	AtomicFlag _IsExecuted{ true };

	/*
	*	Default constructor.
	*/
	Task() NOEXCEPT
	{
	
	}

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

		//Set the atomic flag denoting whether or not this task is executed.
		_IsExecuted.Set();
	}

	/*
	*	Returns whether or not this task has finished executing.
	*/
	bool IsExecuted() const NOEXCEPT
	{
		return _IsExecuted.IsSet();
	}

	/*
	*	Waits for this task to finish executing.
	*/
	template <WaitMode MODE>
	void Wait() const NOEXCEPT
	{
		_IsExecuted.Wait<MODE>();
	}

};