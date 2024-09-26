#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class Enumeration
{

public:

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~Enumeration() NOEXCEPT
	{

	}

	/*
	*	Returns the size of this enumeration.
	*/
	virtual NO_DISCARD uint64 Size() const NOEXCEPT = 0;

	/*
	*	Sets this enumeration from the given index.
	*/
	virtual void SetFromIndex(const uint64 index) NOEXCEPT = 0;

	/*
	*	Sets this enumeration from the given hash.
	*/
	virtual void SetFromHash(const uint64 hash) NOEXCEPT = 0;

	/*
	*	Returns the string for this enumeration.
	*/
	virtual NO_DISCARD const char *const RESTRICT ToString() const NOEXCEPT = 0;

	/*
	*	Returns the string for the given index.
	*/
	virtual NO_DISCARD const char *const RESTRICT IndexToString(const uint64 index) const NOEXCEPT = 0;

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
	NO_DISCARD uint64 Size() const NOEXCEPT override;													\
	void SetFromIndex(const uint64 index) NOEXCEPT override;											\
	void SetFromHash(const uint64 hash) NOEXCEPT override;												\
	NO_DISCARD const char *const RESTRICT ToString() const NOEXCEPT override;							\
	NO_DISCARD const char *const RESTRICT IndexToString(const uint64 index) const NOEXCEPT override;	\
private:																								\
	TYPE _VALUE;																						\
};