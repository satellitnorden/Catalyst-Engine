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
#include <Terrain/TerrainGeneralUtilities.h>
#include <Terrain/TerrainQuadTreeUtilities.h>

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

	//Generate the terrain plane.
	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainGeneralUtilities::GenerateTerrainPlane(	&vertices,
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
	TerrainGeneralUtilities::GenerateNormal(_Properties, position, normal);

	//Return that the retrieval succeeded.
	return true;
}

/*
	*	Returns the patch information index for the identifier.
	*/
uint64 TerrainSystem::GetPatchInformationIndex(const uint64 identifier) const NOEXCEPT
{
	for (uint64 i{ 0 }, size{ _PatchInformations.Size() }; i < size; ++i)
	{
		if (_PatchInformations[i]._Identifier == identifier)
		{
			return i;
		}
	}

	return UINT64_MAX;
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
					const Vector3 gridPointWorldPosition{ GridPoint2::GridPointToWorldPosition(_Update._AddRootNodeUpdate._GridPoint, TerrainConstants::TERRAIN_PATCH_SIZE) };

					_QuadTree._RootGridPoints[i] = _Update._AddRootNodeUpdate._GridPoint;
					_QuadTree._RootNodes[i]._Depth = 0;
					_QuadTree._RootNodes[i]._Subdivided = false;
					_QuadTree._RootNodes[i]._Identifier = _Update._AddRootNodeUpdate._PatchInformation._Identifier;
					_QuadTree._RootNodes[i]._ChildNodes = nullptr;
					_QuadTree._RootNodes[i]._Minimum = Vector2(gridPointWorldPosition._X - (TerrainConstants::TERRAIN_PATCH_SIZE * 0.5f), gridPointWorldPosition._Z - (TerrainConstants::TERRAIN_PATCH_SIZE * 0.5f));
					_QuadTree._RootNodes[i]._Maximum = Vector2(gridPointWorldPosition._X + (TerrainConstants::TERRAIN_PATCH_SIZE * 0.5f), gridPointWorldPosition._Z + (TerrainConstants::TERRAIN_PATCH_SIZE * 0.5f));

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
					DestroyPatch(i);

					break;
				}
			}

			break;
		}

		case TerrainUpdate::Type::RestoreNode:
		{	
			//Destroy the existing child nodes.
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				const uint64 patchInformationIndex{ GetPatchInformationIndex(_Update._RestoreNodeUpdate._Node->_ChildNodes[i]._Identifier) };

				DestroyPatch(patchInformationIndex);
			}

			break;
		}

		case TerrainUpdate::Type::SubdivideNode:
		{
			//Find the patch information for this node and delete it.
			for (uint64 i{ 0 }, size{ _PatchInformations.Size() }; i < size; ++i)
			{
				if (_PatchInformations[i]._Identifier == _Update._SubdivideNodeUpdate._Node->_Identifier)
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

			//Actually subdivide node.
			_Update._SubdivideNodeUpdate._Node->_Subdivided = true;
			_Update._SubdivideNodeUpdate._Node->_ChildNodes = static_cast<TerrainQuadTreeNode *const RESTRICT>(MemoryUtilities::AllocateMemory(sizeof(TerrainQuadTreeNode) * 4));

			for (uint8 i{ 0 }; i < 4; ++i)
			{
				_Update._SubdivideNodeUpdate._Node->_ChildNodes[i]._Depth = _Update._SubdivideNodeUpdate._Node->_Depth + 1;
				_Update._SubdivideNodeUpdate._Node->_ChildNodes[i]._Subdivided = false;
				_Update._SubdivideNodeUpdate._Node->_ChildNodes[i]._Identifier = _Update._SubdivideNodeUpdate._PatchInformations[i]._Identifier;
				_Update._SubdivideNodeUpdate._Node->_ChildNodes[i]._ChildNodes = nullptr;
				_Update._SubdivideNodeUpdate._Node->_ChildNodes[i]._Minimum._X = _Update._SubdivideNodeUpdate._PatchInformations[i]._AxisAlignedBoundingBox._Minimum._X;
				_Update._SubdivideNodeUpdate._Node->_ChildNodes[i]._Minimum._Y = _Update._SubdivideNodeUpdate._PatchInformations[i]._AxisAlignedBoundingBox._Minimum._Z;
				_Update._SubdivideNodeUpdate._Node->_ChildNodes[i]._Maximum._X = _Update._SubdivideNodeUpdate._PatchInformations[i]._AxisAlignedBoundingBox._Maximum._X;
				_Update._SubdivideNodeUpdate._Node->_ChildNodes[i]._Maximum._Y = _Update._SubdivideNodeUpdate._PatchInformations[i]._AxisAlignedBoundingBox._Maximum._Z;

				_PatchInformations.EmplaceSlow(_Update._SubdivideNodeUpdate._PatchInformations[i]);
				_PatchRenderInformations.EmplaceSlow(_Update._SubdivideNodeUpdate._PatchRenderInformations[i]);
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

			GeneratePatch(	GridPoint2::GridPointToWorldPosition(validGridPoint, TerrainConstants::TERRAIN_PATCH_SIZE),
							1.0f,
							TerrainQuadTreeUtilities::ResolutionMultiplier(1.0f),
							&_Update._AddRootNodeUpdate._PatchInformation,
							&_Update._AddRootNodeUpdate._PatchRenderInformation);

			return;
		}
	}

	//Check if a node should be restored.
	for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
	{
		if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
		{
			continue;
		}

		if (CheckRestoration(viewerPosition, &_QuadTree._RootNodes[i]))
		{
			return;
		}
	}

	//Check if a node should be subdivided.
	for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
	{
		if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
		{
			continue;
		}

		if (CheckSubdivision(viewerPosition, &_QuadTree._RootNodes[i]))
		{
			return;
		}
	}
}

