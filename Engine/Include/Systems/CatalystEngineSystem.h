#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>
#include <Core/Pointers/UniquePointer.h>

class CatalystEngineSystem final
{

public:

	/*
	*	Initializes the Catalyst engine system.
	*/
	static void Initialize(const CatalystProjectConfiguration &initialProjectConfiguration) NOEXCEPT;

	/*
	*	Updates the Catalyst engine system.
	*/
	static bool Update() NOEXCEPT;

	/*
	*	Terminates the Catalyst engine system.
	*/
	static void Terminate() NOEXCEPT;

private:

	/*
	*	Executes the sequential update.
	*/
	static void ExecuteSequentialUpdate() NOEXCEPT;

};