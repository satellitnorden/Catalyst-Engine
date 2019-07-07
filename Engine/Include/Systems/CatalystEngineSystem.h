#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>

class CatalystEngineSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(CatalystEngineSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE CatalystEngineSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the Catalyst engine system.
	*/
	void Initialize(const CatalystProjectConfiguration &initialProjectConfiguration) NOEXCEPT;

	/*
	*	Updates the Catalyst engine system.
	*/
	bool Update() NOEXCEPT;

	/*
	*	Terminates the Catalyst engine system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns the project configuration.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const CatalystProjectConfiguration *const RESTRICT GetProjectConfiguration() const NOEXCEPT
	{
		return &_ProjectConfiguration;
	}
	
	/*
	*	Sets whether or not the game should terminate.
	*/
	FORCE_INLINE void SetShouldTerminate(const bool shouldTerminate) NOEXCEPT
	{
		_ShouldTerminate = shouldTerminate;
	}

	/*
	*	Returns the total frames.
	*/
	FORCE_INLINE NO_DISCARD uint64 GetTotalFrames() const NOEXCEPT
	{
		return _TotalFrames;
	}

	/*
	*	Returns the total time.
	*/
	FORCE_INLINE NO_DISCARD float GetTotalTime() const NOEXCEPT
	{
		return _TotalTime;
	}

	/*
	*	Returns the delta time.
	*/
	FORCE_INLINE NO_DISCARD float GetDeltaTime() const NOEXCEPT
	{
		return _DeltaTime;
	}

private:

	//The project configuration.
	CatalystProjectConfiguration _ProjectConfiguration;

	//Denotes whether or not the game should terminate.
	std::atomic<bool> _ShouldTerminate{ false };

	//Denotes the total frames.
	uint64 _TotalFrames{ 0 };

	//Denotes the total time.
	float _TotalTime{ 0.0f };

	//Denotes the delta time.
	float _DeltaTime{ 0.0f };

};