#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class Thread final
{

public:

	//Enumeration covering all priorities.
	enum class Priority : int8
	{
		LOWEST = -2,
		BELOW_NORMAL = -1,
		NORMAL = 0,
		ABOVE_NORMAL = 1,
		HIGHEST = 2
	};

	/*
	*	Type aliases.
	*/
	using Function = void(*)();

	/*
	*	Default constructor.
	*/
	Thread() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~Thread() NOEXCEPT;

	/*
	*	Sets the function.
	*/
	void SetFunction(const Function function) NOEXCEPT;

	/*
	*	Sets the priority of this thread.
	*/
	void SetPriority(const Priority priority) NOEXCEPT;

	/*
	*	Launches this thread.
	*/
	void Launch() NOEXCEPT;

	/*
	*	Joins this thread.
	*	This is a blocking call, will wait for the thread to become joinable.
	*/
	void Join() NOEXCEPT;

private:

	//The platform data.
	void *RESTRICT _PlatformData{ nullptr };

	//The function.
	Function _Function{ nullptr };

	//The priority.
	Priority _Priority{ Priority::NORMAL };

};