#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Concurrency.
#include <Concurrency/Task.h>

class LevelOfDetailSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LevelOfDetailSystem);

	/*
	*	Default constructor.
	*/
	LevelOfDetailSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the level of detail system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the level of detail system during the render update phase.
	*/
	void RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Sets the minimum screen coverage.
	*/
	FORCE_INLINE void SetMinimumScreenCoverage(const float32 value) NOEXCEPT
	{
		_MinimumScreenCoverage = value;
	}

	/*
	*	Sets the maximum screen coverage.
	*/
	FORCE_INLINE void SetMaximumScreenCoverage(const float32 value) NOEXCEPT
	{
		_MaximumScreenCoverage = value;
	}

	/*
	*	Waits for static models level of detail to finish executing.
	*/
	FORCE_INLINE void WaitForStaticModelsLevelOfDetail() const NOEXCEPT
	{
		_StaticModelsLevelOfDetailTask.Wait<WaitMode::YIELD>();
	}

	/*
	*	Waits for dynamic models level of detail to finish executing.
	*/
	FORCE_INLINE void WaitForDynamicModelsLevelOfDetail() const NOEXCEPT
	{
		_DynamicModelsLevelOfDetailTask.Wait<WaitMode::YIELD>();
	}

private:

	//The minimum screen coverage.
	float32 _MinimumScreenCoverage{ 0.0f };

	//The maximum screen coverage.
	float32 _MaximumScreenCoverage{ 1.0f };

	//The static models level of detail task.
	Task _StaticModelsLevelOfDetailTask;

	//The dynamic models level of detail task.
	Task _DynamicModelsLevelOfDetailTask;

	/*
	*	Calculates level of detail for static models.
	*/
	void LevelOfDetailStaticModels() const NOEXCEPT;

	/*
	*	Calculates level of detail for dynamice models.
	*/
	void LevelOfDetailDynamicModels() const NOEXCEPT;

};