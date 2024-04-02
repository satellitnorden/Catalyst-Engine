#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//STL.
#include <iomanip>

DISABLE_WARNING(4996) //Disable warning C4996: 'localtime': This function or variable may be unsafe. Consider using localtime_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.

namespace TimeUtilities
{

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