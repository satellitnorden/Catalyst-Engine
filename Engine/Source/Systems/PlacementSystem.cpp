//Header file.
#include <Systems/PlacementSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PlacementSystem);

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>
#include <Core/General/Perceiver.h>

//Math.
#include <Math/Geometry/GridPoint2.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/TaskSystem.h>

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
	const Vector3<float> perceiverPosition{ Perceiver::Instance->GetWorldTransform().GetAbsolutePosition() };
	const GridPoint2 currentGridPoint{ GridPoint2::WorldPositionToGridPoint(perceiverPosition, data->_GridCellSize) };

	//Calculate the wanted grid points.
	DynamicArray<GridPoint2> wantedGridPoints;
	wantedGridPoints.Reserve(data->_GridSize * data->_GridSize);

	const int8 halfGridSize{ static_cast<int8>(data->_GridSize >> 1) };

	for (int8 x{ -halfGridSize }; x <= halfGridSize; ++x)
	{
		for (int8 y{ -halfGridSize }; y <= halfGridSize; ++y)
		{
			wantedGridPoints.Emplace(currentGridPoint + GridPoint2(x, y));
		}
	}

	//Sort the wanted grid points so that the one's nearest the perceiver appears first.
	SortingAlgorithms::InsertionSort<GridPoint2>(
		wantedGridPoints.Begin(),
		wantedGridPoints.End(),
		data,
		[](const void *const RESTRICT userData, const GridPoint2 *const RESTRICT first, const GridPoint2 *const RESTRICT second)
		{
			const EntityPlacementData *const RESTRICT data{ static_cast<const EntityPlacementData *const RESTRICT>(userData) };

			const Vector3<float> perceiverPosition{ Perceiver::Instance->GetWorldTransform().GetAbsolutePosition() };

			const Vector3<float> firstPosition{ GridPoint2::GridPointToWorldPosition(*first, data->_GridCellSize) };
			const Vector3<float> secondPosition{ GridPoint2::GridPointToWorldPosition(*second, data->_GridCellSize) };

			return Vector3<float>::LengthSquaredXZ(firstPosition - perceiverPosition) < Vector3<float>::LengthSquaredXZ(secondPosition - perceiverPosition);
		});

	//Compare the grid points in the placement data to the wanted grid points and destroy entities if they shouldn't exist.
	for (uint64 i{ 0 }; i < data->_GridPoints.Size();)
	{
		EntityPlacementData::EntityGridPoint &entityGridPoint{ data->_GridPoints[i] };

		bool gridPointExists{ false };

		for (const GridPoint2 wantedGridPoint : wantedGridPoints)
		{
			gridPointExists |= wantedGridPoint == entityGridPoint._GridPoint2;
		}

		if (!gridPointExists)
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
	for (const GridPoint2 wantedGridPoint : wantedGridPoints)
	{
		bool gridPointExists{ false };

		for (const EntityPlacementData::EntityGridPoint &entityGridPoint : data->_GridPoints)
		{
			gridPointExists |= wantedGridPoint == entityGridPoint._GridPoint2;
		}

		if (!gridPointExists)
		{
			data->_GridPoints.Emplace(EntityPlacementData::EntityGridPoint{ wantedGridPoint });
			data->_Entities.Emplace(DynamicArray<Entity *RESTRICT>());

			//Calculate the axis aligned bounding box.
			const Vector3<float> wantedGridPointWorldPosition{ GridPoint2::GridPointToWorldPosition(wantedGridPoint, data->_GridCellSize) };

			AxisAlignedBoundingBox3 box;

			box._Minimum._X = wantedGridPointWorldPosition._X - data->_GridCellSize * 0.5f;
			box._Minimum._Y = 0.0f;
			box._Minimum._Z = wantedGridPointWorldPosition._Z - data->_GridCellSize * 0.5f;

			box._Maximum._X = wantedGridPointWorldPosition._X + data->_GridCellSize * 0.5f;
			box._Maximum._Y = 0.0f;
			box._Maximum._Z = wantedGridPointWorldPosition._Z + data->_GridCellSize * 0.5f;

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