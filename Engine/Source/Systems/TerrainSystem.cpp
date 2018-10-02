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
	_Properties._TerrainTextureTilingFactor = configuration._TerrainTextureTilingFactor;
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
		_PatchInformations[i]._Valid = false;
		_PatchRenderInformations[i]._Draw = false;
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
		//Process the terrain update.
		ProcessTerrainUpdate();

		//cache the relevant data and fire off another asynchronous update.
		const CameraEntity *const RESTRICT camera{ RenderingSystem::Instance->GetActiveCamera() };
		_CurrentCameraPosition = camera->GetPosition();

		TaskSystem::Instance->ExecuteTask(&_UpdateTask);
	}
}

/*
*	Returns the terrain height at the given position.
*/
bool TerrainSystem::GetTerrainHeightAtPosition(const Vector3 &position, float *const RESTRICT height) const NOEXCEPT
{
	//Find the nearest terrain patch to the position.
	const TerrainPatchInformation *RESTRICT nearestPatch{ nullptr };
	float nearestPatchDistanceSquared{ FLOAT_MAXIMUM };

	//Lock the patches.
	_PatchLock.Lock();

	for (const TerrainPatchInformation &patchInformation : _PatchInformations)
	{
		if (patchInformation._Valid)
		{
			const Vector3 patchWorldPosition{ GridPoint::GridPointToWorldPosition(patchInformation._GridPoint, _Properties._PatchSize) };
			const float distanceToPatchSquared{ Vector3::LengthSquaredXZ(patchWorldPosition - position) };

			if (distanceToPatchSquared < nearestPatchDistanceSquared)
			{
				nearestPatch = &patchInformation;
				nearestPatchDistanceSquared = distanceToPatchSquared;
			}
		}
	}

	//Unlock the patches.
	_PatchLock.Unlock();

	//If no valid patch could be found, return that the retrieval failed.
	if (!nearestPatch)
	{
		return false;
	}

	//If the position is not within the nearest patch's axis-aligned bounding box, return that the retrival failed.
	if (position._X < nearestPatch->_AxisAlignedBoundingBox._Minimum._X
		|| position._X > nearestPatch->_AxisAlignedBoundingBox._Maximum._X
		|| position._Z < nearestPatch->_AxisAlignedBoundingBox._Minimum._Z
		|| position._Z > nearestPatch->_AxisAlignedBoundingBox._Maximum._Z)
	{
		return false;
	}

	//Calculate the patch's world position.
	const Vector3 patchWorldPosition{ GridPoint::GridPointToWorldPosition(nearestPatch->_GridPoint, _Properties._PatchSize) };

	//Transform the position's X and Z components into X and Y coordinates.
	const float coordinateX{ (position._X - (patchWorldPosition._X - _Properties._PatchSize * 0.5f)) / _Properties._PatchSize };
	const float coordinateY{ (position._Z - (patchWorldPosition._Z - _Properties._PatchSize * 0.5f)) / _Properties._PatchSize };

	//Sample the patch's normal/height map.
	const Vector4 &normalHeight{ nearestPatch->_NormalHeightMap.At(coordinateX, coordinateY) };

	//Set the height.
	*height = normalHeight._W;

	//Return that the retrieval succeeded.
	return true;
}

