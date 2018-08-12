#pragma once

//Core.
#include <Core/EngineCore.h>

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
	void InitializeSystem(const CatalystProjectInformation &initialProjectInformation) NOEXCEPT;

	/*
	*	Updates the engine system synchronously.
	*/
	void UpdateSystemSynchronous(const float newDeltaTime) NOEXCEPT;

	/*
	*	Releases the engine system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Returns the project information.
	*/
	const CatalystProjectInformation& GetProjectInformation() const { return projectInformation; }

	/*
	*	Returns whether or not the game should terminate.
	*/
	bool ShouldTerminate() const NOEXCEPT { return shouldTerminate; }

	/*
	*	Terminates the game.
	*/
	void Terminate() NOEXCEPT { shouldTerminate = true; }

	/*
	*	Returns the delta time.
	*/
	float GetDeltaTime() const NOEXCEPT { return deltaTime; }

	/*
	*	Returns the total game time.
	*/
	float GetTotalGameTime() const NOEXCEPT { return totalGameTime; }

private:

	//The project information.
	CatalystProjectInformation projectInformation;

	//Denotes whether or not the game should terminate.
	std::atomic<bool> shouldTerminate{ false };

	//Denotes the delta time.
	float deltaTime{ 0.0f };

	//Denotes the total game time.
	float totalGameTime{ 0.0f };

};