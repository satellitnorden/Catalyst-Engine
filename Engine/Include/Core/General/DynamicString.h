#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Utilities/StringUtilities.h>

class DynamicString final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE DynamicString() NOEXCEPT
		:
		_String(nullptr),
		_Length(0)
	{

	}

	/*
	*	Copy constructor.
	*/
	FORCE_INLINE DynamicString(const DynamicString &other) NOEXCEPT
	{
		if (other._String)
		{
			//Set the length of this string.
			_Length = other.Length();

			//Allocate sufficient memory for the underlying string.
			_String = static_cast<char *RESTRICT>(Memory::Allocate(_Length + 1));

			//Copy the contents of the other string.
			Memory::Copy(_String, other._String, _Length + 1);
		}

		else
		{
			_String = nullptr;
			_Length = 0;
		}
	}

	/*
	*	Move constructor.
	*/
	FORCE_INLINE DynamicString(DynamicString &&other) NOEXCEPT
	{
		//Steal the other string's underlying string and set it to nullptr!
		_String = other._String;
		other._String = nullptr;

		//Steal the other string's length!
		_Length = other._Length;
	}

	/*
	*	Constructor taking a C string.
	*/
	FORCE_INLINE DynamicString(const char *const RESTRICT new_string) NOEXCEPT
	{
		//Determine how long the C string is.
		_Length = StringUtilities::StringLength(new_string);

		//Allocate sufficient memory to host the string.
		_String = static_cast<char *RESTRICT>(Memory::Allocate(_Length + 1));

		//Copy the string to the memory.
		Memory::Copy(static_cast<void *RESTRICT>(_String), static_cast<const void *const RESTRICT>(new_string), _Length + 1);
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~DynamicString() NOEXCEPT
	{
		//Free the underlying string.
		if (_String)
		{
			Memory::Free(_String);
		}
	}

	/*
	*	Bool operator overload.
	*/
	FORCE_INLINE constexpr operator bool() const NOEXCEPT
	{
		return _String != nullptr;
	}

	/*
	*	Copy operator overload taking another dynamic string.
	*/
	FORCE_INLINE void operator=(const DynamicString &other) NOEXCEPT
	{
		if (other._String)
		{
			//Update the length of this string.
			_Length = other.Length();

			//Reallocate sufficient memory for the underlying string.
			_String = static_cast<char *RESTRICT>(Memory::Reallocate(static_cast<void *RESTRICT>(_String), _Length + 1));

			//Copy the contents of the other string.
			Memory::Copy(_String, other._String, _Length + 1);
		}

		else
		{
			//Free the underlying string.
			Memory::Free(_String);

			//Reset the string.
			_String = nullptr;

			//Reset the length.
			_Length = 0;
		}
	}

	/*
	*	Copy operator overload taking a C string.
	*/
	FORCE_INLINE void operator=(const char *const RESTRICT new_string) NOEXCEPT
	{
		//Check for nullptr.
		if (new_string)
		{
			//Calculate the length of the new string.
			const uint64 new_string_length{ StringUtilities::StringLength(new_string) };

			//It's possible the string hasn't been constructed yet - if so, allocate the required memory.
			if (!_String)
			{
				_String = static_cast<char *RESTRICT>(Memory::Allocate(new_string_length + 1));
				Memory::Copy(_String, new_string, new_string_length + 1);

				_Length = new_string_length;
			}

			else
			{
				//If the current length matches the new string's length, just copy the data over directly.
				if (_Length == new_string_length)
				{
					Memory::Copy(_String, new_string, _Length + 1);
				}

				//Otherwise, allocate sufficient data to hold the new string.
				else
				{
					_String = static_cast<char *RESTRICT>(Memory::Reallocate(static_cast<void *RESTRICT>(_String), new_string_length + 1));
					Memory::Copy(_String, new_string, new_string_length + 1);

					_Length = new_string_length;
				}
			}
		}

		else
		{
			//Since the new string is nullptr, free the current string.
			if (_String)
			{
				Memory::Free(_String);

				_Length = 0;
			}
		}
	}

	/*
	*	Move operator overload taking another dynamic string.
	*/
	FORCE_INLINE void operator=(DynamicString &&other) NOEXCEPT
	{
		//Steal the other string's underlying string and set it to nullptr!
		_String = other._String;
		other._String = nullptr;

		//Steal the other string's length!
		_Length = other._Length;
	}

	/*
	*	Addition operator overload for C-style strings.
	*/
	FORCE_INLINE NO_DISCARD DynamicString operator+(const char *const RESTRICT new_string) const NOEXCEPT
	{
		//Construct the new dynamic string.
		DynamicString new_dynamic_string;

		//Determine how long the new dynamic string will be.
		uint64 new_string_length{ StringUtilities::StringLength(new_string) };
		uint64 new_length{ this->_Length + new_string_length };

		//Allocate sufficient memory to host the string.
		new_dynamic_string._String = static_cast<char *RESTRICT>(Memory::Allocate(new_length + 1));

		//Copy the original string to the memory.
		Memory::Copy(static_cast<void *RESTRICT>(new_dynamic_string._String), static_cast<const void *const RESTRICT>(this->_String), this->_Length);

		//Copy the new string to the memory.
		Memory::Copy(static_cast<void *RESTRICT>(new_dynamic_string._String + this->_Length), static_cast<const void *const RESTRICT>(new_string), new_string_length + 1);

		//Write the length to the new dynamic string.
		new_dynamic_string._Length = new_length;

		//Return the newly constructed dynamic string.
		return new_dynamic_string;
	}

	/*
	*	Addition assignment operator overload for C-style strings.
	*/
	FORCE_INLINE void operator+=(const char *const RESTRICT new_string) NOEXCEPT
	{
		//Calculate the new length.
		const uint64 new_string_length = StringUtilities::StringLength(new_string) + 1;
		const uint64 new_length = _Length + new_string_length - 1;

		//Allocate sufficient memory to host the concatenated string.
		_String = static_cast<char *RESTRICT>(Memory::Reallocate(static_cast<void *RESTRICT>(_String), new_length + 1));

		//Copy the new string.
		Memory::Copy(_String + _Length, new_string, new_string_length);

		//Update the length of the string.
		_Length = new_length;
	}

	/*
	*	Addition assignment operator overload for charrs.
	*/
	FORCE_INLINE void operator+=(const char new_character) NOEXCEPT
	{
		//Calculate the new length.
		const uint64 new_length = _Length + 1;

		//Allocate sufficient memory to host the concatenated string.
		if (!_String)
		{
			_String = static_cast<char *RESTRICT>(Memory::Allocate( new_length + 1));
		}

		else
		{
			_String = static_cast<char *RESTRICT>(Memory::Reallocate(static_cast<void *RESTRICT>(_String), new_length + 1));
		}

		//Copy the new string.
		Memory::Copy(_String + _Length, &new_character, sizeof(char));
		_String[_Length + 1] = '\0';

		//Update the length of the string.
		_Length = new_length;
	}

	/*
	*	Equality operator overload taking another dynamic string.
	*/
	FORCE_INLINE NO_DISCARD bool operator==(const DynamicString &other) const NOEXCEPT
	{
		return StringUtilities::IsEqual(_String, other._String);
	}

	/*
	*	Equality operator overload taking a C string.
	*/
	FORCE_INLINE NO_DISCARD bool operator==(const char *const RESTRICT string) const NOEXCEPT
	{
		return StringUtilities::IsEqual(_String, string);
	}

	/*
	*	Inequality operator overload taking another dynamic string.
	*/
	FORCE_INLINE NO_DISCARD bool operator!=(const DynamicString &other) const NOEXCEPT
	{
		return !StringUtilities::IsEqual(_String, other._String);
	}

	/*
	*	Inequality operator overload taking a C string.
	*/
	FORCE_INLINE NO_DISCARD bool operator!=(const char *const RESTRICT string) const NOEXCEPT
	{
		return !StringUtilities::IsEqual(_String, string);
	}

	/*
	*	Subscript operator overload, const.
	*/
	FORCE_INLINE NO_DISCARD const char &operator[](const uint64 index) const NOEXCEPT
	{
		return _String[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	FORCE_INLINE NO_DISCARD char &operator[](const uint64 index) NOEXCEPT
	{
		return _String[index];
	}

	/*
	*	Begin iterator, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT Begin() const  NOEXCEPT
	{
		return _String;
	}

	/*
	*	Begin iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD char *const RESTRICT Begin()  NOEXCEPT
	{
		return _String;
	}

	/*
	*	End iterator, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT End() const NOEXCEPT
	{
		return _String + _Length;
	}

	/*
	*	End iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD char *const RESTRICT End() NOEXCEPT
	{
		return _String + _Length;
	}

	/*
	*	Begin iterator, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT begin() const  NOEXCEPT
	{
		return Begin();
	}

	/*
	*	Begin iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD char *const RESTRICT begin()  NOEXCEPT
	{
		return Begin();
	}

	/*
	*	End iterator, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT end() const NOEXCEPT
	{
		return End();
	}

	/*
	*	End iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD char *const RESTRICT end() NOEXCEPT
	{
		return End();
	}

	/*
	*	Returns the underlying data, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT Data() const NOEXCEPT
	{
		return _String;
	}

	/*
	*	Returns the underlying data, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD char *const RESTRICT Data() NOEXCEPT
	{
		return _String;
	}

	/*
	*	Returns the length of the string.
	*/
	FORCE_INLINE NO_DISCARD uint64 Length() const NOEXCEPT
	{
		return _Length;
	}

	/*
	*	Sets the length of the string, reserving the required memory.
	*/
	FORCE_INLINE void SetLength(const uint64 new_length) NOEXCEPT
	{
		//Allocate the new array.
		char *const RESTRICT new_string{ static_cast<char *const RESTRICT>(Memory::Allocate(sizeof(char) * new_length + 1)) };

		//Move over the old string to the new string.
		Memory::Copy(new_string, _String, sizeof(char) * (_Length < new_length ? _Length : new_length));

		//Add the terminating character at the end.
		new_string[new_length] = '\0';

		//Free the old string
		Memory::Free(_String);

		//Update the string and the length.
		_String = new_string;
		_Length = new_length;
	}

	/*
	*	Tries to find the given substring. If it exists, a pointer to the beginning of the substring is returned, otherwise nullptr. Const version.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT Find(const char *const RESTRICT substring) const NOEXCEPT
	{
		return static_cast<const char *const RESTRICT>(const_cast<DynamicString *const RESTRICT>(this)->Find(substring));
	}

	/*
	*	Tries to find the given substring. If it exists, a pointer to the beginning of the substring is returned, otherwise nullptr. Non-const version.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD char *const RESTRICT Find(const char *const RESTRICT substring) NOEXCEPT
	{
		const uint64 substring_length{ StringUtilities::StringLength(substring) };
		uint64 matching_characters{ 0 };

		for (uint64 i{ 0 }; i < _Length; ++i)
		{
			if (_String[i] == substring[matching_characters])
			{
				++matching_characters;

				if (matching_characters == substring_length)
				{
					return &_String[i - matching_characters];
				}
			}

			else
			{
				matching_characters = 0;
			}
		}

		return nullptr;
	}

	/*
	*	Tries to find the last of the given character. If it exists, a pointer to the beginning of the character is returned, otherwise nullptr. Non-const version.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD char *const RESTRICT FindLastOfCharacter(const char character) NOEXCEPT
	{
		for (int64 i{ static_cast<int64>(_Length) - 1 }; i >= 0; --i)
		{
			if (_String[i] == character)
			{
				return &_String[i];
			}
		}

		return nullptr;
	}

private:

	//The underlying C string.
	char *RESTRICT _String;

	//The length of the string.
	uint64 _Length;

};