/*
*	Returns the terrain normal at the given position.
*/
bool TerrainSystem::GetTerrainNormalAtPosition(const Vector3 &position, Vector3 *const RESTRICT normal) const NOEXCEPT
{
	//Find the nearest terrain patch to the position.
	const TerrainPatchInformation *RESTRICT nearestPatch{ nullptr };
	float nearestPatchDistanceSquared{ FLOAT_MAXIMUM };

	//Lock the patches.
	_PatchLock.Lock();

	for (const TerrainPatchInformation &patchInformation : _PatchInformations)
	{
		if (patchInformation._Valid)
		{
			const Vector3 patchWorldPosition{ GridPoint::GridPointToWorldPosition(patchInformation._GridPoint, _Properties._PatchSize) };
			const float distanceToPatchSquared{ Vector3::LengthSquaredXZ(patchWorldPosition - position) };

			if (distanceToPatchSquared < nearestPatchDistanceSquared)
			{
				nearestPatch = &patchInformation;
				nearestPatchDistanceSquared = distanceToPatchSquared;
			}
		}
	}

	//Unlock the patches.
	_PatchLock.Unlock();

	//If no valid patch could be found, return that the retrieval failed.
	if (!nearestPatch)
	{
		return false;
	}

	//If the position is not within the nearest patch's axis-aligned bounding box, return that the retrival failed.
	if (position._X < nearestPatch->_AxisAlignedBoundingBox._Minimum._X
		|| position._X > nearestPatch->_AxisAlignedBoundingBox._Maximum._X
		|| position._Z < nearestPatch->_AxisAlignedBoundingBox._Minimum._Z
		|| position._Z > nearestPatch->_AxisAlignedBoundingBox._Maximum._Z)
	{
		return false;
	}

	//Calculate the patch's world position.
	const Vector3 patchWorldPosition{ GridPoint::GridPointToWorldPosition(nearestPatch->_GridPoint, _Properties._PatchSize) };

	//Transform the position's X and Z components into X and Y coordinates.
	const float coordinateX{ (position._X - (patchWorldPosition._X - _Properties._PatchSize * 0.5f)) / _Properties._PatchSize };
	const float coordinateY{ (position._Z - (patchWorldPosition._Z - _Properties._PatchSize * 0.5f)) / _Properties._PatchSize };

	//Sample the patch's normal/height map.
	const Vector4 &normalHeight{ nearestPatch->_NormalHeightMap.At(coordinateX, coordinateY) };

	//Set the normal.
	*normal = Vector3(normalHeight._X, normalHeight._Y, normalHeight._Z);

	//Return that the retrieval succeeded.
	return true;
}

