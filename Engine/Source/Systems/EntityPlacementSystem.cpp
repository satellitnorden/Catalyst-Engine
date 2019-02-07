//Header file.
#include <Systems/EntityPlacementSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Systems.
#include <Systems/EntityCreationSystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EntityPlacementSystem);

/*
*	Initializes the entity placement system.
*/
void EntityPlacementSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the procedural placement update task.
	_PlacementUpdateTask._Function = [](void *const RESTRICT)
	{
		EntityPlacementSystem::Instance->UpdatePlacement();
	};
	_PlacementUpdateTask._Arguments = nullptr;
}

/*
*	Updates the entity placement system sequentially.
*/
void EntityPlacementSystem::SequentialUpdateSystemSynchronous() NOEXCEPT
{
	//If the placement update has finished, fire off another update.
	if (_PlacementUpdateTask.IsExecuted())
	{
		//Fire off another update.
		TaskSystem::Instance->ExecuteTask(&_PlacementUpdateTask);
	}
}

/*
*	Registers a two dimensional placement function.
*/
void EntityPlacementSystem::RegisterTwoDimensionalPlacementFunction(EntityPlacementFunction function, const float gridSize) NOEXCEPT
{
	//Create the new placement data.
	_TwoDimensionalPlacementData.EmplaceSlow();
	EntityTwoDimensionalPlacementData *const RESTRICT data{ &_TwoDimensionalPlacementData.Back() };

	//Fill in the details.
	data->_Function = function;
	data->_GridSize = gridSize;

	//Reset all grid points.
	for (EntityTwoDimensionalGridPoint &gridPoint : data->_GridPoints)
	{
		gridPoint._Valid = false;
	}
}

/*
*	Registers a three dimensional placement function.
*/
void EntityPlacementSystem::RegisterThreeDimensionalPlacementFunction(EntityPlacementFunction function, const float gridSize) NOEXCEPT
{
	//Create the new placement data.
	_ThreeDimensionalPlacementData.EmplaceSlow();
	EntityThreeDimensionalPlacementData *const RESTRICT data{ &_ThreeDimensionalPlacementData.Back() };

	//Fill in the details.
	data->_Function = function;
	data->_GridSize = gridSize;

	//Reset all grid points.
	for (EntityThreeDimensionalGridPoint &gridPoint : data->_GridPoints)
	{
		gridPoint._Valid = false;
	}
}

/*
*	Updates placement.
*/
void EntityPlacementSystem::UpdatePlacement() NOEXCEPT
{
	//Update two dimensional placement.
	UpdateTwoDimensionalPlacement();

	//Update three dimensional placement.
	UpdateThreeDimensionalPlacement();
}

/*
*	Updates two-dimensional placement.
*/
void EntityPlacementSystem::UpdateTwoDimensionalPlacement() NOEXCEPT
{
	//Cache the current perceiver position.
	const Vector3<float> perceiverPosition{ Perceiver::Instance->GetPosition() };

	//Update all two dimensional placement data.
	for (EntityTwoDimensionalPlacementData &data : _TwoDimensionalPlacementData)
	{
		//Calculate the current grid point based on the current perceiver position.
		const GridPoint2 currentGridPoint{ GridPoint2::WorldPositionToGridPoint(perceiverPosition, data._GridSize) };

		//Create an array with the valid grid positions.
		StaticArray<GridPoint2, 9> validGridPoints
		{
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y + 1),

			GridPoint2(currentGridPoint._X, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y + 1),

			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y + 1),
		};

		//Determine the grid points to invalidate.
		for (EntityTwoDimensionalGridPoint &gridPoint : data._GridPoints)
		{
			//If this grid point is already invalid, no need to invalidate it.
			if (!gridPoint._Valid)
			{
				continue;
			}

			bool valid{ false };

			for (const GridPoint2 &validGridPoint : validGridPoints)
			{
				if (gridPoint._GridPoint == validGridPoint)
				{
					valid = true;

					break;
				}
			}

			if (!valid)
			{
				//Invalidate this grid point.
				gridPoint._Valid = false;

				for (Entity *const RESTRICT entity : gridPoint._Entities)
				{
					EntityCreationSystem::Instance->RequestTermination(entity);
					EntityCreationSystem::Instance->RequestDestruction(entity);
				}

				gridPoint._Entities.ClearFast();
			}
		}

		//Determine the grid points to update.
		for (const GridPoint2 &validGridPoint : validGridPoints)
		{
			bool exists{ false };

			for (EntityTwoDimensionalGridPoint &gridPoint : data._GridPoints)
			{
				if (gridPoint._Valid && gridPoint._GridPoint == validGridPoint)
				{
					exists = true;

					break;
				}
			}

			if (!exists)
			{
				//Find the first invalid grid point and update it.
				for (EntityTwoDimensionalGridPoint &gridPoint : data._GridPoints)
				{
					if (!gridPoint._Valid)
					{
						gridPoint._Valid = true;
						gridPoint._GridPoint = validGridPoint;

						//Construct the axis aligned bounding box.
						const Vector3<float> currentGridPointWorldPosition{ GridPoint2::GridPointToWorldPosition(gridPoint._GridPoint, data._GridSize) };

						AxisAlignedBoundingBox box;

						box._Minimum._X = currentGridPointWorldPosition._X - (data._GridSize * 0.5f);
						box._Minimum._Y = 0.0f;
						box._Minimum._Z = currentGridPointWorldPosition._Z - (data._GridSize * 0.5f);

						box._Maximum._X = currentGridPointWorldPosition._X + (data._GridSize * 0.5f);
						box._Maximum._Y = 0.0f;
						box._Maximum._Z = currentGridPointWorldPosition._Z + (data._GridSize * 0.5f);

						data._Function(box, &gridPoint._Entities);

						break;
					}
				}
			}
		}
	}
}

