#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Concurrency.
#include <Concurrency/Task.h>

//Entities.
#include <Entities/Placement/EntityPlacementData.h>

class PlacementSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PlacementSystem);

	/*
	*	Default constructor.
	*/
	PlacementSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the placement system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the placement system during the sequential update phase.
	*/
	void SequentialUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Registers an placement function.
	*/
	void RegisterPlacementFunction(	const EntityPlacementType type,
									const EntityPlacementFunction function,
									const uint8 gridSize,
									const float gridCellSize) NOEXCEPT;

private:

	//The asynchronous update task.
	Task _AsynchronousUpdateTask;

	//Container for all placement data.
	DynamicArray<EntityPlacementData> _PlacementData;

	/*
	*	Updates the placement system asynchronously.
	*/
	void AsynchronousUpdate() NOEXCEPT;

	/*
	*	Updates the given two dimensional placement data.
	*/
	void UpdateTwoDimensionalPlacementData(EntityPlacementData *const RESTRICT data) NOEXCEPT;

	/*
	*	Updates the given three dimensional placement data.
	*/
	void UpdateThreeDimensionalPlacementData(EntityPlacementData *const RESTRICT data) NOEXCEPT;

};