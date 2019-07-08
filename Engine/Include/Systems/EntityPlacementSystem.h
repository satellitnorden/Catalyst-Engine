#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Entities.
#include <Entities/Placement/EntityPlacementData.h>

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
	void Initialize() NOEXCEPT;

	/*
	*	Updates the entity placement system during the sequential update phase.
	*/
	void SequentialUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Registers an entity placement function.
	*/
	void RegisterEntityPlacementFunction(	const EntityPlacementType type,
											const EntityPlacementFunction function,
											const uint8 gridSize,
											const float gridCellSize) NOEXCEPT;

private:

	//The asynchronous update task.
	Task _AsynchronousUpdateTask;

	//Container for all entity placement data.
	DynamicArray<EntityPlacementData> _EntityPlacementData;

	/*
	*	Updates the entity placement system asynchronously.
	*/
	void AsynchronousUpdate() NOEXCEPT;

	/*
	*	Updates the given two dimensional entity placement data.
	*/
	void UpdateTwoDimensionalEntityPlacementData(EntityPlacementData *const RESTRICT data) NOEXCEPT;

	/*
	*	Updates the given three dimensional entity placement data.
	*/
	void UpdateThreeDimensionalEntityPlacementData(EntityPlacementData *const RESTRICT data) NOEXCEPT;

};