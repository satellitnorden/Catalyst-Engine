#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Any.h>

//Concurrency.
#include <Concurrency/Task.h>

//Entities.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/Geometry/GridPoint2.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class PlacementSystem final
{

public:

	//Constants.
	constexpr static uint64 CUSTOM_PLACEMENT_DATA_SIZE{ 8 };
	constexpr static uint64 CUSTOM_CELL_DATA_SIZE{ 8 };

	//Singleton declaration.
	DECLARE_SINGLETON(PlacementSystem);

	//Type aliases.
	using PlacementFunction = void(*)(	const Any<CUSTOM_PLACEMENT_DATA_SIZE> &custom_placement_data,
										const WorldSpaceAxisAlignedBoundingBox3D &box,
										DynamicArray<Entity *const RESTRICT> *const RESTRICT entities,
										Any<CUSTOM_CELL_DATA_SIZE> *const RESTRICT custom_cell_data);
	using DeplacementFunction = void(*)(const Any<CUSTOM_PLACEMENT_DATA_SIZE> &custom_placement_data,
										DynamicArray<Entity *const RESTRICT> *const RESTRICT entities,
										Any<CUSTOM_CELL_DATA_SIZE> *const RESTRICT custom_cell_data);

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
	void SequentialUpdate() NOEXCEPT;

	/*
	*	Registers an placement function.
	*/
	void RegisterPlacementFunction(	const Any<CUSTOM_PLACEMENT_DATA_SIZE> &custom_placement_data,
									const PlacementFunction placement_function,
									const DeplacementFunction deplacement_function,
									const uint8 grid_dimensions,
									const float32 grid_size) NOEXCEPT;

private:

	/*
	*	Placement data class definition.
	*/
	class PlacementData final
	{

	public:

		//The custom placement data.
		Any<CUSTOM_PLACEMENT_DATA_SIZE> _CustomPlacementData;

		//The placement function.
		PlacementFunction _PlacementFunction;

		//The deplacement function.
		DeplacementFunction _DeplacementFunction;

		//The grid dimensions.
		uint8 _GridDimensions;

		//The grid size.
		float32 _GridSize;

		//Container for all grid points.
		DynamicArray<GridPoint2> _GridPoints;

		//The entities.
		DynamicArray<DynamicArray<Entity *const RESTRICT>> _Entities;

		//The custom data for the cells.
		DynamicArray<Any<CUSTOM_CELL_DATA_SIZE>> _CustomCellData;

	};

	//The asynchronous update task.
	Task _AsynchronousUpdateTask;

	//Container for all placement data.
	DynamicArray<PlacementData> _PlacementData;

	/*
	*	Updates the placement system asynchronously.
	*/
	void AsynchronousUpdate() NOEXCEPT;

};