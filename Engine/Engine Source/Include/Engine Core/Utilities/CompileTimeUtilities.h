#pragma once

namespace CompileTimeUtilities
{

	static constexpr uint64 StringLength(const char *RESTRICT string)
	{
		uint64 length{ 0 };

		while (*string++)
		{
			++length;
		}

		return length;
	}
}