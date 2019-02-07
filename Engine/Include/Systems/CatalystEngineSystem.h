#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>
#include <Core/General/DeltaTimer.h>
#include <Core/Pointers/UniquePointer.h>

class CatalystEngineSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(CatalystEngineSystem);

	/*
	*	Default constructor.
	*/
	CatalystEngineSystem() NOEXCEPT
	{
	
	}

	/*
	*	Initializes the Catalyst engine system.
	*/
	void Initialize(const CatalystProjectConfiguration &initialProjectConfiguration) NOEXCEPT;

	/*
	*	Updates the Catalyst engine system.
	*/
	void Update() NOEXCEPT;

	/*
	*	Terminates the Catalystengine system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns the project configuration.
	*/
	FORCE_INLINE NO_DISCARD const CatalystProjectConfiguration& GetProjectConfiguration() const NOEXCEPT
	{
		return _ProjectConfiguration;
	}

	/*
	*	Returns the delta time.
	*/
	FORCE_INLINE NO_DISCARD float GetDeltaTime() const NOEXCEPT
	{
		return _DeltaTime;
	}

	/*
	*	Returns the total time.
	*/
	FORCE_INLINE NO_DISCARD float GetTotalTime() const NOEXCEPT
	{
		return _TotalTime;
	}

	/*
	*	Returns the total frames.
	*/
	FORCE_INLINE NO_DISCARD uint64 GetTotalFrames() const NOEXCEPT
	{
		return _TotalFrames;
	}

	/*
	*	Returns whether or not the game should terminate.
	*/
	bool GetShouldTerminate() const NOEXCEPT
	{
		return _ShouldTerminate;
	}

	/*
	*	Sets whether or not the game should terminate.
	*/
	void SetShouldTerminate(const bool shouldTerminate) NOEXCEPT
	{
		_ShouldTerminate = shouldTerminate;
	}

private:

	//Enumeration covering all sequential updates.
	enum class SequentialUpdate : uint8
	{
		EntityPlacementSystem,
		TerrainSystem,
		VegetationSystem,

		NumberOfSequentialUpdates
	};

	//The project configuration.
	CatalystProjectConfiguration _ProjectConfiguration;

	//The delta timer.
	DeltaTimer _DeltaTimer;

	//Denotes the delta time.
	float _DeltaTime{ 0.0f };

	//Denotes the total game time.
	float _TotalTime{ 0.0f };

	//Denotes the total frames.
	std::atomic<uint64> _TotalFrames{ 0 };

	//Denotes whether or not the game should terminate.
	std::atomic<bool> _ShouldTerminate{ false };

	//The current sequential update.
	SequentialUpdate _CurrentSequentialUpdate{ SequentialUpdate::NumberOfSequentialUpdates };

	/*
	*	Executes the sequential update.
	*/
	void ExecuteSequentialUpdate() NOEXCEPT;

};