#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Concurrency/Thread.h>

//Platform.
#include <Platform/Windows/CatalystPlatformWindows.h>

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

	//Set the thread priority.
	{
		int32 windows_priority{ THREAD_PRIORITY_NORMAL };

		switch (_Priority)
		{
			case Priority::LOWEST:
			{
				windows_priority = THREAD_PRIORITY_LOWEST;

				break;
			}

			case Priority::BELOW_NORMAL:
			{
				windows_priority = THREAD_PRIORITY_BELOW_NORMAL;

				break;
			}

			case Priority::NORMAL:
			{
				windows_priority = THREAD_PRIORITY_NORMAL;

				break;
			}

			case Priority::ABOVE_NORMAL:
			{
				windows_priority = THREAD_PRIORITY_ABOVE_NORMAL;

				break;
			}

			case Priority::HIGHEST:
			{
				windows_priority = THREAD_PRIORITY_HIGHEST;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		const HRESULT result{ SetThreadPriority(native_handle, windows_priority) };

		ASSERT(SUCCEEDED(result), "Couldn't set thread priority!");
	}

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Set the thread name.
	{
		const std::string string_name{ _Name.Data() };
		const std::wstring name{ string_name.begin(), string_name.end() };

		const HRESULT result{ SetThreadDescription(native_handle, name.c_str()) };

		ASSERT(SUCCEEDED(result), "Couldn't set thread description!");
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