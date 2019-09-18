#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Multithreading.
#include <Multithreading/Task.h>

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
	*	Waits for vegetation level of detail to finish executing.
	*/
	FORCE_INLINE void WaitForVegetationLevelOfDetail() const NOEXCEPT
	{
		_VegetationLevelOfDetailTask.WaitFor();
	}

private:

	//The vegetation level of detail task.
	Task _VegetationLevelOfDetailTask;

	/*
	*	Calculates level of detail for vegetation.
	*/
	void LevelOfDetailVegetation() const NOEXCEPT;

};