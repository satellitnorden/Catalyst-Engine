//Header file.
#include <Systems/TerrainSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Entities.
#include <Entities/CameraEntity.h>

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
	_Properties._PatchResolution = configuration._PatchResolution;
	_Properties._PatchSize = configuration._PatchSize;
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
	for (uint64 i = 0, size = _HighDetailPatchInformations.Size(); i < size; ++i)
	{
		for (uint64 j = 0, size = _HighDetailPatchInformations[i].Size(); j < size; ++j)
		{
			_HighDetailPatchInformations[i][j]._Valid = false;
			_HighDetailPatchRenderInformations[i][j]._Draw = false;
		}
	}
}

/*
*	Updates the terrain system sequentially.
*/
void TerrainSystem::SequentialUpdateSystemSynchronous() NOEXCEPT
{
	//Check if the asynchronous update has finished,
	if (_UpdateTask.IsExecuted())
	{
		//If the current grid point has been updated, flip the buffers.
		if (_LastGridPoint != _CurrentGridPoint)
		{
			_CurrentSynchronousBuffer ^= static_cast<uint8>(1);
			_CurrentAsynchronousBuffer ^= static_cast<uint8>(1);

			_LastGridPoint = _CurrentGridPoint;
		}

		//Cache the current camera position.
		const CameraEntity *const RESTRICT camera{ RenderingSystem::Instance->GetActiveCamera() };
		_CurrentCameraPosition = camera->GetPosition();

		//Fire off another asynchronous update.
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
*	Updates the terrain system asynchronously.
*/
void TerrainSystem::UpdateSystemAsynchronous() NOEXCEPT
{
	//Calculate the grid point for the current camera position.
	const GridPoint2 currentGridPoint{ GridPoint2::WorldPositionToGridPoint(_CurrentCameraPosition, _Properties._PatchSize) };

	//If the last grid point has changed, generate a new set of patches.
	if (currentGridPoint != _LastGridPoint)
	{
		//Check if the current asynchronous informations are valid, and if so, invalidate them.
		for (uint64 i = 0, size = _HighDetailPatchInformations[_CurrentAsynchronousBuffer].Size(); i < size; ++i)
		{
			if (_HighDetailPatchInformations[_CurrentAsynchronousBuffer][i]._Valid)
			{
				_HighDetailPatchInformations[_CurrentAsynchronousBuffer][i]._Valid = false;
				_HighDetailPatchRenderInformations[_CurrentAsynchronousBuffer][i]._Draw = false;

				RenderingSystem::Instance->DestroyConstantBuffer(_HighDetailPatchRenderInformations[_CurrentAsynchronousBuffer][i]._Buffer);
			}
		}

		for (uint64 i = 0, size = _LowDetailPatchInformations[_CurrentAsynchronousBuffer].Size(); i < size; ++i)
		{
			if (_LowDetailPatchInformations[_CurrentAsynchronousBuffer][i]._Valid)
			{
				_LowDetailPatchInformations[_CurrentAsynchronousBuffer][i]._Valid = false;
				_LowDetailPatchRenderInformations[_CurrentAsynchronousBuffer][i]._Draw = false;

				RenderingSystem::Instance->DestroyConstantBuffer(_LowDetailPatchRenderInformations[_CurrentAsynchronousBuffer][i]._Buffer);
			}
		}

		//Create an array with the valid high detail grid points.
		StaticArray<GridPoint2, 9> validHighDetailGridPoints
		{
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y - 1),

			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y),

			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y + 1),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y + 1),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y + 1),
		};

		//Generate all high detail patches.
		GenerateHighDetailPatch(validHighDetailGridPoints[0], TerrainAxis::NegativeX | TerrainAxis::NegativeY, &_HighDetailPatchInformations[_CurrentAsynchronousBuffer][0], &_HighDetailPatchRenderInformations[_CurrentAsynchronousBuffer][0]);
		GenerateHighDetailPatch(validHighDetailGridPoints[1], TerrainAxis::NegativeY, &_HighDetailPatchInformations[_CurrentAsynchronousBuffer][1], &_HighDetailPatchRenderInformations[_CurrentAsynchronousBuffer][1]);
		GenerateHighDetailPatch(validHighDetailGridPoints[2], TerrainAxis::PositiveX | TerrainAxis::NegativeY, &_HighDetailPatchInformations[_CurrentAsynchronousBuffer][2], &_HighDetailPatchRenderInformations[_CurrentAsynchronousBuffer][2]);

		GenerateHighDetailPatch(validHighDetailGridPoints[3], TerrainAxis::NegativeX, &_HighDetailPatchInformations[_CurrentAsynchronousBuffer][3], &_HighDetailPatchRenderInformations[_CurrentAsynchronousBuffer][3]);
		GenerateHighDetailPatch(validHighDetailGridPoints[4], TerrainAxis::None, &_HighDetailPatchInformations[_CurrentAsynchronousBuffer][4], &_HighDetailPatchRenderInformations[_CurrentAsynchronousBuffer][4]);
		GenerateHighDetailPatch(validHighDetailGridPoints[5], TerrainAxis::PositiveX, &_HighDetailPatchInformations[_CurrentAsynchronousBuffer][5], &_HighDetailPatchRenderInformations[_CurrentAsynchronousBuffer][5]);

		GenerateHighDetailPatch(validHighDetailGridPoints[6], TerrainAxis::NegativeX | TerrainAxis::PositiveY, &_HighDetailPatchInformations[_CurrentAsynchronousBuffer][6], &_HighDetailPatchRenderInformations[_CurrentAsynchronousBuffer][6]);
		GenerateHighDetailPatch(validHighDetailGridPoints[7], TerrainAxis::PositiveY, &_HighDetailPatchInformations[_CurrentAsynchronousBuffer][7], &_HighDetailPatchRenderInformations[_CurrentAsynchronousBuffer][7]);
		GenerateHighDetailPatch(validHighDetailGridPoints[8], TerrainAxis::PositiveX | TerrainAxis::PositiveY, &_HighDetailPatchInformations[_CurrentAsynchronousBuffer][8], &_HighDetailPatchRenderInformations[_CurrentAsynchronousBuffer][8]);

		//Generate all low detail patches.
		GenerateLowDetailPatches(currentGridPoint, 3, 0);
		GenerateLowDetailPatches(currentGridPoint, 9, 1);
		GenerateLowDetailPatches(currentGridPoint, 27, 2);
		GenerateLowDetailPatches(currentGridPoint, 81, 3);
		GenerateLowDetailPatches(currentGridPoint, 243, 4);
		GenerateLowDetailPatches(currentGridPoint, 729, 5);
		GenerateLowDetailPatches(currentGridPoint, 2'187, 6);
		GenerateLowDetailPatches(currentGridPoint, 6'561, 7);

		//Update the current grid point.
		_CurrentGridPoint = currentGridPoint;
	}
}

