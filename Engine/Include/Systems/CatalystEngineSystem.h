#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/CatalystProjectConfiguration.h>
#include <Core/General/FramePacer.h>

//Concurrency.
#include <Concurrency/Task.h>

//Systems.
#include <Systems/System.h>

class CatalystEngineSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		CatalystEngineSystem
	);

	//Type aliases.
	using UpdateFunction = void(*)(void *const RESTRICT arguments);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE CatalystEngineSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the Catalyst engine system.
	*/
	void Initialize(const CatalystProjectConfiguration &initial_project_configuration) NOEXCEPT;

	/*
	*	Updates the Catalyst engine system.
	*/
	bool Update() NOEXCEPT;

	/*
	*	Terminates the Catalyst engine system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Registers an update.
	*	Returns a unique identifier for this update,
	*	which can later be used to query information about the update and deregister the update.
	*/
	uint64 RegisterUpdate(	const UpdateFunction update_function,
							void *const RESTRICT update_arguments,
							const UpdatePhase start,
							const UpdatePhase end,
							const bool run_on_main_thread,
							const bool only_update_in_game) NOEXCEPT;

	/*
	*	Registers a  sequential update.
	*	Returns a unique identifier for this sequential update,
	*	which can later be used to query information about the update and deregister the update.
	*	Sequential updates are run sequentially at the end of the frame,
	*	e.g. if multiple are registered, it will run once ever few frames or so.
	*	Useful for systems that just needs to check a few things, kick off a few tasks or so,
	*	but doesn't need to get updated every frame.
	*/
	uint64 RegisterSequentialUpdate(const UpdateFunction update_function, void* const RESTRICT update_arguments) NOEXCEPT;

	/*
	*	Deregisters an update.
	*/
	void DeregisterUpdate(const uint64 identifier) NOEXCEPT;

	/*
	*	Deregisters a sequential update.
	*/
	void DeregisterSequentialUpdate(const uint64 identifier) NOEXCEPT;

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
	FORCE_INLINE void SetShouldTerminate() NOEXCEPT
	{
		_ShouldTerminate = true;
	}

	/*
	*	Returns whether or not the game should terminate.
	*/
	FORCE_INLINE NO_DISCARD bool ShouldTerminate() NOEXCEPT
	{
		return _ShouldTerminate;
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
	FORCE_INLINE NO_DISCARD float32 GetTotalTime() const NOEXCEPT
	{
		return _TotalTime;
	}

	/*
	*	Returns the delta time.
	*/
	FORCE_INLINE NO_DISCARD float32 GetDeltaTime() const NOEXCEPT
	{
		return _DeltaTime;
	}

	/*
	*	Returns the update speed.
	*/
	FORCE_INLINE NO_DISCARD float32 GetUpdateSpeed() const NOEXCEPT
	{
		return _UpdateSpeed;
	}

	/*
	*	Sets the update speed.
	*/
	FORCE_INLINE void SetUpdateSpeed(const float32 update_speed) NOEXCEPT
	{
		_UpdateSpeed = update_speed;
	}

	/*
	*	Returns whether or not the engine is paused.
	*/
	FORCE_INLINE NO_DISCARD bool IsEnginePaused() const NOEXCEPT
	{
		return _EnginePaused;
	}

	/*
	*	Pauses the engine.
	*/
	FORCE_INLINE void PauseEngine() NOEXCEPT
	{
		_EnginePaused = true;
	}

	/*
	*	Un-pauses the engine.
	*/
	FORCE_INLINE void UnpauseEngine() NOEXCEPT
	{
		_EnginePaused = false;
	}

private:

	/*
	*	Update data class definition.
	*/
	class UpdateData final
	{

	public:

		//The identifier.
		uint64 _Identifier;

		//The update function.
		UpdateFunction _UpdateFunction;

		//The update arguments.
		void *RESTRICT _UpdateArguments;

		//The start update phase.
		UpdatePhase _Start;

		//The end update phase.
		UpdatePhase _End;

		//Denotes whether or not to run this update on the main thread.
		bool _RunOnMainThread{ false };

		//Denotes whether or not to run this update only when the engine is in game.
		bool _OnlyUpdateInGame{ false };

		//The task.
		Task _Task;

	};

	/*
	*	Sequential update data class definition.
	*/
	class SequentialUpdateData final
	{

	public:

		//The identifier.
		uint64 _Identifier;

		//The update function.
		UpdateFunction _UpdateFunction;

		//The update arguments.
		void* RESTRICT _UpdateArguments;

	};

	//The project configuration.
	CatalystProjectConfiguration _ProjectConfiguration;

	//Denotes if the engine is running single-threaded.
	bool _SingleThreaded{ false };

	//Denotes whether or not the game should terminate.
	bool _ShouldTerminate{ false };

	//Denotes the total frames.
	uint64 _TotalFrames{ 0 };

	//Denotes the total time.
	float32 _TotalTime{ 0.0f };

	//Denotes the delta time.
	float32 _DeltaTime{ 0.0f };

	//The update speed.
	float32 _UpdateSpeed{ 1.0f };

	//The update data counter.
	uint64 _UpdateDataCounter{ 0 };

	//Container for all start updata data.
	StaticArray<DynamicArray<UpdateData *RESTRICT>, UNDERLYING(UpdatePhase::NUMBER_OF_UPDATES_PHASES)> _StartUpdateData;

	//Container for all end updata data.
	StaticArray<DynamicArray<UpdateData *RESTRICT>, UNDERLYING(UpdatePhase::NUMBER_OF_UPDATES_PHASES)> _EndUpdateData;

	//The sequential update data counter.
	uint64 _SequentialUpdateDataCounter{ 0 };

	//The current sequential update index.
	uint64 _CurrentSequentialUpdateIndex{ 0 };

	//The sequential update data.
	DynamicArray<SequentialUpdateData> _SequentialUpdateData;

	//Denotes if the engine is paused.
	bool _EnginePaused{ false };

	//The frame pacer.
	FramePacer _FramePacer;

	/*
	*	Updates an individual phase.
	*/
	void UpdateIndividualPhase(const UpdatePhase phase) NOEXCEPT;

	/*
	*	Updates sequentially.
	*/
	void UpdateSequentially() NOEXCEPT;

};