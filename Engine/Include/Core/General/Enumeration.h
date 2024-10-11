#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class Enumeration
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Enumeration(const uint64 identifier) NOEXCEPT
		:
		_Identifier(identifier)
	{

	}

	/*
	*	Returns the size of this enumeration.
	*/
	NO_DISCARD uint64 Size() const NOEXCEPT;

	/*
	*	Sets this enumeration from the given index.
	*/
	void SetFromIndex(const uint64 index) NOEXCEPT;

	/*
	*	Sets this enumeration from the given hash.
	*/
	void SetFromHash(const uint64 hash) NOEXCEPT;

	/*
	*	Returns the string for this enumeration.
	*/
	NO_DISCARD const char *const RESTRICT ToString() const NOEXCEPT;

	/*
	*	Returns the string for the given index.
	*/
	NO_DISCARD const char *const RESTRICT IndexToString(const uint64 index) const NOEXCEPT;

private:

	//The identifier.
	uint64 _Identifier{ 0 };

};

/*
*	This defines an enumeration.
*	The code generation pass will put the definition for this enumeration in Generated/Enumeration.Generated.cpp.
*/
#define CATALYST_ENUMERATION(NAME, TYPE, ...)															\
class NAME final : public Enumeration																	\
{																										\
public:																									\
	enum _VALUES : TYPE																					\
	{																									\
		__VA_ARGS__																						\
	};																									\
	NAME() NOEXCEPT;																					\
	NAME(const TYPE value) NOEXCEPT;																	\
	operator TYPE() const NOEXCEPT;																		\
private:																								\
	TYPE _VALUE;																						\
};