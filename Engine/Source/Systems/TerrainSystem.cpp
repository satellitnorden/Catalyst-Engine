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
	_PatchInformations.UpsizeFast(TerrainConstants::NUMBER_OF_TERRAIN_PATCHES);
	_PatchRenderInformations.UpsizeFast(TerrainConstants::NUMBER_OF_TERRAIN_PATCHES);

	for (uint64 i = 0, size = _PatchInformations.Size(); i < size; ++i)
	{
		_PatchInformations[i]._Valid = false;
		_PatchRenderInformations[i]._Visibility = VisibilityFlag::None;
	}

	//Generate the terrain plane.
	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainUtilities::GenerateTerrainPlane(	&vertices,
											&indices);

	StaticArray<void *RESTRICT, 2> bufferData;

	bufferData[0] = vertices.Data();
	bufferData[1] = indices.Data();

	StaticArray<uint64, 2> bufferDataSizes;

	bufferDataSizes[0] = sizeof(TerrainVertex) * vertices.Size();
	bufferDataSizes[1] = sizeof(uint32) * indices.Size();

	_Properties._Buffer = RenderingSystem::Instance->CreateConstantBuffer(bufferData.Data(), bufferDataSizes.Data(), 2);
	_Properties._IndexOffset = bufferDataSizes[0];
	_Properties._IndexCount = static_cast<uint32>(indices.Size());

	//Initialize the quad tree.
	_QuadTree.Initialize();
}

/*
*	Updates the terrain system sequentially.
*/
void TerrainSystem::SequentialUpdateSystemSynchronous() NOEXCEPT
{
	//Check if the asynchronous update has finished.
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
	switch (_Update._Type)
	{
		case TerrainUpdate::Type::AddRootNode:
		{
			for (uint64 i{ 0 }, size{ _QuadTree._RootGridPoints.Size() }; i < size; ++i)
			{
				if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
				{
					_QuadTree._RootGridPoints[i] = _Update._AddRootNodeUpdate._GridPoint;
					_QuadTree._RootNodes[i]._Identifier = _Update._AddRootNodeUpdate._PatchInformation._Identifier;

					break;
				}
			}

			_PatchInformations.EmplaceSlow(_Update._AddRootNodeUpdate._PatchInformation);
			_PatchRenderInformations.EmplaceSlow(_Update._AddRootNodeUpdate._PatchRenderInformation);

			break;
		}

		case TerrainUpdate::Type::RemoveRootNode:
		{
			_QuadTree._RootGridPoints[_Update._RemoveRootNodeUpdate._Index] = GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM);

			for (uint64 i{ 0 }, size{ _PatchInformations.Size() }; i < size; ++i)
			{
				if (_PatchInformations[i]._Identifier == _QuadTree._RootNodes[_Update._RemoveRootNodeUpdate._Index]._Identifier)
				{
					RenderingSystem::Instance->DestroyTexture2D(_PatchInformations[i]._HeightTexture);
					RenderingSystem::Instance->DestroyTexture2D(_PatchInformations[i]._NormalTexture);
					RenderingSystem::Instance->DestroyTexture2D(_PatchInformations[i]._LayerWeightsTexture);
					RenderingSystem::Instance->DestroyRenderDataTable(_PatchRenderInformations[i]._RenderDataTable);

					_PatchInformations.EraseAt(i);
					_PatchRenderInformations.EraseAt(i);

					break;
				}
			}

			break;
		}
	}
}

