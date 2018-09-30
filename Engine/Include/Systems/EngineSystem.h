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
	EngineSystem() NOEXCEPT
	{
	
	}

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
	const CatalystProjectConfiguration& GetProjectConfiguration() const NOEXCEPT
	{
		return _ProjectConfiguration;
	}

	/*
	*	Returns the current update phase.
	*/
	UpdatePhase GetCurrentUpdatePhase() const NOEXCEPT
	{
		return _CurrentUpdatePhase;
	}

	/*
	*	Returns the delta time.
	*/
	float GetDeltaTime() const NOEXCEPT
	{
		return _DeltaTime;
	}

	/*
	*	Returns the total time.
	*/
	float GetTotalTime() const NOEXCEPT
	{
		return _TotalTime;
	}

	/*
	*	Terminates the game.
	*/
	void Terminate() NOEXCEPT
	{
		_ShouldTerminate = true;
	}

	/*
	*	Returns whether or not the game should terminate.
	*/
	bool ShouldTerminate() const NOEXCEPT
	{
		return _ShouldTerminate;
	}

private:

	//Enumeration covering all sequential updates.
	enum class SequentialUpdate : uint8
	{
		VegetationSystem,

		NumberOfSequentialUpdates
	};

	//The project configuration.
	CatalystProjectConfiguration _ProjectConfiguration;

	//Denotes the current update phase.
	UpdatePhase _CurrentUpdatePhase;

	//Denotes the delta time.
	float _DeltaTime{ 0.0f };

	//Denotes the total game time.
	float _TotalTime{ 0.0f };

	//Denotes whether or not the game should terminate.
	std::atomic<bool> _ShouldTerminate{ false };

	//The current sequential update.
	SequentialUpdate _CurrentSequentialUpdate{ SequentialUpdate::NumberOfSequentialUpdates };

	/*
	*	Executes the sequential update.
	*/
	void ExecuteSequentialUpdate() NOEXCEPT;

};