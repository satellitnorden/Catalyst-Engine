//Header file.
#include <Systems/PlacementSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//Math.
#include <Math/Geometry/GridPoint2.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PlacementSystem);

/*
*	Updates the placement system during the sequential update phase.
*/
void PlacementSystem::SequentialUpdate() NOEXCEPT
{
	//Check all placement data, and fire off the asynchronous tasks if they are executed.
	for (PlacementData *const RESTRICT placement_data : _PlacementData)
	{
		if (placement_data->_AsynchronousUpdateTask.IsExecuted())
		{
			TaskSystem::Instance->ExecuteTask(&placement_data->_AsynchronousUpdateTask);
		}
	}
}

/*
*	Registers an placement function.
*/
void PlacementSystem::RegisterPlacementFunction(const Any<CUSTOM_PLACEMENT_DATA_SIZE> &custom_placement_data,
												const PlacementFunction placement_function,
												const DeplacementFunction deplacement_function,
												const uint8 grid_dimensions,
												const float32 grid_size) NOEXCEPT
{
	//Allocate the new placement data.
	PlacementData *const RESTRICT data{ new (MemorySystem::Instance->TypeAllocate<PlacementData>()) PlacementData() };

	//Initialize the asynchronous update function.
	data->_AsynchronousUpdateTask._Function = [](void *const RESTRICT arguments)
	{
		PlacementSystem::Instance->AsynchronousUpdate(static_cast<PlacementData *const RESTRICT>(arguments));
	};
	data->_AsynchronousUpdateTask._Arguments = data;
	data->_AsynchronousUpdateTask._ExecutableOnSameThread = false;
	data->_CustomPlacementData = custom_placement_data;
	data->_PlacementFunction = placement_function;
	data->_DeplacementFunction = deplacement_function;
	data->_GridDimensions = grid_dimensions;
	data->_GridSize = grid_size;

	//Put it in the container.
	_PlacementData.Emplace(data);
}

/*
*	Updates the placement system asynchronously.
*/
void PlacementSystem::AsynchronousUpdate(PlacementData *const RESTRICT placement_data) NOEXCEPT
{
	//Terminate early if the game is terminating.
	if (CatalystEngineSystem::Instance->ShouldTerminate())
	{
		return;
	}

	//Calculate the current grid point based on the camera's position.
	const Vector3<float32> camera_position{ RenderingSystem::Instance->GetCurrentCamera()->GetWorldTransform().GetAbsolutePosition() };
	const GridPoint2 current_grid_point{ GridPoint2::WorldPositionToGridPoint(camera_position, placement_data->_GridSize) };

	//Calculate the wanted grid points.
	DynamicArray<GridPoint2> wanted_grid_points;
	wanted_grid_points.Reserve(placement_data->_GridDimensions * placement_data->_GridDimensions);

	const int8 half_grid_size{ static_cast<int8>(placement_data->_GridDimensions >> 1) };

	for (int8 x{ static_cast<int8>(-half_grid_size) }; x <= half_grid_size; ++x)
	{
		for (int8 y{ static_cast<int8>(-half_grid_size) }; y <= half_grid_size; ++y)
		{
			wanted_grid_points.Emplace(current_grid_point + GridPoint2(x, y));
		}
	}

	//Sort the wanted grid points so that the one's nearest the camera appears first.
	SortingAlgorithms::InsertionSort<GridPoint2>(
	wanted_grid_points.Begin(),
	wanted_grid_points.End(),
	placement_data,
	[](const void *const RESTRICT user_data, const GridPoint2 *const RESTRICT first, const GridPoint2 *const RESTRICT second)
	{
		const PlacementData *const RESTRICT data{ static_cast<const PlacementData *const RESTRICT>(user_data) };

		const Vector3<float32> camera_position{ RenderingSystem::Instance->GetCurrentCamera()->GetWorldTransform().GetAbsolutePosition() };

		const Vector3<float32> first_position{ GridPoint2::GridPointToWorldPosition(*first, data->_GridSize) };
		const Vector3<float32> second_position{ GridPoint2::GridPointToWorldPosition(*second, data->_GridSize) };

		return Vector3<float32>::LengthSquaredXZ(first_position - camera_position) < Vector3<float>::LengthSquaredXZ(second_position - camera_position);
	});

	//Compare the grid points in the placement data to the wanted grid points and destroy entities if they shouldn't exist.
	for (uint64 i{ 0 }; i < placement_data->_GridPoints.Size();)
	{
		GridPoint2 &entity_grid_point{ placement_data->_GridPoints[i] };

		bool grid_point_exists{ false };

		for (const GridPoint2 wanted_grid_point : wanted_grid_points)
		{
			grid_point_exists |= wanted_grid_point == entity_grid_point;
		}

		if (!grid_point_exists)
		{
			if (placement_data->_DeplacementFunction)
			{
				placement_data->_DeplacementFunction(placement_data->_CustomPlacementData, &placement_data->_Entities[i], &placement_data->_CustomCellData[i]);
			}

			for (Entity* const RESTRICT entity : placement_data->_Entities[i])
			{
				EntitySystem::Instance->RequestTermination(entity);
				EntitySystem::Instance->RequestDestruction(entity);
			}

			placement_data->_GridPoints.EraseAt<false>(i);
			placement_data->_Entities.EraseAt<false>(i);
			placement_data->_CustomCellData.EraseAt<false>(i);
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

		for (const GridPoint2 &grid_point : placement_data->_GridPoints)
		{
			grid_point_exists |= wanted_grid_point == grid_point;
		}

		if (!grid_point_exists)
		{
			placement_data->_GridPoints.Emplace(wanted_grid_point);
			placement_data->_Entities.Emplace();
			placement_data->_CustomCellData.Emplace();

			//Calculate the axis aligned bounding box.
			const Vector3<float32> wanted_grid_Point_world_position{ GridPoint2::GridPointToWorldPosition(wanted_grid_point, placement_data->_GridSize) };

			AxisAlignedBoundingBox3D box;

			box._Minimum._X = wanted_grid_Point_world_position._X - placement_data->_GridSize * 0.5f;
			box._Minimum._Y = 0.0f;
			box._Minimum._Z = wanted_grid_Point_world_position._Z - placement_data->_GridSize * 0.5f;

			box._Maximum._X = wanted_grid_Point_world_position._X + placement_data->_GridSize * 0.5f;
			box._Maximum._Y = 0.0f;
			box._Maximum._Z = wanted_grid_Point_world_position._Z + placement_data->_GridSize * 0.5f;

			WorldSpaceAxisAlignedBoundingBox3D world_space_axis_aligned_box{ WorldPosition(box._Minimum), WorldPosition(box._Maximum) };

			//Execute the placement function!
			placement_data->_PlacementFunction(placement_data->_CustomPlacementData, world_space_axis_aligned_box, &placement_data->_Entities.Back(), &placement_data->_CustomCellData.Back());
		}
	}
}