#pragma once

class StringUtilities
{

public:

	/*
	*	Returns if the two provided strings are equal.
	*/
	FORCE_INLINE static constexpr NO_DISCARD bool IsEqual(const char *const RESTRICT first, const char *const RESTRICT second) NOEXCEPT
	{
		const uint64 first_length{ StringLength(first) };
		const uint64 second_length{ StringLength(second) };

		const uint64 length{ first_length < second_length ? first_length : second_length };

		for (uint64 i{ 0 }; i < length; ++i)
		{
			if (first[i] != second[i])
			{
				return false;
			}
		}

		return true;
	}

	/*
	*	Returns if the two provided strings are equal. Will only compare the first 'length' characters.
	*/
	FORCE_INLINE static constexpr NO_DISCARD bool IsEqual(const char *const RESTRICT first, const char *const RESTRICT second, const uint64 length) NOEXCEPT
	{
		for (uint64 i{ 0 }; i < length; ++i)
		{
			if (first[i] != second[i])
			{
				return false;
			}
		}

		return true;
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

};