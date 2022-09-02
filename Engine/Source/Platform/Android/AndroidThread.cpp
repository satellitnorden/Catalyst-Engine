#if defined(CATALYST_PLATFORM_ANDROID)
//Header file.
#include <Concurrency/Thread.h>

//Platform.
#include <Platform/Android/AndroidUtilities.h>

/*
*	Android thread data class definition.
*/
class AndroidThreadData final
{

public:

	//The function.
	Thread::Function _Function;

	//The priority.
	Thread::Priority _Priority;

	//The thread.
	std::thread *RESTRICT _Thread;

};

//Android thread logic.
namespace AndroidThreadLogic
{

	/*
	*	The thread function.
	*/
	void ThreadFunction(AndroidThreadData *const RESTRICT thread_data) NOEXCEPT
	{
		/*
        *   Set the thread priority.
        *   The priorities are based on Linux priority levels, with the highest being -20 and lowest 19.
        *   Might need further tweaking so the lowest priority truly is the lowest, but this feels good right now. (:
        */
        switch (thread_data->_Priority)
        {
            case Thread::Priority::LOWEST:
            {
                AndroidUtilities::SetThreaPriority(0);

                break;
            }

            case Thread::Priority::BELOW_NORMAL:
            {
                AndroidUtilities::SetThreaPriority(-5);

                break;
            }

            case Thread::Priority::NORMAL:
            {
                AndroidUtilities::SetThreaPriority(-10);

                break;
            }

            case Thread::Priority::ABOVE_NORMAL:
            {
                AndroidUtilities::SetThreaPriority(-15);

                break;
            }

            case Thread::Priority::HIGHEST:
            {
                AndroidUtilities::SetThreaPriority(-20);

                break;
            }

            default:
            {
                ASSERT(false, "Invalid case!");
            }
        }

		//Launch the function.
		thread_data->_Function();
	}

}

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

	//Allocate the thread data.
	_PlatformData = new AndroidThreadData();

	//Set the function.
	static_cast<AndroidThreadData *const RESTRICT>(_PlatformData)->_Function = _Function;

	//Set the priority.
	static_cast<AndroidThreadData *const RESTRICT>(_PlatformData)->_Priority = _Priority;

	//Launch the thread!
	static_cast<AndroidThreadData *const RESTRICT>(_PlatformData)->_Thread = new std::thread(AndroidThreadLogic::ThreadFunction, static_cast<AndroidThreadData *const RESTRICT>(_PlatformData));
}

/*
*	Joins this thread.
*	This is a blocking call, will wait for the thread to become joinable.
*/
void Thread::Join() NOEXCEPT
{
	ASSERT(_PlatformData, "Trying to join a thread that hasn't been launched yet!");

	//Join the thread.
	static_cast<AndroidThreadData *const RESTRICT>(_PlatformData)->_Thread->join();

	//Free the thread.
	delete static_cast<AndroidThreadData *const RESTRICT>(_PlatformData)->_Thread;

	//Free the thread data.
	delete static_cast<AndroidThreadData *const RESTRICT>(_PlatformData);
}
#endif