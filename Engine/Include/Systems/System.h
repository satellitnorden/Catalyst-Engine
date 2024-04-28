#pragma once

/*
*	Declares a Catalyst system.
*	Put this within the class.
*	This will define an "Instance" static variable within the class that is automatically allocated by the engine.
*	This will also delete copy constructors, making this system a singleton.
*/
#define CATALYST_SYSTEM(X)			\
public:								\
	static X *RESTRICT Instance;	\
	X(const X &other) = delete;		\
	X(X &&other) = delete;