/*
*	Generates low detail patches.
*/
void TerrainSystem::GenerateLowDetailPatches(const GridPoint2 &currentGridPoint, const int32 gridPointOffset, const uint8 layer) NOEXCEPT
{
	//Create an array with the valid high detail grid points.
	StaticArray<GridPoint2, 8> validLowDetailGridPoints
	{
		GridPoint2(currentGridPoint._X - gridPointOffset, currentGridPoint._Y - gridPointOffset),
		GridPoint2(currentGridPoint._X, currentGridPoint._Y - gridPointOffset),
		GridPoint2(currentGridPoint._X + gridPointOffset, currentGridPoint._Y - gridPointOffset),

		GridPoint2(currentGridPoint._X - gridPointOffset, currentGridPoint._Y),
		GridPoint2(currentGridPoint._X + gridPointOffset, currentGridPoint._Y),

		GridPoint2(currentGridPoint._X - gridPointOffset, currentGridPoint._Y + gridPointOffset),
		GridPoint2(currentGridPoint._X, currentGridPoint._Y + gridPointOffset),
		GridPoint2(currentGridPoint._X + gridPointOffset, currentGridPoint._Y + gridPointOffset),
	};

	//Generate all low detail patches.
	GenerateLowDetailPatch(validLowDetailGridPoints[0], TerrainAxis::NegativeX | TerrainAxis::NegativeY, static_cast<float>(gridPointOffset), &_LowDetailPatchInformations[_CurrentAsynchronousBuffer][0 + (layer * 8)], &_LowDetailPatchRenderInformations[_CurrentAsynchronousBuffer][0 + (layer * 8)]);
	GenerateLowDetailPatch(validLowDetailGridPoints[1], TerrainAxis::NegativeY, static_cast<float>(gridPointOffset), &_LowDetailPatchInformations[_CurrentAsynchronousBuffer][1 + (layer * 8)], &_LowDetailPatchRenderInformations[_CurrentAsynchronousBuffer][1 + (layer * 8)]);
	GenerateLowDetailPatch(validLowDetailGridPoints[2], TerrainAxis::PositiveX | TerrainAxis::NegativeY, static_cast<float>(gridPointOffset), &_LowDetailPatchInformations[_CurrentAsynchronousBuffer][2 + (layer * 8)], &_LowDetailPatchRenderInformations[_CurrentAsynchronousBuffer][2 + (layer * 8)]);

	GenerateLowDetailPatch(validLowDetailGridPoints[3], TerrainAxis::NegativeX, static_cast<float>(gridPointOffset), &_LowDetailPatchInformations[_CurrentAsynchronousBuffer][3 + (layer * 8)], &_LowDetailPatchRenderInformations[_CurrentAsynchronousBuffer][3 + (layer * 8)]);
	GenerateLowDetailPatch(validLowDetailGridPoints[4], TerrainAxis::PositiveX, static_cast<float>(gridPointOffset), &_LowDetailPatchInformations[_CurrentAsynchronousBuffer][4 + (layer * 8)], &_LowDetailPatchRenderInformations[_CurrentAsynchronousBuffer][4 + (layer * 8)]);

	GenerateLowDetailPatch(validLowDetailGridPoints[5], TerrainAxis::NegativeX | TerrainAxis::PositiveY, static_cast<float>(gridPointOffset), &_LowDetailPatchInformations[_CurrentAsynchronousBuffer][5 + (layer * 8)], &_LowDetailPatchRenderInformations[_CurrentAsynchronousBuffer][5 + (layer * 8)]);
	GenerateLowDetailPatch(validLowDetailGridPoints[6], TerrainAxis::PositiveY, static_cast<float>(gridPointOffset), &_LowDetailPatchInformations[_CurrentAsynchronousBuffer][6 + (layer * 8)], &_LowDetailPatchRenderInformations[_CurrentAsynchronousBuffer][6 + (layer * 8)]);
	GenerateLowDetailPatch(validLowDetailGridPoints[7], TerrainAxis::PositiveX | TerrainAxis::PositiveY, static_cast<float>(gridPointOffset), &_LowDetailPatchInformations[_CurrentAsynchronousBuffer][7 + (layer * 8)], &_LowDetailPatchRenderInformations[_CurrentAsynchronousBuffer][7 + (layer * 8)]);
}

