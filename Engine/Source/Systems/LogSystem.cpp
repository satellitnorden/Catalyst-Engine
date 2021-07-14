//Header file.
#include <Systems/LogSystem.h>

//Core.
#include <Core/Containers/StaticArray.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>
#include <Concurrency/SpinLock.h>
#include <Concurrency/Task.h>

//File.
#include <File/Core/FileCore.h>

//Memory.
#include <Memory/LinearAllocator.h>

//Systems.
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(LogSystem);

//Log system constants.
namespace LogSystemConstants
{
	constexpr uint64 MAXIMUM_LINE_LENGTH{ 512 };
}

/*
*	Queued lines class definition.
*/
class QueuedLines final
{

public:

	//The linear allocator.
	LinearAllocator<LogSystemConstants::MAXIMUM_LINE_LENGTH * 2'048> _Allocator; //~1mb

	//The current line index.
	uint64 _CurrentLineIndex{ 0 };

	//The lines.
	StaticArray<const char *RESTRICT, 1'024> _Lines;

	//The flush task.
	Task _FlushTask;

};

//Log system data.
namespace LogSystemData
{

	//The lock.
	Spinlock _Lock;

	//The temporary message buffer.
	char _TemporaryMessageBuffer[LogSystemConstants::MAXIMUM_LINE_LENGTH];

	//The temporary line buffer.
	char _TemporaryLineBuffer[LogSystemConstants::MAXIMUM_LINE_LENGTH];

	//The current buffer index.
	uint8 _CurrentBufferIndex{ 0 };

	//The queued lines.
	StaticArray<QueuedLines, 2> _QueuedLines;

}

//Log system logic.
namespace LogSystemLogic
{

	/*
	*	Returns the string for the given log level.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT GetLogLevelString(const LogSystem::LogLevel log_level) NOEXCEPT
	{
		switch (log_level)
		{
			case LogSystem::LogLevel::DEBUG:
			{
				return "DEBUG";
			}

			case LogSystem::LogLevel::INFORMATION:
			{
				return "INFORMATION";
			}

			case LogSystem::LogLevel::WARNING:
			{
				return "WARNING";
			}

			case LogSystem::LogLevel::ERROR:
			{
				return "ERROR";
			}

			case LogSystem::LogLevel::FATAL:
			{
				return "FATAL";
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return "";
			}
		}
	}

	/*
	*	Flushes the given queued lines.B
	*/
	FORCE_INLINE void FlushQueuedLines(const uint8 index) NOEXCEPT
	{
		//Cache the queued lines.
		QueuedLines &queued_lines{ LogSystemData::_QueuedLines[index] };

		//Write all the lines.
		std::ofstream log_file{ "Catalyst Engine Log.txt", std::ofstream::app };

		for (uint64 i{ 0 }; i < queued_lines._CurrentLineIndex; ++i)
		{
			log_file << queued_lines._Lines[i] << std::endl;
		}

		log_file.close();

		//Reset the allocator.
		queued_lines._Allocator.Reset();

		//Reset the current line index.
		queued_lines._CurrentLineIndex = 0;
	}

}

/*
*	Initializes the log system.
*/
void LogSystem::Initialize() NOEXCEPT
{
	//Create the log file.
	std::ofstream log_file{ "Catalyst Engine Log.txt" };

	log_file << "Catalyst Engine Log Start" << std::endl;

	log_file.close();

	//Set up the flush tasks.
	for (uint8 i{ 0 }; i < 2; ++i)
	{
		QueuedLines &queued_lines{ LogSystemData::_QueuedLines[i] };

		switch (i)
		{
			case 0:
			{
				queued_lines._FlushTask._Function = [](void *const RESTRICT)
				{
					LogSystemLogic::FlushQueuedLines(0);
				};

				break;
			}

			case 1:
			{
				queued_lines._FlushTask._Function = [](void *const RESTRICT)
				{
					LogSystemLogic::FlushQueuedLines(1);
				};

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		queued_lines._FlushTask._Arguments = nullptr;
		queued_lines._FlushTask._ExecutableOnSameThread = false;
	}
}

/*
*	Logs.
*/
void LogSystem::Log(const LogLevel log_level,
					const char *const RESTRICT file,
					const char *const RESTRICT function,
					const int32 line,
					const char *const RESTRICT format,
					...) NOEXCEPT
{
	//Lock the lock.
	SCOPED_LOCK(LogSystemData::_Lock);

	//Print the line.
	va_list variadic_arguments;
	va_start(variadic_arguments, format);
	vsprintf_s(LogSystemData::_TemporaryMessageBuffer, format, variadic_arguments);
	va_end(variadic_arguments);
	sprintf_s(LogSystemData::_TemporaryLineBuffer, "[%s - %s - %s - Line %i] - %s", LogSystemLogic::GetLogLevelString(log_level), file, function, line, LogSystemData::_TemporaryMessageBuffer);
	
	//Add the queued line.
	QueuedLines &current_queued_lines{ LogSystemData::_QueuedLines[LogSystemData::_CurrentBufferIndex] };
	const char *const RESTRICT new_line{ static_cast<const char *const RESTRICT>(current_queued_lines._Allocator.Allocate(LogSystemConstants::MAXIMUM_LINE_LENGTH)) };
	Memory::Copy(new_line, LogSystemData::_TemporaryLineBuffer, LogSystemConstants::MAXIMUM_LINE_LENGTH);
	current_queued_lines._Lines[current_queued_lines._CurrentLineIndex++] = new_line;

	//Should the queued lines be flushed?
	if (current_queued_lines._CurrentLineIndex == current_queued_lines._Lines.Size())
	{
		//Execute the flush task.
		TaskSystem::Instance->ExecuteTask(&current_queued_lines._FlushTask);

		//Flip the current buffer index.
		LogSystemData::_CurrentBufferIndex ^= 1;

		//Wait for the flush task to finish.
		LogSystemData::_QueuedLines[LogSystemData::_CurrentBufferIndex]._FlushTask.Wait<WaitMode::PAUSE>();
	}
}

/*
*	Flushes the current logs.
*/
void LogSystem::Flush() NOEXCEPT
{
	//Lock the lock.
	SCOPED_LOCK(LogSystemData::_Lock);

	//Flush the current buffer.
	LogSystemLogic::FlushQueuedLines(LogSystemData::_CurrentBufferIndex);
}