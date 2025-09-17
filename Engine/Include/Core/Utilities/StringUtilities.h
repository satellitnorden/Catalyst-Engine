#pragma once

class StringUtilities
{

public:

	/*
	*	Returns if string A contains string B.
	*/
	FORCE_INLINE static NO_DISCARD bool Contains(const char *const RESTRICT A, const char *const RESTRICT B) NOEXCEPT
	{
		const uint64 A_length{ StringLength(A) };
		const uint64 B_length{ StringLength(B) };

		uint64 count{ 0 };

		for (uint64 i{ 0 }; i < A_length; ++i)
		{
			if (A[i] == B[count])
			{
				++count;

				if (count == B_length)
				{
					return true;
				}
			}

			else
			{
				count = 0;
			}
		}

		return false;
	}

	/*
	*	Returns the full name from a full file path.
	*/
	FORCE_INLINE RESTRICTED static constexpr NO_DISCARD const char *const RESTRICT GetFileNameFromFilePath(const char* const RESTRICT file_path) NOEXCEPT
	{
		const uint64 length{ StringLength(file_path) };

		for (int64 i{ static_cast<int64>(length) - 1 }; i >= 0; --i)
		{
			if (file_path[i] == '\\')
			{
				return &file_path[i + 1];
			}
		}

		return file_path;
	}

	/*
	*	Returns if the two provided strings are equal.
	*/
	FORCE_INLINE static constexpr NO_DISCARD bool IsEqual(const char *const RESTRICT first, const char *const RESTRICT second, const uint64 length = UINT64_MAXIMUM) NOEXCEPT
	{
		const uint64 first_length{ length != UINT64_MAXIMUM ? length : StringLength(first) };
		const uint64 second_length{ length != UINT64_MAXIMUM ? length : StringLength(second) };

		if (first_length != second_length)
		{
			return false;
		}

		for (uint64 i{ 0 }; i < first_length; ++i)
		{
			if (first[i] != second[i])
			{
				return false;
			}
		}

		return true;
	}

	/*
	*	Returns if the given character is a number.
	*/
	FORCE_INLINE static constexpr NO_DISCARD bool IsNumber(const char character) NOEXCEPT
	{
		return character >= '0' && character <= '9';
	}

	/*
	*	Returns the length of the provided string.
	*/
	FORCE_INLINE static constexpr NO_DISCARD uint64 StringLength(const char *RESTRICT string) NOEXCEPT
	{
		uint64 length{ 0 };

		while (*string++)
		{
			++length;
		}

		return length;
	}

	/*
	*	Converts a string to all uppercase.
	*/
	FORCE_INLINE static constexpr void ToUpper(char *const RESTRICT string) NOEXCEPT
	{
		ToUpper(string, StringLength(string));
	}

	/*
	*	Converts a string to all uppercase.
	*/
	FORCE_INLINE static constexpr void ToUpper(char *const RESTRICT string, const uint64 length) NOEXCEPT
	{
		for (uint64 i{ 0 }; i < length; ++i)
		{
			string[i] -= 32 * static_cast<char>(string[i] >= 'a' && string[i] <= 'z');
		}
	}

};