/*
*	Checks restoration of a node. Returns whether or not the node was restored.
*/
bool TerrainSystem::CheckRestoration(const Vector3 &viewerPosition, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	/*
	//If this node is already subdivided, check all of it's child nodes.
	if (node->_Subdivided)
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			if (CheckRestoration(viewerPosition, &node->_ChildNodes[i]))
			{
				return true;
			}
		}
	}

	//Else, check if this node should be restored.
	else
	{
		if (!ShouldBeSubdivided(viewerPosition, node))
		{
			RestoreNode(node);

			return true;
		}
	}
	*/

	return false;
}

/*
*	Checks subdivisions of a node. Returns whether or not the node was subdivided.
*/
bool TerrainSystem::CheckSubdivision(const Vector3 &viewerPosition, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	//If this node is already subdivided, check all of it's child nodes.
	if (node->_Subdivided)
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			if (CheckSubdivision(viewerPosition, &node->_ChildNodes[i]))
			{
				return true;
			}
		}
	}

	//Else, check if this node should be subdivided.
	else
	{
		if (TerrainQuadTreeUtilities::ShouldBeSubdivided(*node, viewerPosition))
		{
			//If there is a requirement for this subdivision to happen, subdivide that node instead.
			if (TerrainQuadTreeNode *const RESTRICT requiredNode{ TerrainQuadTreeUtilities::SubdivisionRequirement(node, &_QuadTree) })
			{
				SubdivideNode(requiredNode);
			}

			else
			{
				SubdivideNode(node);
			}

			return true;
		}
	}

	return false;
}

/*
*	Restores a node.
*/
void TerrainSystem::RestoreNode(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	_Update._Type = TerrainUpdate::Type::RestoreNode;
	_Update._RestoreNodeUpdate._Node = node;

	const float patchSizeMultiplier{ TerrainQuadTreeUtilities::PatchSizeMultiplier(*node) };

	const Vector3 worldPosition{	node->_Minimum._X + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier,
									0.0f,
									node->_Minimum._Y + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier };

	GeneratePatch(	worldPosition,
					patchSizeMultiplier,
					TerrainQuadTreeUtilities::ResolutionMultiplier(patchSizeMultiplier),
					&_Update._RestoreNodeUpdate._PatchInformation,
					&_Update._RestoreNodeUpdate._PatchRenderInformation);
}

