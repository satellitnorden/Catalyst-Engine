#if defined(CATALYST_PLATFORM_OCULUS_QUEST)
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
	ASSERT(_Function, "Need to set a function to launch this thread!");

	//Allocate an std::thread with the function.
	_PlatformData = new std::thread(_Function);
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