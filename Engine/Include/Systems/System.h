#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Declares a Catalyst system.
*	Put this within the class.
*	This will define an "Instance" static variable within the class that is automatically allocated by the engine.
*	This will also delete copy constructors, making this system a singleton.
*
*	Can optionally add arguments, which can include;
*	- DEFINED_REQUIREMENT(X) - Means that this system will only be set up if the given define is defined.
*	- NOT_DEFINED_REQUIREMENT(X) - Means that this system will only be set up if the given define is not defined.
*/
#define CATALYST_SYSTEM(X, ...)		\
public:								\
	static X *RESTRICT Instance;	\
	X(const X &other) = delete;		\
	X(X &&other) = delete;