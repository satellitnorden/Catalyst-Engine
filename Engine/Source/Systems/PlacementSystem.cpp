//Header file.
#include <Systems/PlacementSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>
#include <Core/General/Perceiver.h>

//Math.
#include <Math/Geometry/GridPoint2.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PlacementSystem);

/*
*	Initializes the placement system.
*/
void PlacementSystem::Initialize() NOEXCEPT
{
	//Initialize the asynchronous update function.
	_AsynchronousUpdateTask._Function = [](void *const RESTRICT)
	{
		PlacementSystem::Instance->AsynchronousUpdate();
	};
	_AsynchronousUpdateTask._Arguments = nullptr;
	_AsynchronousUpdateTask._ExecutableOnSameThread = false;
}

/*
*	Updates the placement system during the sequential update phase.
*/
void PlacementSystem::SequentialUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Is the asynchronous update task done?
	if (!_AsynchronousUpdateTask.IsExecuted())
	{
		return;
	}

	//Has any placement data been registered?
	if (_PlacementData.Empty())
	{
		return;
	}

	//Fire off the asynchronous update task.
	TaskSystem::Instance->ExecuteTask(&_AsynchronousUpdateTask);
}

/*
*	Registers an placement function.
*/
void PlacementSystem::RegisterPlacementFunction(const EntityPlacementType type,
												const EntityPlacementFunction function,
												const uint8 gridSize,
												const float gridCellSize) NOEXCEPT
{
	//Initialize the new placement data.
	EntityPlacementData data;

	data._Type = type;
	data._Function = function;
	data._GridSize = gridSize;
	data._GridCellSize = gridCellSize;

	//Put it in the container.
	_PlacementData.Emplace(data);
}

/*
*	Updates the placement system asynchronously.
*/
void PlacementSystem::AsynchronousUpdate() NOEXCEPT
{
	//Iterate over all entity placement data and check if they need updating.
	for (EntityPlacementData &data : _PlacementData)
	{
		//Update differently depending on if the entity placement data is two or three dimensional.
		switch (data._Type)
		{
			case EntityPlacementType::TWO_DIMENSIONAL:
			{
				UpdateTwoDimensionalPlacementData(&data);

				break;
			}

			case EntityPlacementType::THREE_DIMENSIONAL:
			{
				UpdateThreeDimensionalPlacementData(&data);

				break;
			}

			default:
			{
				ASSERT(false, "Unhandled case!");

				break;
			}
		}
	}
}

/*
*	Updates the given two dimensional placement data.
*/
void PlacementSystem::UpdateTwoDimensionalPlacementData(EntityPlacementData *const RESTRICT data) NOEXCEPT
{
	//Calculate the current grid point based on the perceiver's position.
	const Vector3<float32> perceiver_position{ Perceiver::Instance->GetWorldTransform().GetAbsolutePosition() };
	const GridPoint2 current_grid_point{ GridPoint2::WorldPositionToGridPoint(perceiver_position, data->_GridCellSize) };

	//Calculate the wanted grid points.
	DynamicArray<GridPoint2> wanted_grid_points;
	wanted_grid_points.Reserve(data->_GridSize * data->_GridSize);

	const int8 half_grid_size{ static_cast<int8>(data->_GridSize >> 1) };

	for (int8 x{ -half_grid_size }; x <= half_grid_size; ++x)
	{
		for (int8 y{ -half_grid_size }; y <= half_grid_size; ++y)
		{
			wanted_grid_points.Emplace(current_grid_point + GridPoint2(x, y));
		}
	}

	//Sort the wanted grid points so that the one's nearest the perceiver appears first.
	SortingAlgorithms::InsertionSort<GridPoint2>(
	wanted_grid_points.Begin(),
	wanted_grid_points.End(),
	data,
	[](const void *const RESTRICT user_data, const GridPoint2 *const RESTRICT first, const GridPoint2 *const RESTRICT second)
	{
		const EntityPlacementData *const RESTRICT data{ static_cast<const EntityPlacementData *const RESTRICT>(user_data) };

		const Vector3<float32> perceiver_position{ Perceiver::Instance->GetWorldTransform().GetAbsolutePosition() };

		const Vector3<float32> first_position{ GridPoint2::GridPointToWorldPosition(*first, data->_GridCellSize) };
		const Vector3<float32> second_position{ GridPoint2::GridPointToWorldPosition(*second, data->_GridCellSize) };

		return Vector3<float32>::LengthSquaredXZ(first_position - perceiver_position) < Vector3<float>::LengthSquaredXZ(second_position - perceiver_position);
	});

	//Compare the grid points in the placement data to the wanted grid points and destroy entities if they shouldn't exist.
	for (uint64 i{ 0 }; i < data->_GridPoints.Size();)
	{
		EntityPlacementData::EntityGridPoint &entity_grid_point{ data->_GridPoints[i] };

		bool grid_point_exists{ false };

		for (const GridPoint2 wanted_grid_point : wanted_grid_points)
		{
			grid_point_exists |= wanted_grid_point == entity_grid_point._GridPoint2;
		}

		if (!grid_point_exists)
		{
			for (Entity *const RESTRICT entity : data->_Entities[i])
			{
				EntitySystem::Instance->RequestTermination(entity);
				EntitySystem::Instance->RequestDestruction(entity);
			}

			data->_GridPoints.EraseAt<false>(i);
			data->_Entities.EraseAt<false>(i);
		}

		else
		{
			++i;
		}
	}

	//Compare the grid points in the placement data to the wanted grid points and execute placement if it doesn't exist.
	for (const GridPoint2 wanted_grid_point : wanted_grid_points)
	{
		bool grid_point_exists{ false };

		for (const EntityPlacementData::EntityGridPoint &entityGridPoint : data->_GridPoints)
		{
			grid_point_exists |= wanted_grid_point == entityGridPoint._GridPoint2;
		}

		if (!grid_point_exists)
		{
			data->_GridPoints.Emplace(EntityPlacementData::EntityGridPoint{ wanted_grid_point });
			data->_Entities.Emplace(DynamicArray<Entity *RESTRICT>());

			//Calculate the axis aligned bounding box.
			const Vector3<float32> wanted_grid_Point_world_position{ GridPoint2::GridPointToWorldPosition(wanted_grid_point, data->_GridCellSize) };

			AxisAlignedBoundingBox3D box;

			box._Minimum._X = wanted_grid_Point_world_position._X - data->_GridCellSize * 0.5f;
			box._Minimum._Y = 0.0f;
			box._Minimum._Z = wanted_grid_Point_world_position._Z - data->_GridCellSize * 0.5f;

			box._Maximum._X = wanted_grid_Point_world_position._X + data->_GridCellSize * 0.5f;
			box._Maximum._Y = 0.0f;
			box._Maximum._Z = wanted_grid_Point_world_position._Z + data->_GridCellSize * 0.5f;

			//Execute the placement function!
			data->_Function(box, &data->_Entities.Back());
		}
	}
}

/*
*	Updates the given three dimensional placement data.
*/
void PlacementSystem::UpdateThreeDimensionalPlacementData(EntityPlacementData *const RESTRICT data) NOEXCEPT
{
	ASSERT(false, "Not implemented yet!");
}