#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/CatalystProjectConfiguration.h>
#include <Core/Pointers/UniquePointer.h>

class EngineSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EngineSystem);

	/*
	*	Default constructor.
	*/
	EngineSystem() NOEXCEPT { }

	/*
	*	Initializes the engine system.
	*/
	void InitializeSystem(const CatalystProjectConfiguration &initialProjectConfiguration) NOEXCEPT;

	/*
	*	Updates the engine system synchronously.
	*/
	void UpdateSystemSynchronous(const float newDeltaTime) NOEXCEPT;

	/*
	*	Releases the engine system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Returns the project configuration.
	*/
	const CatalystProjectConfiguration& GetProjectConfiguration() const { return _ProjectConfiguration; }

	/*
	*	Returns whether or not the game should terminate.
	*/
	bool ShouldTerminate() const NOEXCEPT { return _ShouldTerminate; }

	/*
	*	Terminates the game.
	*/
	void Terminate() NOEXCEPT { _ShouldTerminate = true; }

	/*
	*	Returns the delta time.
	*/
	float GetDeltaTime() const NOEXCEPT { return _DeltaTime; }

	/*
	*	Returns the total time.
	*/
	float GetTotalTime() const NOEXCEPT { return _TotalTime; }

private:

	//The project configuration.
	CatalystProjectConfiguration _ProjectConfiguration;

	//Denotes whether or not the game should terminate.
	std::atomic<bool> _ShouldTerminate{ false };

	//Denotes the delta time.
	float _DeltaTime{ 0.0f };

	//Denotes the total game time.
	float _TotalTime{ 0.0f };

};