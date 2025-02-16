#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//STL.
#include <iomanip>
#include <sstream>

DISABLE_WARNING(4996) //Disable warning C4996: 'localtime': This function or variable may be unsafe. Consider using localtime_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.

namespace TimeUtilities
{

	/*
	*	Prints a time string with hours, minutes and seconds, given a seconds value.
	*/
	template <uint64 BUFFER_SIZE>
	FORCE_INLINE void PrintTimeString(float64 seconds, char (&buffer)[BUFFER_SIZE]) NOEXCEPT
	{
		uint32 total_hours{ 0 };
		uint32 total_minutes{ 0 };

		while (seconds >= 60.0)
		{
			++total_minutes;
			seconds -= 60.0;
		}

		while (total_minutes >= 60)
		{
			++total_hours;
			total_minutes -= 60;
		}

		sprintf_s(buffer, "%u hours, %u minutes, %f seconds", total_hours, total_minutes, seconds);
	}

	/*
	*	Returns the current date and time as a string in the format YEAR-MONTH-DAY HOUR.MINUTE.SECOND
	*/
	FORCE_INLINE void GetCurrentDateAndTime(DynamicString *const RESTRICT output) NOEXCEPT
	{
		const time_t time{ std::time(nullptr) };
		const tm *const RESTRICT time_info{ std::localtime(&time) };
		std::ostringstream string_stream;
		string_stream << std::put_time(time_info, "%Y-%m-%d %H.%M.%S");

		*output = string_stream.str().c_str();
	}

}