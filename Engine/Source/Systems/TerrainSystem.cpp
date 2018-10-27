//Header file.
#include <Systems/TerrainSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/CatalystProjectConfiguration.h>


//Rendering.
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Terrain.
#include <Terrain/TerrainUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(TerrainSystem);

/*
*	Initializes the terrain system.
*/
void TerrainSystem::InitializeSystem(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT
{
	//Copy over the relevant properties.
	_Properties._HeightGenerationFunction = configuration._HeightGenerationFunction;
	_Properties._LayerWeightsGenerationFunction = configuration._LayerWeightsGenerationFunction;
	_Properties._PatchPropertiesGenerationFunction = configuration._PatchPropertiesGenerationFunction;

	//Set the function for the update task.
	_UpdateTask._Function = [](void *const RESTRICT)
	{
		TerrainSystem::Instance->UpdateSystemAsynchronous();
	};
	_UpdateTask._Arguments = nullptr;

	//Set all the patch and patch render informations to their default state.
	for (uint64 i = 0, size = _PatchInformations.Size(); i < size; ++i)
	{
		_PatchInformations[i]._Valid = false;
		_PatchRenderInformations[i]._Visibility = VisibilityFlag::None;
	}

	//Generate the starting schedule.
	_Schedule.GenerateNewSchedule(GridPoint2(0, 0), GridPoint2(0, 0));
}

/*
*	Updates the terrain system sequentially.
*/
void TerrainSystem::SequentialUpdateSystemSynchronous() NOEXCEPT
{
	//Check if the asynchronous update has finished,
	if (_UpdateTask.IsExecuted())
	{
		//Process the update.
		ProcessUpdate();

		//Fire off another asynchronous update.
		TaskSystem::Instance->ExecuteTask(&_UpdateTask);
	}
}

/*
*	Returns the terrain height at the given position.
*/
bool TerrainSystem::GetTerrainHeightAtPosition(const Vector3 &position, float *const RESTRICT height) const NOEXCEPT
{
	//Just ask the height generation function what the height at the position are.
	_Properties._HeightGenerationFunction(_Properties, position, height);

	//Return that the retrieval succeeded.
	return true;
}

/*
*	Returns the terrain normal at the given position.
*/
bool TerrainSystem::GetTerrainNormalAtPosition(const Vector3 &position, Vector3 *const RESTRICT normal) const NOEXCEPT
{
	//Generate a normal at the position.
	TerrainUtilities::GenerateNormal(_Properties, position, normal);

	//Return that the retrieval succeeded.
	return true;
}

/*
*	Processes the update.
*/
void TerrainSystem::ProcessUpdate() NOEXCEPT
{
	if (_Update._Index != -1)
	{
		if (_PatchInformations[_Update._Index]._Valid)
		{
			RenderingSystem::Instance->DestroyTexture2D(_PatchInformations[_Update._Index]._NormalTexture);
			RenderingSystem::Instance->DestroyRenderDataTable(_PatchRenderInformations[_Update._Index]._NormalRenderDataTable);
			RenderingSystem::Instance->DestroyConstantBuffer(_PatchRenderInformations[_Update._Index]._Buffer);
		}

		_PatchInformations[_Update._Index] = _Update._PatchInformation;
		_PatchRenderInformations[_Update._Index] = _Update._PatchRenderInformation;
	}
}

/*
*	Updates the terrain system asynchronously.
*/
void TerrainSystem::UpdateSystemAsynchronous() NOEXCEPT
{
	//Reset the update.
	_Update._Index = -1;

	switch (_State)
	{
		case TerrainSystemState::Starting:
		{
			//Follow the schedule.
			FollowSchedule();

			break;
		}

		case TerrainSystemState::Idling:
		{
			//Get the current viewer position.
			const Vector3 viewerPosition{ Viewer::Instance->GetPosition() };

			//Calculate the current grid point.
			_CurrentGridPoint = GridPoint2::WorldPositionToGridPoint(viewerPosition, TerrainConstants::TERRAIN_PATCH_SIZE);

			//If the grid point changed, generate a new schedule.
			if (_LastGridPoint != _CurrentGridPoint)
			{
				_Schedule.GenerateNewSchedule(_LastGridPoint, _CurrentGridPoint);

				if (_LastGridPoint._X != _CurrentGridPoint._X)
				{
					_LastGridPoint._X = _CurrentGridPoint._X;
				}

				else
				{
					_LastGridPoint._Y = _CurrentGridPoint._Y;
				}

				_State = TerrainSystemState::Updating;

				//Follow the schedule.
				FollowSchedule();
			}

			break;
		}

		case TerrainSystemState::Updating:
		{
			//Follow the schedule.
			FollowSchedule();

			break;
		}
	}
}

/*
*	Follows the schedule.
*/
void TerrainSystem::FollowSchedule() NOEXCEPT
{
	//Get the next schedule item.
	const TerrainScheduleItem& scheduleItem{ _Schedule.GetNextScheduleItem() };

	//Update the update.
	_Update._Index = scheduleItem._Index;

	//Generate a new patch.
	GeneratePatch(	scheduleItem._GridPoint,
					scheduleItem._Borders,
					scheduleItem._PatchSizeMultiplier,
					scheduleItem._NormalResolutionMultiplier,
					&_Update._PatchInformation,
					&_Update._PatchRenderInformation);

	//If the schedule is done, begin idling.
	if (_Schedule.IsDone())
	{
		_State = TerrainSystemState::Idling;
	}
}

/*
*	Generates a patch.
*/
void TerrainSystem::GeneratePatch(const GridPoint2 &gridPoint, const TerrainBorder borders, const float patchSizeMultiplier, const uint8 normalResolutionMultiplier, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT
{
	//Calculate the world position of the grid point.
	const Vector3 gridPointWorldPosition{ GridPoint2::GridPointToWorldPosition(gridPoint, TerrainConstants::TERRAIN_PATCH_SIZE) };

	//Generate the terrain plane.
	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainUtilities::GenerateTerrainPlane(	_Properties,
											gridPointWorldPosition,
											patchSizeMultiplier,
											borders,
											&vertices,
											&indices);

	//Get the material and the displacement information.
	TerrainMaterial material;

	_Properties._PatchPropertiesGenerationFunction(_Properties, gridPointWorldPosition, &material);

	//Create the constant buffer.
	StaticArray<void *RESTRICT, 2> bufferData;

	bufferData[0] = vertices.Data();
	bufferData[1] = indices.Data();

	StaticArray<uint64, 2> bufferDataSizes;

	bufferDataSizes[0] = sizeof(TerrainVertex) * vertices.Size();
	bufferDataSizes[1] = sizeof(uint32) * indices.Size();

	//Fill in the details about the patch information.
	patchInformation->_Valid = true;
	patchInformation->_GridPoint = gridPoint;

	float minimumHeight;
	float maximumHeight;

	TerrainUtilities::FindMinimumMaximumHeight(vertices, &minimumHeight, &maximumHeight);

	patchInformation->_AxisAlignedBoundingBox._Minimum = Vector3(gridPointWorldPosition._X - (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f), minimumHeight, gridPointWorldPosition._Z - (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f));
	patchInformation->_AxisAlignedBoundingBox._Maximum = Vector3(gridPointWorldPosition._X + (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f), maximumHeight, gridPointWorldPosition._Z + (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f));

	//Fill in the details about the patch render information.
	patchRenderInformation->_Visibility = VisibilityFlag::None;
	patchRenderInformation->_WorldPosition = Vector2(gridPointWorldPosition._X, gridPointWorldPosition._Z);
	patchRenderInformation->_PatchSize = TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier;
	patchRenderInformation->_Buffer = RenderingSystem::Instance->CreateConstantBuffer(bufferData.Data(), bufferDataSizes.Data(), 2);
	patchRenderInformation->_IndexOffset = bufferDataSizes[0];
	patchRenderInformation->_IndexCount = static_cast<uint32>(indices.Size());

	TerrainUtilities::GenerateNormalTexture(	_Properties,
												patchSizeMultiplier,
												normalResolutionMultiplier,
												gridPointWorldPosition,
												&patchInformation->_NormalTexture,
												&patchRenderInformation->_NormalRenderDataTable);
}