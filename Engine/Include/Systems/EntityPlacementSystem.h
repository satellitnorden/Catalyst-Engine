#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Pointers/UniquePointer.h>

//Entities.
#include <Entities/Placement/EntityPlacementCore.h>
#include <Entities/Placement/EntityTwoDimensionalPlacementData.h>
#include <Entities/Placement/EntityThreeDimensionalPlacementData.h>

//Multithreading.
#include <Multithreading/Task.h>

class EntityPlacementSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EntityPlacementSystem);

	/*
	*	Default constructor.
	*/
	EntityPlacementSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the entity placement system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the entity system sequentially.
	*/
	void SequentialUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Registers a two dimensional placement function.
	*/
	void RegisterTwoDimensionalPlacementFunction(EntityPlacementFunction function, const float gridSize) NOEXCEPT;

	/*
	*	Registers a three dimensional placement function.
	*/
	void RegisterThreeDimensionalPlacementFunction(EntityPlacementFunction function, const float gridSize) NOEXCEPT;

private:

	//The placement update task.
	Task _PlacementUpdateTask;

	//The two dimensional placement data.
	DynamicArray<EntityTwoDimensionalPlacementData> _TwoDimensionalPlacementData;

	//The three dimensional placement data.
	DynamicArray<EntityThreeDimensionalPlacementData> _ThreeDimensionalPlacementData;

	/*
	*	Updates placement.
	*/
	void UpdatePlacement() NOEXCEPT;

	/*
	*	Updates two dimensional placement.
	*/
	void UpdateTwoDimensionalPlacement() NOEXCEPT;

	/*
	*	Updates three dimensional placement.
	*/
	void UpdateThreeDimensionalPlacement() NOEXCEPT;

};