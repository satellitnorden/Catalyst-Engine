#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Concurrency.
#include <Concurrency/Task.h>

class CatalystEngineSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(CatalystEngineSystem);

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
							const bool run_on_main_thread) NOEXCEPT;

	/*
	*	Deregisters an update.
	*/
	void DeregisterUpdate(const uint64 identifier) NOEXCEPT;

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
	*	Sets the update speed.
	*/
	FORCE_INLINE void SetUpdateSpeed(const float32 update_speed) NOEXCEPT
	{
		_UpdateSpeed = update_speed;
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

		//The task.
		Task _Task;

	};

	//The project configuration.
	CatalystProjectConfiguration _ProjectConfiguration;

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

	/*
	*	Updates an individual phase.
	*/
	void UpdateIndividualPhase(const UpdatePhase phase) NOEXCEPT;

};