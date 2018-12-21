#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>
#include <Core/Pointers/UniquePointer.h>

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
	LevelOfDetailSystem() NOEXCEPT { }

	/*
	*	Initializes the level of detail system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the level of detail system synchronously.
	*/
	void UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Waits for the solid vegetation level of detail to finish.
	*/
	void WaitForSolidVegetationLevelOfDetail() const NOEXCEPT
	{
		_LevelOfDetailTasks[UNDERLYING(LevelOfDetailTask::SolidVegetation)].WaitFor();
	}

private:

	//Enumeration covering all level of detail tasks.
	enum class LevelOfDetailTask : uint8
	{
		SolidVegetation,

		NumberOfLevelOfDetailTasks
	};

	//Container for all level of detail tasks.
	StaticArray<Task, UNDERLYING(LevelOfDetailTask::NumberOfLevelOfDetailTasks)> _LevelOfDetailTasks;

	/*
	*	Calculates level of detail for solid vegetation.
	*/
	void LevelOfDetailSolidVegetation() NOEXCEPT;

};