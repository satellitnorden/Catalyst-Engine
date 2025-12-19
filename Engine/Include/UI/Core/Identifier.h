#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Algorithms/HashAlgorithms.h>
#include <Core/Utilities/StringUtilities.h>

namespace UI
{

	/*
	*	Represents a unique identifier for a UI object.
	*	Can be constructed from strings, and also combined with other identifiers to create unique identifiers.
	*/
	class Identifier final
	{
		
	public:

		/*
		*	Default constructor.
		*/
		FORCE_INLINE constexpr Identifier() NOEXCEPT
			:
			_Value(0)
		{

		}

		/*
		*	Constructor taking the value.
		*/
		FORCE_INLINE constexpr Identifier(const uint64 value) NOEXCEPT
			:
			_Value(value)
		{

		}

		/*
		*	Constructor from a string.
		*/
		FORCE_INLINE constexpr Identifier(const char *const RESTRICT string) NOEXCEPT
			:
			_Value(HashAlgorithms::MurmurHash64(string, StringUtilities::StringLength(string)))
		{

		}

		/*
		*	Constructor from a string and an index.
		*/
		FORCE_INLINE constexpr Identifier(const char *const RESTRICT string, const uint64 index) NOEXCEPT
			:
			_Value(HashAlgorithms::MurmurHash64(string, StringUtilities::StringLength(string), index))
		{

		}

		/*
		*	Constructor from two identifiers.
		*/
		FORCE_INLINE constexpr Identifier(const Identifier A, const Identifier B) NOEXCEPT
			:
			_Value(HashAlgorithms::MurmurHash64(reinterpret_cast<const char *const RESTRICT>(&A._Value), sizeof(uint64), B._Value))
		{

		}

		/*
		*	Equality operator overload.
		*/
		FORCE_INLINE constexpr NO_DISCARD bool operator==(const Identifier other) NOEXCEPT
		{
			return _Value == other._Value;
		}

	private:

		//The value.
		uint64 _Value;

	};

}