/*
*	Subdivides a node.
*/
void TerrainSystem::SubdivideNode(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	_Update._Type = TerrainUpdate::Type::SubdivideNode;
	_Update._SubdivideNodeUpdate._Node = node;

	const float patchSizeMultiplier{ TerrainQuadTreeUtilities::PatchSizeMultiplier(*node) * 0.5f };

	const StaticArray<Vector3, 4> positions
	{
		Vector3(	node->_Minimum._X + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f,
					0.0f,
					node->_Minimum._Y + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f),

		Vector3(	node->_Minimum._X + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f,
					0.0f,
					node->_Minimum._Y + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 1.5f),

		Vector3(	node->_Minimum._X + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 1.5f,
					0.0f,
					node->_Minimum._Y + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 1.5f),

		Vector3(	node->_Minimum._X + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 1.5f,
					0.0f,
					node->_Minimum._Y + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f)
	};

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		GeneratePatch(	positions[i],
						patchSizeMultiplier,
						TerrainQuadTreeUtilities::ResolutionMultiplier(patchSizeMultiplier),
						&_Update._SubdivideNodeUpdate._PatchInformations[i],
						&_Update._SubdivideNodeUpdate._PatchRenderInformations[i]);
	}
}

/*
*	Generates a patch.
*/
void TerrainSystem::GeneratePatch(const Vector3 &worldPosition, const float patchSizeMultiplier, const uint8 resolutionMultiplier, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT
{
	ASSERT(resolutionMultiplier > 0, "No");

	//Get the material and the displacement information.
	TerrainMaterial material;

	_Properties._PatchPropertiesGenerationFunction(_Properties, worldPosition, &material);

	//Fill in the details about the patch information.
	patchInformation->_Identifier = TerrainGeneralUtilities::GeneratePatchIdentifier();
	patchInformation->_Valid = true;

	//Fill in the details about the patch render information.
	patchRenderInformation->_Visibility = VisibilityFlag::None;
	patchRenderInformation->_WorldPosition = Vector2(worldPosition._X, worldPosition._Z);
	patchRenderInformation->_PatchSize = TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier;

	RenderingSystem::Instance->CreateRenderDataTable(RenderingSystem::Instance->GetCommonRenderDataTableLayout(CommonRenderDataTableLayout::Terrain), &patchRenderInformation->_RenderDataTable);

	float minimumHeight;
	float maximumHeight;

	TerrainGeneralUtilities::GenerateHeightTexture(	_Properties,
													patchSizeMultiplier,
													worldPosition,
													&minimumHeight,
													&maximumHeight,
													&patchInformation->_HeightTexture,
													&patchRenderInformation->_RenderDataTable);

	TerrainGeneralUtilities::GenerateNormalTexture(	_Properties,
													patchSizeMultiplier,
													resolutionMultiplier,
													worldPosition,
													&patchInformation->_NormalTexture,
													&patchRenderInformation->_RenderDataTable);

	TerrainGeneralUtilities::GenerateLayerWeightsTexture(	_Properties,
															patchSizeMultiplier,
															resolutionMultiplier,
															worldPosition,
															&patchInformation->_LayerWeightsTexture,
															&patchRenderInformation->_RenderDataTable);

	patchInformation->_AxisAlignedBoundingBox._Minimum = Vector3(worldPosition._X - (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f), minimumHeight, worldPosition._Z - (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f));
	patchInformation->_AxisAlignedBoundingBox._Maximum = Vector3(worldPosition._X + (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f), maximumHeight, worldPosition._Z + (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f));
}

/*
*	Destroys a patch.
*/
void TerrainSystem::DestroyPatch(const uint64 index) NOEXCEPT
{
	RenderingSystem::Instance->DestroyTexture2D(_PatchInformations[index]._HeightTexture);
	RenderingSystem::Instance->DestroyTexture2D(_PatchInformations[index]._NormalTexture);
	RenderingSystem::Instance->DestroyTexture2D(_PatchInformations[index]._LayerWeightsTexture);
	RenderingSystem::Instance->DestroyRenderDataTable(_PatchRenderInformations[index]._RenderDataTable);

	_PatchInformations.EraseAt(index);
	_PatchRenderInformations.EraseAt(index);
}