/*
*	Updates three dimensional placement.
*/
void EntityPlacementSystem::UpdateThreeDimensionalPlacement() NOEXCEPT
{
	//Cache the current perceiver position.
	const Vector3<float> perceiverPosition{ Perceiver::Instance->GetPosition() };

	//Update all three dimensional placement data.
	for (EntityThreeDimensionalPlacementData &data : _ThreeDimensionalPlacementData)
	{
		//Calculate the current grid point based on the current perceiver position.
		const GridPoint3 currentGridPoint{ GridPoint3::WorldPositionToGridPoint(perceiverPosition, data._GridSize) };

		//Create an array with the valid grid positions.
		StaticArray<GridPoint3, 27> validGridPoints
		{
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y - 1, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y - 1, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y - 1, currentGridPoint._Z + 1),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y, currentGridPoint._Z + 1),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y + 1, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y + 1, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X - 1, currentGridPoint._Y + 1, currentGridPoint._Z + 1),

			GridPoint3(currentGridPoint._X, currentGridPoint._Y - 1, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y - 1, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y - 1, currentGridPoint._Z + 1),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y, currentGridPoint._Z + 1),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y + 1, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y + 1, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X, currentGridPoint._Y + 1, currentGridPoint._Z + 1),

			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y - 1, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y - 1, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y - 1, currentGridPoint._Z + 1),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y, currentGridPoint._Z + 1),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y + 1, currentGridPoint._Z - 1),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y + 1, currentGridPoint._Z),
			GridPoint3(currentGridPoint._X + 1, currentGridPoint._Y + 1, currentGridPoint._Z + 1),
		};

		//Determine the grid points to invalidate.
		for (EntityThreeDimensionalGridPoint &gridPoint : data._GridPoints)
		{
			//If this grid point is already invalid, no need to invalidate it.
			if (!gridPoint._Valid)
			{
				continue;
			}

			bool valid{ false };

			for (const GridPoint3 &validGridPoint : validGridPoints)
			{
				if (gridPoint._GridPoint == validGridPoint)
				{
					valid = true;

					break;
				}
			}

			if (!valid)
			{
				//Invalidate this grid point.
				gridPoint._Valid = false;

				for (Entity *const RESTRICT entity : gridPoint._Entities)
				{
					EntityCreationSystem::Instance->RequestTermination(entity);
					EntityCreationSystem::Instance->RequestDestruction(entity);
				}

				gridPoint._Entities.ClearFast();
			}
		}

		//Determine the grid points to update.
		for (const GridPoint3 &validGridPoint : validGridPoints)
		{
			bool exists{ false };

			for (EntityThreeDimensionalGridPoint &gridPoint : data._GridPoints)
			{
				if (gridPoint._Valid && gridPoint._GridPoint == validGridPoint)
				{
					exists = true;

					break;
				}
			}

			if (!exists)
			{
				//Find the first invalid grid point and update it.
				for (EntityThreeDimensionalGridPoint &gridPoint : data._GridPoints)
				{
					if (!gridPoint._Valid)
					{
						gridPoint._Valid = true;
						gridPoint._GridPoint = validGridPoint;

						//Construct the axis aligned bounding box.
						const Vector3<float> currentGridPointWorldPosition{ GridPoint3::GridPointToWorldPosition(gridPoint._GridPoint, data._GridSize) };

						AxisAlignedBoundingBox box;

						box._Minimum._X = currentGridPointWorldPosition._X - (data._GridSize * 0.5f);
						box._Minimum._Y = currentGridPointWorldPosition._Y - (data._GridSize * 0.5f);
						box._Minimum._Z = currentGridPointWorldPosition._Z - (data._GridSize * 0.5f);

						box._Maximum._X = currentGridPointWorldPosition._X + (data._GridSize * 0.5f);
						box._Maximum._Y = currentGridPointWorldPosition._Y + (data._GridSize * 0.5f);
						box._Maximum._Z = currentGridPointWorldPosition._Z + (data._GridSize * 0.5f);

						data._Function(box, &gridPoint._Entities);

						break;
					}
				}
			}
		}
	}
}