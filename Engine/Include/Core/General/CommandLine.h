#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

class ALIGN(8) CommandLine final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(CommandLine);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE CommandLine() NOEXCEPT
	{

	}

	/*
	*	Initializes the command line.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Returns if the command line has the given key.
	*/
	NO_DISCARD bool HasKey(const char *const RESTRICT key) NOEXCEPT;

	/*
	*	Returns the value for the given key.
	*/
	NO_DISCARD bool GetValue(const char *const RESTRICT key, const char *RESTRICT *const RESTRICT value) NOEXCEPT;

private:

	//Denotes if the command line is initialized.
	bool _Initialized{ false };

	//The command line.
	DynamicString _CommandLine;

	//The keys.
	DynamicArray<const char* RESTRICT> _Keys;

	//The values.
	DynamicArray<const char* RESTRICT> _Values;

};