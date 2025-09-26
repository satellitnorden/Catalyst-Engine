#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Put this in your system declaration if this system should only exist if this is defined.
*/
#define SYSTEM_DEFINED_REQUIREMENT(X)

/*
*	Put this in your system declaration if this system should only exist if this is not defined.
*/
#define SYSTEM_NOT_DEFINED_REQUIREMENT(X)

/*
*	Put this in your system declaration and implement it to receive an "Initialize()" call.
*/
#define SYSTEM_INITIALIZE()		\
public:							\
	void Initialize() NOEXCEPT;

/*
*	Declares a Catalyst system.
*	Put this within the class.
*	This will define an "Instance" static variable within the class that is automatically allocated by the engine.
*	This will also delete copy constructors, making this system a singleton.
*
*	Can optionally add arguments, as above.
*/
#define CATALYST_SYSTEM(X, ...)		\
public:								\
	static X *RESTRICT Instance;	\
	X(const X &other) = delete;		\
	X(X &&other) = delete;			\
									\
	__VA_ARGS__

/*
*	"Manager" class for all systems.
*/
class Systems final
{

public:

	/*
	*	Initializes components.
	*/
	static void Initialize() NOEXCEPT;

};