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
	void UpdateSystemSynchronous(const float deltaTime) NOEXCEPT;

	/*
	*	Releases the engine system.
	*/
	void ReleaseSystem() NOEXCEPT;

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
		EntityPlacementSystem,
		TerrainSystem,
		VegetationSystem,

		NumberOfSequentialUpdates
	};

	//The project configuration.
	CatalystProjectConfiguration _ProjectConfiguration;

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