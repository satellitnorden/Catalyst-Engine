#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

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
	*	Waits for static models level of detail to finish executing.
	*/
	FORCE_INLINE void WaitForStaticModelsLevelOfDetail() const NOEXCEPT
	{
		_StaticModelsLevelOfDetailTask.WaitFor();
	}

	/*
	*	Waits for dynamic models level of detail to finish executing.
	*/
	FORCE_INLINE void WaitForDynamicModelsLevelOfDetail() const NOEXCEPT
	{
		_DynamicModelsLevelOfDetailTask.WaitFor();
	}

	/*
	*	Waits for vegetation level of detail to finish executing.
	*/
	FORCE_INLINE void WaitForVegetationLevelOfDetail() const NOEXCEPT
	{
		_VegetationLevelOfDetailTask.WaitFor();
	}

private:

	//The static models level of detail task.
	Task _StaticModelsLevelOfDetailTask;

	//The dynamic models level of detail task.
	Task _DynamicModelsLevelOfDetailTask;

	//The vegetation level of detail task.
	Task _VegetationLevelOfDetailTask;

	/*
	*	Calculates level of detail for static models.
	*/
	void LevelOfDetailStaticModels() const NOEXCEPT;

	/*
	*	Calculates level of detail for dynamice models.
	*/
	void LevelOfDetailDynamicModels() const NOEXCEPT;

	/*
	*	Calculates level of detail for vegetation.
	*/
	void LevelOfDetailVegetation() const NOEXCEPT;

};