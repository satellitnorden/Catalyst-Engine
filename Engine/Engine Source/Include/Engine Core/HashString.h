#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Algorithms.
#include <Engine Core/Algorithms/HashAlgorithms.h>

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
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	constexpr HashString() NOEXCEPT = delete;

	/*
	*	Constructor taking a string.
	*/
	constexpr HashString(const char *RESTRICT string) NOEXCEPT
		:
#if defined(HASHING_ALGORITHM_CATALYST)
		hash(HashAlgorithms::CatalystHash(string, CompileTimeUtilities::StringLength(string)))
#elif defined(HASHING_ALGORITHM_DJB2)
		hash(HashAlgorithms::DJB2Hash(string, CompileTimeUtilities::StringLength(string)))
#elif defined(HASHING_ALGORITHM_JENKINS)
		hash(HashAlgorithms::JenkinsHash(string, CompileTimeUtilities::StringLength(string)))
#endif
	{

	}

	/*
	*	Constructor taking an already hashed value.
	*/
	constexpr HashString(const uint64 initialHash) NOEXCEPT
		:
		hash(initialHash)
	{

	}

	/*
	*	Less than operator overload.
	*/
	constexpr bool operator<(const HashString otherHashString) const NOEXCEPT
	{
		return this->hash < otherHashString.hash;
	}

private:

	//The underlying hash.
	uint64 hash;

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