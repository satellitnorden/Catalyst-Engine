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
	for (uint64 i = 0, size = _PatchInformations.Size(); i < size; ++i)
	{
		for (uint64 j = 0, size = _PatchInformations[i].Size(); j < size; ++j)
		{
			_PatchInformations[i][j]._Valid = false;
			_PatchRenderInformations[i][j]._Visibility = VisibilityFlag::None;
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
	//Cache the current viewer position.
	const Vector3 viewerPosition{ Viewer::Instance->GetPosition() };

	//Calculate the grid point for the current viewer position.
	const GridPoint2 currentGridPoint{ GridPoint2::WorldPositionToGridPoint(viewerPosition, _Properties._PatchSize) };

	//If the last grid point has changed, generate a new set of patches.
	if (currentGridPoint != _LastGridPoint)
	{
		//Check if the current asynchronous informations are valid, and if so, invalidate them.
		for (uint64 i = 0, size = _PatchInformations[_CurrentAsynchronousBuffer].Size(); i < size; ++i)
		{
			if (_PatchInformations[_CurrentAsynchronousBuffer][i]._Valid)
			{
				_PatchInformations[_CurrentAsynchronousBuffer][i]._Valid = false;
				_PatchRenderInformations[_CurrentAsynchronousBuffer][i]._Visibility = VisibilityFlag::None;

				RenderingSystem::Instance->DestroyConstantBuffer(_PatchRenderInformations[_CurrentAsynchronousBuffer][i]._Buffer);
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
		GenerateHighDetailPatch(validHighDetailGridPoints[0], TerrainAxis::NegativeX | TerrainAxis::NegativeY, &_PatchInformations[_CurrentAsynchronousBuffer][0], &_PatchRenderInformations[_CurrentAsynchronousBuffer][0]);
		GenerateHighDetailPatch(validHighDetailGridPoints[1], TerrainAxis::NegativeY, &_PatchInformations[_CurrentAsynchronousBuffer][1], &_PatchRenderInformations[_CurrentAsynchronousBuffer][1]);
		GenerateHighDetailPatch(validHighDetailGridPoints[2], TerrainAxis::PositiveX | TerrainAxis::NegativeY, &_PatchInformations[_CurrentAsynchronousBuffer][2], &_PatchRenderInformations[_CurrentAsynchronousBuffer][2]);

		GenerateHighDetailPatch(validHighDetailGridPoints[3], TerrainAxis::NegativeX, &_PatchInformations[_CurrentAsynchronousBuffer][3], &_PatchRenderInformations[_CurrentAsynchronousBuffer][3]);
		GenerateHighDetailPatch(validHighDetailGridPoints[4], TerrainAxis::None, &_PatchInformations[_CurrentAsynchronousBuffer][4], &_PatchRenderInformations[_CurrentAsynchronousBuffer][4]);
		GenerateHighDetailPatch(validHighDetailGridPoints[5], TerrainAxis::PositiveX, &_PatchInformations[_CurrentAsynchronousBuffer][5], &_PatchRenderInformations[_CurrentAsynchronousBuffer][5]);

		GenerateHighDetailPatch(validHighDetailGridPoints[6], TerrainAxis::NegativeX | TerrainAxis::PositiveY, &_PatchInformations[_CurrentAsynchronousBuffer][6], &_PatchRenderInformations[_CurrentAsynchronousBuffer][6]);
		GenerateHighDetailPatch(validHighDetailGridPoints[7], TerrainAxis::PositiveY, &_PatchInformations[_CurrentAsynchronousBuffer][7], &_PatchRenderInformations[_CurrentAsynchronousBuffer][7]);
		GenerateHighDetailPatch(validHighDetailGridPoints[8], TerrainAxis::PositiveX | TerrainAxis::PositiveY, &_PatchInformations[_CurrentAsynchronousBuffer][8], &_PatchRenderInformations[_CurrentAsynchronousBuffer][8]);

		//Generate all low detail patches.
		GenerateLowDetailPatches(currentGridPoint, 3, 0);
		GenerateLowDetailPatches(currentGridPoint, 9, 1);
		GenerateLowDetailPatches(currentGridPoint, 27, 2);
		GenerateLowDetailPatches(currentGridPoint, 81, 3);
		GenerateLowDetailPatches(currentGridPoint, 243, 4);
		GenerateLowDetailPatches(currentGridPoint, 729, 5);
		GenerateLowDetailPatches(currentGridPoint, 2'187, 6);

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
	GenerateLowDetailPatch(validLowDetailGridPoints[0], TerrainAxis::NegativeX | TerrainAxis::NegativeY, static_cast<float>(gridPointOffset), &_PatchInformations[_CurrentAsynchronousBuffer][0 + (layer * 8) + 9], &_PatchRenderInformations[_CurrentAsynchronousBuffer][0 + (layer * 8) + 9]);
	GenerateLowDetailPatch(validLowDetailGridPoints[1], TerrainAxis::NegativeY, static_cast<float>(gridPointOffset), &_PatchInformations[_CurrentAsynchronousBuffer][1 + (layer * 8) + 9], &_PatchRenderInformations[_CurrentAsynchronousBuffer][1 + (layer * 8) + 9]);
	GenerateLowDetailPatch(validLowDetailGridPoints[2], TerrainAxis::PositiveX | TerrainAxis::NegativeY, static_cast<float>(gridPointOffset), &_PatchInformations[_CurrentAsynchronousBuffer][2 + (layer * 8) + 9], &_PatchRenderInformations[_CurrentAsynchronousBuffer][2 + (layer * 8) + 9]);

	GenerateLowDetailPatch(validLowDetailGridPoints[3], TerrainAxis::NegativeX, static_cast<float>(gridPointOffset), &_PatchInformations[_CurrentAsynchronousBuffer][3 + (layer * 8) + 9], &_PatchRenderInformations[_CurrentAsynchronousBuffer][3 + (layer * 8) + 9]);
	GenerateLowDetailPatch(validLowDetailGridPoints[4], TerrainAxis::PositiveX, static_cast<float>(gridPointOffset), &_PatchInformations[_CurrentAsynchronousBuffer][4 + (layer * 8) + 9], &_PatchRenderInformations[_CurrentAsynchronousBuffer][4 + (layer * 8) + 9]);

	GenerateLowDetailPatch(validLowDetailGridPoints[5], TerrainAxis::NegativeX | TerrainAxis::PositiveY, static_cast<float>(gridPointOffset), &_PatchInformations[_CurrentAsynchronousBuffer][5 + (layer * 8) + 9], &_PatchRenderInformations[_CurrentAsynchronousBuffer][5 + (layer * 8) + 9]);
	GenerateLowDetailPatch(validLowDetailGridPoints[6], TerrainAxis::PositiveY, static_cast<float>(gridPointOffset), &_PatchInformations[_CurrentAsynchronousBuffer][6 + (layer * 8) + 9], &_PatchRenderInformations[_CurrentAsynchronousBuffer][6 + (layer * 8) + 9]);
	GenerateLowDetailPatch(validLowDetailGridPoints[7], TerrainAxis::PositiveX | TerrainAxis::PositiveY, static_cast<float>(gridPointOffset), &_PatchInformations[_CurrentAsynchronousBuffer][7 + (layer * 8) + 9], &_PatchRenderInformations[_CurrentAsynchronousBuffer][7 + (layer * 8) + 9]);
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
	patchRenderInformation->_Visibility = VisibilityFlag::None;
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
	patchRenderInformation->_Visibility = VisibilityFlag::None;
	patchRenderInformation->_HighDetail = false;
	patchRenderInformation->_Buffer = RenderingSystem::Instance->CreateConstantBuffer(bufferData.Data(), bufferDataSizes.Data(), 2);
	patchRenderInformation->_IndexOffset = bufferDataSizes[0];
	patchRenderInformation->_IndexCount = static_cast<uint32>(indices.Size());
	patchRenderInformation->_DisplacementInformation = displacementInformation;
}