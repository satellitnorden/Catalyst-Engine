#pragma once

//Engine core.
#include <EngineCore.h>

class EngineSystem
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EngineSystem);

	/*
	*	Default constructor.
	*/
	EngineSystem() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~EngineSystem() CATALYST_NOEXCEPT;

	/*
	*	Initializes the engine system.
	*/
	void InitializeSystem() CATALYST_NOEXCEPT;

	/*
	*	Updates the engine system synchronously. Returns whether or not the game should terminate.
	*/
	bool UpdateSystemSynchronous() CATALYST_NOEXCEPT;

	/*
	*	Releases the engine system.
	*/
	void ReleaseSystem() CATALYST_NOEXCEPT;

	/*
	*	Returns whether or not the game should terminate.
	*/
	CATALYST_FORCE_INLINE bool ShouldTerminate() const CATALYST_NOEXCEPT { return shouldTerminate; }

	/*
	*	Terminates the game.
	*/
	CATALYST_FORCE_INLINE void Terminate() CATALYST_NOEXCEPT { shouldTerminate = true; }

private:

	/*
	*	Enumeration covering all asynchronous threads.
	*/
	enum AsynchronousThread : uint8
	{
		InputSystem,
		GraphicsSystem,
		NumberOfAsynchronousThreads
	};

	//Denotes whether or not the game should terminate.
	std::atomic<bool> shouldTerminate{ false };

	//The delta time.
	float deltaTime{ 0.0f };

	//The current time.
	std::chrono::time_point<std::chrono::steady_clock> currentTime{ };

	//Container of all asynchronous threads.
	std::thread asynchronousThreads[AsynchronousThread::NumberOfAsynchronousThreads];

};