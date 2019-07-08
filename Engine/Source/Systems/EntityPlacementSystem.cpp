//Header file.
#include <Systems/EntityPlacementSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EntityPlacementSystem);

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>
#include <Core/General/Perceiver.h>

//Math.
#include <Math/Geometry/GridPoint2.h>

//Systems.
#include <Systems/TaskSystem.h>

/*
*	Initializes the entity placement system.
*/
void EntityPlacementSystem::Initialize() NOEXCEPT
{
	//Initialize the asynchronous update function.
	_AsynchronousUpdateTask._Function = [](void *const RESTRICT)
	{
		EntityPlacementSystem::Instance->AsynchronousUpdate();
	};
	_AsynchronousUpdateTask._Arguments = nullptr;
}

/*
*	Updates the entity placement system during the sequential update phase.
*/
void EntityPlacementSystem::SequentialUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Is the asynchronous update task done?
	if (!_AsynchronousUpdateTask.IsExecuted())
	{
		return;
	}

	//Has any entity placement data been registered?
	if (_EntityPlacementData.Empty())
	{
		return;
	}

	//Fire off the asynchronous update task.
	TaskSystem::Instance->ExecuteTask(&_AsynchronousUpdateTask);
}

/*
*	Registers an entity placement function.
*/
void EntityPlacementSystem::RegisterEntityPlacementFunction(const EntityPlacementType type,
															const EntityPlacementFunction function,
															const uint8 gridSize,
															const float gridCellSize) NOEXCEPT
{
	//Initialize the new entity placement data.
	EntityPlacementData data;

	data._Type = type;
	data._Function = function;
	data._GridSize = gridSize;
	data._GridCellSize = gridCellSize;

	//Put it in the container.
	_EntityPlacementData.EmplaceSlow(data);
}

/*
*	Updates the entity placement system asynchronously.
*/
void EntityPlacementSystem::AsynchronousUpdate() NOEXCEPT
{
	//Iterate over all entity placement data and check if they need updating.
	for (EntityPlacementData &data : _EntityPlacementData)
	{
		//Update differently depending on if the entity placement data is two or three dimensional.
		switch (data._Type)
		{
			case EntityPlacementType::TwoDimensional:
			{
				UpdateTwoDimensionalEntityPlacementData(&data);

				break;
			}

			case EntityPlacementType::ThreeDimensional:
			{
				UpdateThreeDimensionalEntityPlacementData(&data);

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
*	Updates the given two dimensional entity placement data.
*/
void EntityPlacementSystem::UpdateTwoDimensionalEntityPlacementData(EntityPlacementData *const RESTRICT data) NOEXCEPT
{
	//Calculate the current grid point based on the perceiver's position.
	const Vector3<float> perceiverPosition{ Perceiver::Instance->GetPosition() };
	const GridPoint2 currentGridPoint{ GridPoint2::WorldPositionToGridPoint(perceiverPosition, data->_GridCellSize) };

	//Calculate the wanted grid points.
	DynamicArray<GridPoint2> wantedGridPoints;
	wantedGridPoints.Reserve(data->_GridSize * data->_GridSize);

	const int8 halfGridSize{ static_cast<int8>(data->_GridSize >> 1) };

	for (int8 x{ -halfGridSize }; x <= halfGridSize; ++x)
	{
		for (int8 y{ -halfGridSize }; y <= halfGridSize; ++y)
		{
			wantedGridPoints.EmplaceFast(currentGridPoint + GridPoint2(x, y));
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

			const Vector3<float> perceiverPosition{ Perceiver::Instance->GetPosition() };

			const Vector3<float> firstPosition{ GridPoint2::GridPointToWorldPosition(*first, data->_GridCellSize) };
			const Vector3<float> secondPosition{ GridPoint2::GridPointToWorldPosition(*second, data->_GridCellSize) };

			return Vector3<float>::LengthSquaredXZ(firstPosition - perceiverPosition) < Vector3<float>::LengthSquaredXZ(secondPosition - perceiverPosition);
		});

	//Compare the grid points in the entity placement data to the wanted grid points and execute placement if it doesn't exist.
	for (const GridPoint2 wantedGridPoint : wantedGridPoints)
	{
		bool gridPointExists{ false };

		for (const EntityPlacementData::EntityGridPoint &entityGridPoint : data->_GridPoints)
		{
			gridPointExists |= wantedGridPoint == entityGridPoint._GridPoint2;
		}

		if (!gridPointExists)
		{
			data->_GridPoints.EmplaceSlow(EntityPlacementData::EntityGridPoint{ wantedGridPoint });
			data->_Entities.EmplaceSlow(DynamicArray<Entity *RESTRICT>());

			//Calculate the axis aligned bounding box.
			const Vector3<float> wantedGridPointWorldPosition{ GridPoint2::GridPointToWorldPosition(wantedGridPoint, data->_GridCellSize) };

			AxisAlignedBoundingBox box;

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
*	Updates the given three dimensional entity placement data.
*/
void EntityPlacementSystem::UpdateThreeDimensionalEntityPlacementData(EntityPlacementData *const RESTRICT data) NOEXCEPT
{
	//Calculate the current grid point based on the perceiver's position.
	const Vector3<float> perceiverPosition{ Perceiver::Instance->GetPosition() };
}