/*
*	Updates the terrain system asynchronously.
*/
void TerrainSystem::UpdateSystemAsynchronous() NOEXCEPT
{
	//Reset the update.
	_Update._Type = TerrainUpdate::Type::Invalid;

	//Get the current viewer position.
	const Vector3 viewerPosition{ Viewer::Instance->GetPosition() };

	//Calculate the viewer grid point.
	const GridPoint2 currentGridPoint{ GridPoint2::WorldPositionToGridPoint(viewerPosition, TerrainConstants::TERRAIN_PATCH_SIZE) };

	//Calculate the valid grid points.
	const StaticArray<GridPoint2, 9> validGridPoints
	{
		GridPoint2(currentGridPoint._X, currentGridPoint._Y),

		GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y - 1),
		GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y),
		GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y + 1),

		GridPoint2(currentGridPoint._X, currentGridPoint._Y - 1),
		GridPoint2(currentGridPoint._X, currentGridPoint._Y + 1),

		GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y - 1),
		GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y),
		GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y + 1)
	};

	//If a root grid point does exist that isn't valid, remove it.
	for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
	{
		if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
		{
			continue;
		}

		bool exists{ false };

		for (const GridPoint2 validGridPoint : validGridPoints)
		{
			if (validGridPoint == _QuadTree._RootGridPoints[i])
			{
				exists = true;

				break;
			}
		}

		if (!exists)
		{
			_Update._Type = TerrainUpdate::Type::RemoveRootNode;
			_Update._RemoveRootNodeUpdate._Index = i;

			return;
		}
	}

	//If a valid grid point does not exist, construct a new update.
	for (const GridPoint2 validGridPoint : validGridPoints)
	{
		bool exists{ false };

		for (const GridPoint2 rootGridPoint : _QuadTree._RootGridPoints)
		{
			if (rootGridPoint == validGridPoint)
			{
				exists = true;

				break;
			}
		}

		if (!exists)
		{
			_Update._Type = TerrainUpdate::Type::AddRootNode;
			_Update._AddRootNodeUpdate._GridPoint = validGridPoint;

			GeneratePatch(validGridPoint, TerrainBorder::None, 1.0f, 1, &_Update._AddRootNodeUpdate._PatchInformation, &_Update._AddRootNodeUpdate._PatchRenderInformation);

			return;
		}
	}
}

/*
*	Generates a patch.
*/
void TerrainSystem::GeneratePatch(const GridPoint2 &gridPoint, const TerrainBorder borders, const float patchSizeMultiplier, const uint8 normalResolutionMultiplier, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT
{
	//Calculate the world position of the grid point.
	const Vector3 gridPointWorldPosition{ GridPoint2::GridPointToWorldPosition(gridPoint, TerrainConstants::TERRAIN_PATCH_SIZE) };

	//Get the material and the displacement information.
	TerrainMaterial material;

	_Properties._PatchPropertiesGenerationFunction(_Properties, gridPointWorldPosition, &material);

	//Fill in the details about the patch information.
	patchInformation->_Identifier = TerrainUtilities::GeneratePatchIdentifier();
	patchInformation->_Valid = true;
	patchInformation->_GridPoint = gridPoint;

	//Fill in the details about the patch render information.
	patchRenderInformation->_Visibility = VisibilityFlag::None;
	patchRenderInformation->_WorldPosition = Vector2(gridPointWorldPosition._X, gridPointWorldPosition._Z);
	patchRenderInformation->_PatchSize = TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier;

	RenderingSystem::Instance->CreateRenderDataTable(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Terrain), &patchRenderInformation->_RenderDataTable);

	float minimumHeight;
	float maximumHeight;

	TerrainUtilities::GenerateHeightTexture(	_Properties,
												patchSizeMultiplier,
												gridPointWorldPosition,
												&minimumHeight,
												&maximumHeight,
												&patchInformation->_HeightTexture,
												&patchRenderInformation->_RenderDataTable);

	TerrainUtilities::GenerateNormalTexture(	_Properties,
												patchSizeMultiplier,
												normalResolutionMultiplier,
												gridPointWorldPosition,
												&patchInformation->_NormalTexture,
												&patchRenderInformation->_RenderDataTable);

	TerrainUtilities::GenerateLayerWeightsTexture(	_Properties,
													patchSizeMultiplier,
													normalResolutionMultiplier,
													gridPointWorldPosition,
													&patchInformation->_LayerWeightsTexture,
													&patchRenderInformation->_RenderDataTable);

	patchInformation->_AxisAlignedBoundingBox._Minimum = Vector3(gridPointWorldPosition._X - (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f), minimumHeight, gridPointWorldPosition._Z - (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f));
	patchInformation->_AxisAlignedBoundingBox._Maximum = Vector3(gridPointWorldPosition._X + (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f), maximumHeight, gridPointWorldPosition._Z + (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f));
}