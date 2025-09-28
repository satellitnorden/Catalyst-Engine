#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Utilities/StringUtilities.h>

//Systems.
#include <Systems/System.h>

//Macros.
#define LOG_DEBUG(MESSAGE, ...) LogSystem::Instance->Log(LogSystem::LogLevel::DEBUG, StringUtilities::GetFileNameFromFilePath(__FILE__), __func__, __LINE__, MESSAGE, __VA_ARGS__)
#define LOG_INFORMATION(MESSAGE, ...) LogSystem::Instance->Log(LogSystem::LogLevel::INFORMATION, StringUtilities::GetFileNameFromFilePath(__FILE__), __func__, __LINE__, MESSAGE, __VA_ARGS__)
#define LOG_WARNING(MESSAGE, ...) LogSystem::Instance->Log(LogSystem::LogLevel::WARNING, StringUtilities::GetFileNameFromFilePath(__FILE__), __func__, __LINE__, MESSAGE, __VA_ARGS__)
#define LOG_ERROR(MESSAGE, ...) LogSystem::Instance->Log(LogSystem::LogLevel::ERROR, StringUtilities::GetFileNameFromFilePath(__FILE__), __func__, __LINE__, MESSAGE, __VA_ARGS__)
#define LOG_FATAL(MESSAGE, ...) LogSystem::Instance->Log(LogSystem::LogLevel::FATAL, StringUtilities::GetFileNameFromFilePath(__FILE__), __func__, __LINE__, MESSAGE, __VA_ARGS__)

class LogSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		LogSystem,
		SYSTEM_INITIALIZE()
	);

	//Enumeration covering all log levels.
	enum class LogLevel : uint8
	{
		DEBUG,
		INFORMATION,
		WARNING,
		ERROR,
		FATAL
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LogSystem() NOEXCEPT
	{

	}

	/*
	*	Logs.
	*/
	void Log(	const LogLevel log_level,
				const char *const RESTRICT file,
				const char *const RESTRICT function,
				const int32 line,
				const char *const RESTRICT format,
				...) NOEXCEPT;

	/*
	*	Flushes the current logs.
	*/
	void Flush() NOEXCEPT;

};