/*
*	Updates the terrain system asynchronously.
*/
void TerrainSystem::UpdateSystemAsynchronous() NOEXCEPT
{
	//Reset the update.
	_Update._PatchesToInvalidate.ClearFast();
	_Update._AddPatch = false;

	//Calculate the current grid point based on the current camera position.
	const GridPoint currentGridPoint{ GridPoint::WorldPositionToGridPoint(_CurrentCameraPosition, _Properties._PatchSize) };

	//Create an array with the valid grid positions.
	StaticArray<GridPoint, 25> validGridPoints
	{
		GridPoint(currentGridPoint._X - 2, currentGridPoint._Y - 2),
		GridPoint(currentGridPoint._X - 1, currentGridPoint._Y - 2),
		GridPoint(currentGridPoint._X, currentGridPoint._Y - 2),
		GridPoint(currentGridPoint._X + 1, currentGridPoint._Y - 2),
		GridPoint(currentGridPoint._X + 2, currentGridPoint._Y - 2),

		GridPoint(currentGridPoint._X - 2, currentGridPoint._Y - 1),
		GridPoint(currentGridPoint._X - 1, currentGridPoint._Y - 1),
		GridPoint(currentGridPoint._X, currentGridPoint._Y - 1),
		GridPoint(currentGridPoint._X + 1, currentGridPoint._Y - 1),
		GridPoint(currentGridPoint._X + 2, currentGridPoint._Y - 1),

		GridPoint(currentGridPoint._X - 2, currentGridPoint._Y),
		GridPoint(currentGridPoint._X - 1, currentGridPoint._Y),
		GridPoint(currentGridPoint._X, currentGridPoint._Y),
		GridPoint(currentGridPoint._X + 1, currentGridPoint._Y),
		GridPoint(currentGridPoint._X + 2, currentGridPoint._Y),

		GridPoint(currentGridPoint._X - 2, currentGridPoint._Y + 1),
		GridPoint(currentGridPoint._X - 1, currentGridPoint._Y + 1),
		GridPoint(currentGridPoint._X, currentGridPoint._Y + 1),
		GridPoint(currentGridPoint._X + 1, currentGridPoint._Y + 1),
		GridPoint(currentGridPoint._X + 2, currentGridPoint._Y + 1),

		GridPoint(currentGridPoint._X - 2, currentGridPoint._Y + 2),
		GridPoint(currentGridPoint._X - 1, currentGridPoint._Y + 2),
		GridPoint(currentGridPoint._X, currentGridPoint._Y + 2),
		GridPoint(currentGridPoint._X + 1, currentGridPoint._Y + 2),
		GridPoint(currentGridPoint._X + 2, currentGridPoint._Y + 2),
	};

	//Construct the sorting data.
	class SortingData final
	{

	public:

		//The current camera position.
		Vector3 _CameraPosition;

		//The patch size.
		float _PatchSize;

	};

	SortingData sortingData;

	sortingData._CameraPosition = _CurrentCameraPosition;
	sortingData._PatchSize = _Properties._PatchSize;

	//Sort the array so that the closest grid point is first.
	SortingAlgorithms::InsertionSort<GridPoint>(validGridPoints.begin(), validGridPoints.end(), &sortingData, [](const void *const RESTRICT userData, const GridPoint *const RESTRICT first, const GridPoint *const RESTRICT second)
	{
		const SortingData *const RESTRICT sortingData{ static_cast<const SortingData *const RESTRICT>(userData) };

		const Vector3 firstGridPosition{ GridPoint::GridPointToWorldPosition(*first, sortingData->_PatchSize) };
		const Vector3 secondGridPosition{ GridPoint::GridPointToWorldPosition(*second, sortingData->_PatchSize) };

		return Vector3::LengthSquaredXZ(sortingData->_CameraPosition - firstGridPosition) < Vector3::LengthSquaredXZ(sortingData->_CameraPosition - secondGridPosition);
	});

	//Determine the patch indices to invalidate.
	for (uint64 i = 0, size = _PatchInformations.Size(); i < size; ++i)
	{
		//If this patch is already invalid, no need to invalidate it.
		if (!_PatchInformations[i]._Valid)
		{
			continue;
		}

		bool valid{ false };

		for (const GridPoint &gridPoint : validGridPoints)
		{
			if (_PatchInformations[i]._GridPoint == gridPoint)
			{
				valid = true;

				break;
			}
		}

		if (!valid)
		{
			_Update._PatchesToInvalidate.EmplaceSlow(static_cast<uint8>(i));
		}
	}

	//Determine if a new patch should be generated.
	bool generatedNewPatch{ false };

	for (const GridPoint &gridPoint : validGridPoints)
	{
		bool exists{ false };

		for (const TerrainPatchInformation &information : _PatchInformations)
		{
			if (information._Valid && information._GridPoint == gridPoint)
			{
				exists = true;

				break;
			}
		}

		if (!exists)
		{
			//Generate the new patch.
			GeneratePatch(gridPoint, &_Update._NewPatchInformation, &_Update._NewPatchRenderInformation);

			_Update._AddPatch = true;

			break;
		}
	}
}

