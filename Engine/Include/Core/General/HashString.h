#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Utilities/StringUtilities.h>

//Algorithms.
#include <Core/Algorithms/HashAlgorithms.h>

/*
*	Defines the hashing algorithm used by the HashString function.
*/
#define HASHING_ALGORITHM_CATALYST
//#define HASHING_ALGORITHM_DJB2
//#define HASHING_ALGORITHM_JENKINS

/*
*	Wrapper around an unsigned 64-bit integer hash value with an accompying hashing algorithm.
*	It is not recommended to create hash strings at runtime, which is why the class is constexpr-constructible.
*/
class HashString final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE FINAL_CONSTEXPR HashString() NOEXCEPT
		:
		_Hash(0)
	{

	}

	/*
	*	Constructor taking a string.
	*/
	FORCE_INLINE FINAL_CONSTEXPR HashString(const char *const RESTRICT string) NOEXCEPT
		:
#if defined(HASHING_ALGORITHM_CATALYST)
		_Hash(HashAlgorithms::CatalystHash(string, StringUtilities::StringLength(string)))
#elif defined(HASHING_ALGORITHM_DJB2)
		_Hash(HashAlgorithms::DJB2Hash(string, StringUtilities::StringLength(string)))
#elif defined(HASHING_ALGORITHM_JENKINS)
		_Hash(HashAlgorithms::JenkinsHash(string, StringUtilities::StringLength(string)))
#endif
	{

	}

	/*
	*	Constructor taking an already hashed value.
	*/
	FORCE_INLINE FINAL_CONSTEXPR HashString(const uint64 initialHash) NOEXCEPT
		:
		_Hash(initialHash)
	{

	}

	/*
	*	Equal operator overload.
	*/
	FORCE_INLINE FINAL_CONSTEXPR bool operator==(const HashString otherHashString) const NOEXCEPT
	{
		return this->_Hash == otherHashString._Hash;
	}

	/*
	*	Less than operator overload.
	*/
	FORCE_INLINE FINAL_CONSTEXPR bool operator<(const HashString otherHashString) const NOEXCEPT
	{
		return this->_Hash < otherHashString._Hash;
	}

	/*
	*	uint64 operator overload.
	*/
	FORCE_INLINE FINAL_CONSTEXPR operator uint64() const NOEXCEPT
	{
		return _Hash;
	}

	/*
	*	Returns if this hash string is empty.
	*/
	FORCE_INLINE NO_DISCARD bool Empty() const NOEXCEPT
	{
		static HashString EMPTY_HASH_STRING{ "" };

		return *this == EMPTY_HASH_STRING;
	}

private:

	//The underlying hash.
	uint64 _Hash;

};

//Undefine defines so that they don't leak into other files.
#if defined(HASHING_ALGORITHM_CATALYST)
	#undef HASHING_ALGORITHM_CATALYST
#endif

#if defined(HASHING_ALGORITHM_DJB2)
	#undef HASHING_ALGORITHM_DJB2
#endif

#if defined(HASHING_ALGORITHM_JENKINS)
	#undef HASHING_ALGORITHM_JENKINS
#endif