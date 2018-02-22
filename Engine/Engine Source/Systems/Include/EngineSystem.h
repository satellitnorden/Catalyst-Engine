#pragma once

//Engine core.
#include <EngineCore.h>

class EngineSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EngineSystem);

	/*
	*	Default constructor.
	*/
	EngineSystem() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~EngineSystem() NOEXCEPT;

	/*
	*	Initializes the engine system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the engine system synchronously. Returns whether or not the game should terminate.
	*/
	bool UpdateSystemSynchronous(const float deltaTime) NOEXCEPT;

	/*
	*	Releases the engine system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Returns whether or not the game should terminate.
	*/
	bool ShouldTerminate() const NOEXCEPT { return shouldTerminate; }

	/*
	*	Terminates the game.
	*/
	void Terminate() NOEXCEPT { shouldTerminate = true; }

private:

	//Denotes whether or not the game should terminate.
	std::atomic<bool> shouldTerminate{ false };

};