/*
*	Generates a new patch at the specified grid point.
*/
void TerrainSystem::GeneratePatch(const GridPoint &gridPoint, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT
{
	//Calculate the world position of the grid point.
	const Vector3 gridPointWorldPosition{ GridPoint::GridPointToWorldPosition(gridPoint, _Properties._PatchSize) };

	//Generate the height.
	patchInformation->_NormalHeightMap.Initialize(_Properties._PatchResolution, _Properties._PatchResolution);

	for (uint32 i = 0; i < _Properties._PatchResolution; ++i)
	{
		for (uint32 j = 0; j < _Properties._PatchResolution; ++j)
		{
			//Calculate the coordinates for this point.
			const float xCoordinate{ static_cast<float>(i) / static_cast<float>(_Properties._PatchResolution) };
			const float yCoordinate{ static_cast<float>(j) / static_cast<float>(_Properties._PatchResolution) };

			//Calculate the world position for this point.
			const Vector3 pointWorldPosition{	gridPointWorldPosition._X - (_Properties._PatchSize * 0.5f) + (_Properties._PatchSize * xCoordinate),
												0.0f, 
												gridPointWorldPosition._Z - (_Properties._PatchSize * 0.5f) + (_Properties._PatchSize * yCoordinate) };

			//Generate the height.
			_Properties._HeightGenerationFunction(_Properties, pointWorldPosition, &patchInformation->_NormalHeightMap.At(i, j)._W);
		}
	}

	//Generate normals.
	TerrainUtilities::GeneratePatchNormals(_Properties._PatchSize, &patchInformation->_NormalHeightMap);

	//Generate the layer weights.
	patchInformation->_LayerWeightsMap.Initialize(_Properties._PatchResolution, _Properties._PatchResolution);

	for (uint32 i = 0; i < _Properties._PatchResolution; ++i)
	{
		for (uint32 j = 0; j < _Properties._PatchResolution; ++j)
		{
			//Get the normal at this point.
			const Vector4 &pointNormalheight{ patchInformation->_NormalHeightMap.At(i, j) };
			const Vector3 pointNormal{ pointNormalheight._X, pointNormalheight._Y, pointNormalheight._Z };

			//Calculate the coordinates for this point.
			const float xCoordinate{ static_cast<float>(i) / static_cast<float>(_Properties._PatchResolution) };
			const float yCoordinate{ static_cast<float>(j) / static_cast<float>(_Properties._PatchResolution) };

			//Calculate the world position for this point.
			const Vector3 pointWorldPosition{	gridPointWorldPosition._X - (_Properties._PatchSize * 0.5f) + (_Properties._PatchSize * xCoordinate),
												pointNormalheight._W,
												gridPointWorldPosition._Z - (_Properties._PatchSize * 0.5f) + (_Properties._PatchSize * yCoordinate) };

			//Generate the layer weights.
			_Properties._LayerWeightsGenerationFunction(_Properties, pointWorldPosition, pointNormal, &patchInformation->_LayerWeightsMap.At(i, j));
		}
	}

	//Generate the terrain plane.
	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainUtilities::GenerateTerrainPlane(_Properties,
											_Properties._PatchResolution,
											gridPointWorldPosition,
											patchInformation->_NormalHeightMap,
											patchInformation->_LayerWeightsMap,
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

	TerrainUtilities::FindMinimumMaximumHeight(patchInformation->_NormalHeightMap, &minimumHeight, &maximumHeight);

	patchInformation->_AxisAlignedBoundingBox._Minimum = Vector3(gridPointWorldPosition._X - (_Properties._PatchSize * 0.5f), minimumHeight, gridPointWorldPosition._Z - (_Properties._PatchSize * 0.5f));
	patchInformation->_AxisAlignedBoundingBox._Maximum = Vector3(gridPointWorldPosition._X + (_Properties._PatchSize * 0.5f), maximumHeight, gridPointWorldPosition._Z + (_Properties._PatchSize * 0.5f));

	//Fill in the details about the patch render information.
	patchRenderInformation->_Draw = true;
	patchRenderInformation->_Buffer = RenderingSystem::Instance->CreateConstantBuffer(bufferData.Data(), bufferDataSizes.Data(), 2);
	patchRenderInformation->_IndexOffset = bufferDataSizes[0];
	patchRenderInformation->_IndexCount = static_cast<uint32>(indices.Size());
	patchRenderInformation->_RenderDataTable = material._RenderDataTable;
	patchRenderInformation->_DisplacementInformation = displacementInformation;
}

/*
*	Processes the terrain update.
*/
void TerrainSystem::ProcessTerrainUpdate() NOEXCEPT
{
	//Invalidate patches.
	for (const uint64 index : _Update._PatchesToInvalidate)
	{
		InvalidatePatch(index);
	}

	//If there is a new patch to add, add it.
	if (_Update._AddPatch)
	{
		//Lock the patches.
		_PatchLock.Lock();

		for (uint64 i = 0, size = _PatchInformations.Size(); i < size; ++i)
		{
			if (!_PatchInformations[i]._Valid)
			{
				_PatchInformations[i] = _Update._NewPatchInformation;
				_PatchRenderInformations[i] = _Update._NewPatchRenderInformation;

				break;
			}
		}

		//Unlock the patches.
		_PatchLock.Unlock();
	}
}

/*
*	Invalidates the patch at the specified index.
*/
void TerrainSystem::InvalidatePatch(const uint64 index) NOEXCEPT
{
	_PatchInformations[index]._Valid = false;
	_PatchRenderInformations[index]._Draw = false;

	RenderingSystem::Instance->DestroyConstantBuffer(_PatchRenderInformations[index]._Buffer);
}