/*
*	Generates a new high detail patch at the specified grid point.
*/
void TerrainSystem::GenerateHighDetailPatch(const GridPoint2 &gridPoint, const TerrainAxis borders, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT
{
	//Calculate the world position of the grid point.
	const Vector3 gridPointWorldPosition{ GridPoint2::GridPointToWorldPosition(gridPoint, _Properties._PatchSize) };

	//Generate the terrain plane.
	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainUtilities::GenerateTerrainPlane(	_Properties,
											_Properties._PatchResolution,
											gridPointWorldPosition,
											_Properties._PatchSize,
											borders,
											&vertices,
											&indices);

	//Get the material and the displacement information.
	TerrainMaterial material;
	TerrainDisplacementInformation displacementInformation;

	_Properties._PatchPropertiesGenerationFunction(_Properties, gridPointWorldPosition, &material, &displacementInformation);

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

	patchInformation->_AxisAlignedBoundingBox._Minimum = Vector3(gridPointWorldPosition._X - (_Properties._PatchSize * 0.5f), minimumHeight, gridPointWorldPosition._Z - (_Properties._PatchSize * 0.5f));
	patchInformation->_AxisAlignedBoundingBox._Maximum = Vector3(gridPointWorldPosition._X + (_Properties._PatchSize * 0.5f), maximumHeight, gridPointWorldPosition._Z + (_Properties._PatchSize * 0.5f));

	//Fill in the details about the patch render information.
	patchRenderInformation->_Draw = true;
	patchRenderInformation->_HighDetail = true;
	patchRenderInformation->_Buffer = RenderingSystem::Instance->CreateConstantBuffer(bufferData.Data(), bufferDataSizes.Data(), 2);
	patchRenderInformation->_IndexOffset = bufferDataSizes[0];
	patchRenderInformation->_IndexCount = static_cast<uint32>(indices.Size());
	patchRenderInformation->_DisplacementInformation = displacementInformation;
}

/*
*	Generates a new low detail patch at the specified grid point.
*/
void TerrainSystem::GenerateLowDetailPatch(const GridPoint2 &gridPoint, const TerrainAxis borders, const float patchSizeMultiplier, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT
{
	//Calculate the world position of the grid point.
	const Vector3 gridPointWorldPosition{ GridPoint2::GridPointToWorldPosition(gridPoint, _Properties._PatchSize) };

	//Generate the terrain plane.
	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainUtilities::GenerateTerrainPlane(	_Properties,
											_Properties._PatchResolution,
											gridPointWorldPosition,
											_Properties._PatchSize * patchSizeMultiplier,
											borders,
											&vertices,
											&indices);

	//Get the material and the displacement information.
	TerrainMaterial material;
	TerrainDisplacementInformation displacementInformation;

	_Properties._PatchPropertiesGenerationFunction(_Properties, gridPointWorldPosition, &material, &displacementInformation);

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

	patchInformation->_AxisAlignedBoundingBox._Minimum = Vector3(gridPointWorldPosition._X - (_Properties._PatchSize * patchSizeMultiplier * 0.5f), minimumHeight, gridPointWorldPosition._Z - (_Properties._PatchSize * patchSizeMultiplier * 0.5f));
	patchInformation->_AxisAlignedBoundingBox._Maximum = Vector3(gridPointWorldPosition._X + (_Properties._PatchSize * patchSizeMultiplier * 0.5f), maximumHeight, gridPointWorldPosition._Z + (_Properties._PatchSize * patchSizeMultiplier * 0.5f));

	//Fill in the details about the patch render information.
	patchRenderInformation->_Draw = true;
	patchRenderInformation->_HighDetail = false;
	patchRenderInformation->_Buffer = RenderingSystem::Instance->CreateConstantBuffer(bufferData.Data(), bufferDataSizes.Data(), 2);
	patchRenderInformation->_IndexOffset = bufferDataSizes[0];
	patchRenderInformation->_IndexCount = static_cast<uint32>(indices.Size());
	patchRenderInformation->_DisplacementInformation = displacementInformation;
}