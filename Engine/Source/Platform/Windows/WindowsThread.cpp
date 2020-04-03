#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Concurrency/Thread.h>

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
*	Sets the priority of this thread.
*/
void Thread::SetPriority(const Priority priority) NOEXCEPT
{
	_Priority = priority;
}

/*
*	Launches this thread.
*/
void Thread::Launch() NOEXCEPT
{
	ASSERT(_Function, "Need to set a function to launch this thread!");

	//Allocate an std::thread with the function.
	_PlatformData = new std::thread(_Function);

	//Retrieve the native handle.
	std::thread::native_handle_type native_handle{ static_cast<std::thread* const RESTRICT>(_PlatformData)->native_handle() };

	//Set the thread priority.
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
	}

	SetThreadPriority(native_handle, windows_priority);
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