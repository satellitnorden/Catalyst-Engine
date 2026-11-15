#if defined(CATALYST_PLATFORM_LINUX)
//Header file.
#include <Concurrency/Thread.h>

//STL.
#include <thread>

/*
*	Default constructor.
*/
Thread::Thread() NOEXCEPT
{

}

/*
*	Default destructor.
*/
Thread::~Thread() NOEXCEPT
{

}

/*
*	Sets the function.
*/
void Thread::SetFunction(const Function function) NOEXCEPT
{
	_Function = function;
}

/*
*	Sets the function with arguments.
*/
void Thread::SetFunctionWithArguments(const FunctionWithArguments function_with_arguments, void *const RESTRICT arguments) NOEXCEPT
{
	//Set the function with arguments.
	_FunctionWithArguments = function_with_arguments;

	//Set the arguments.
	_Arguments = arguments;
}

/*
*	Sets the priority of this thread.
*/
void Thread::SetPriority(const Priority priority) NOEXCEPT
{
	_Priority = priority;
}

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Sets the name of this thread.
*/
void Thread::SetName(const DynamicString &name) NOEXCEPT
{
	_Name = name;
}
#endif

/*
*	Launches this thread.
*/
void Thread::Launch() NOEXCEPT
{
	//Allocate an std::thread with the function.
	if (_Function)
	{
		_PlatformData = new std::thread(_Function);
	}

	else if (_FunctionWithArguments)
	{
		ASSERT(_Arguments, "Set function with arguments without arguments!");
		_PlatformData = new std::thread(_FunctionWithArguments, _Arguments);
	}

	else
	{
		ASSERT(false, "Need to set either function or function with arguments before launching!");
	}

	//Retrieve the native handle.
	std::thread::native_handle_type native_handle{ static_cast<std::thread *const RESTRICT>(_PlatformData)->native_handle() };

	//TODO: Set the thread priority.
	{
		
	}

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//TODO: Set the thread name.
	if (_Name)
	{
		
	}
#endif
}

/*
*	Joins this thread.
*	This is a blocking call, will wait for the thread to become joinable.
*/
void Thread::Join() NOEXCEPT
{
	ASSERT(_PlatformData, "Trying to join a thread that hasn't been launched yet!");

	static_cast<std::thread *const RESTRICT>(_PlatformData)->join();
}
#endif