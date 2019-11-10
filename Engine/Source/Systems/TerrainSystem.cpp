//Header file.
#include <Systems/TerrainSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/CatalystProjectConfiguration.h>
#include <Core/General/Perceiver.h>

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
void TerrainSystem::Initialize() NOEXCEPT
{
	//Set the function for the update task.
	_UpdateTask._Function = [](void *const RESTRICT)
	{
		TerrainSystem::Instance->UpdateAsynchronous();
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

	RenderingSystem::Instance->CreateBuffer(bufferDataSizes[0] + bufferDataSizes[1], BufferUsage::IndexBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &_Properties._Buffer);
	RenderingSystem::Instance->UploadDataToBuffer(bufferData.Data(), bufferDataSizes.Data(), 2, &_Properties._Buffer);

	_Properties._IndexOffset = bufferDataSizes[0];
	_Properties._IndexCount = static_cast<uint32>(indices.Size());

	//Initialize the quad tree.
	_QuadTree.Initialize();

	//Allocate memory.
	_PatchInformations.Reserve(TerrainConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES);
	_PatchRenderInformations.Reserve(TerrainConstants::MAXIMUM_NUMBER_OF_TERRAIN_PATCHES);
}

/*
*	Updates the terrain system during the sequential update phase.
*/
void TerrainSystem::SequentialUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//If the properties aren't valid, just return early.
	if (!_Properties._HeightFunction || !_Properties._MaterialFunction)
	{
		return;
	}

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
bool TerrainSystem::GetTerrainHeightAtPosition(const Vector3<float> &position, float *const RESTRICT height, const void* const RESTRICT context) const NOEXCEPT
{
	//If there's no height function, just set the height to zero, and return that the retrieval failed.
	if (!_Properties._HeightFunction)
	{
		*height = 0.0f;

		return false;
	}

	//Generate the height.
	_Properties._HeightFunction(_Properties, position, height, context);

	//Return that the retrieval succeeded.
	return true;
}

/*
*	Returns the terrain normal at the given position.
*/
bool TerrainSystem::GetTerrainNormalAtPosition(const Vector3<float> &position, Vector3<float> *const RESTRICT normal, float *const RESTRICT height, const void* const RESTRICT context) const NOEXCEPT
{
	//If there's no height function, just set the normal to up and the height to zero, and return that the retrieval failed.
	if (!_Properties._HeightFunction)
	{
		*normal = VectorConstants::UP;
		*height = 0.0f;

		return false;
	}

	//Generate a normal at the position.
	TerrainGeneralUtilities::GenerateNormal(_Properties, position, normal, height, context);

	//Return that the retrieval succeeded.
	return true;
}

/*
*	Returns the terrain material at the given position.
*	Can optionally retrieve the height and the normal at the same time.
*/
bool TerrainSystem::GetTerrainMaterialAtPosition(const Vector3<float> &position, uint8 *const RESTRICT material, float *const RESTRICT height, Vector3<float> *const RESTRICT normal, const void* const RESTRICT context) const NOEXCEPT
{
	//If there's no height function, just set the material to zero, the normal to up and the height to zero, and return that the retrieval failed.
	if (!_Properties._MaterialFunction)
	{
		*material = 0;
		*normal = VectorConstants::UP;
		*height = 0.0f;

		return false;
	}

	//Retrieve the height and the normal.
	float terrainHeight;
	Vector3<float> terrainNormal;

	GetTerrainNormalAtPosition(position, &terrainNormal, &terrainHeight, context);

	//Generate the material.
	_Properties._MaterialFunction(_Properties, position, terrainHeight, terrainNormal, material, context);

	//Set the height.
	if (height)
	{
		*height = terrainHeight;
	}

	//Set the normal.
	if (normal)
	{
		*normal = terrainNormal;
	}

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
					const Vector3<float> gridPointWorldPosition{ GridPoint2::GridPointToWorldPosition(_Update._AddRootNodeUpdate._GridPoint, TerrainConstants::TERRAIN_PATCH_SIZE) };

					_QuadTree._RootGridPoints[i] = _Update._AddRootNodeUpdate._GridPoint;
					_QuadTree._RootNodes[i]._Depth = 0;
					_QuadTree._RootNodes[i]._Subdivided = false;
					_QuadTree._RootNodes[i]._Identifier = _Update._AddRootNodeUpdate._PatchInformation._Identifier;
					_QuadTree._RootNodes[i]._ChildNodes = nullptr;
					_QuadTree._RootNodes[i]._Minimum = Vector2<float>(gridPointWorldPosition._X - (TerrainConstants::TERRAIN_PATCH_SIZE * 0.5f), gridPointWorldPosition._Z - (TerrainConstants::TERRAIN_PATCH_SIZE * 0.5f));
					_QuadTree._RootNodes[i]._Maximum = Vector2<float>(gridPointWorldPosition._X + (TerrainConstants::TERRAIN_PATCH_SIZE * 0.5f), gridPointWorldPosition._Z + (TerrainConstants::TERRAIN_PATCH_SIZE * 0.5f));

					break;
				}
			}

			_PatchInformations.EmplaceFast(_Update._AddRootNodeUpdate._PatchInformation);
			_PatchRenderInformations.EmplaceFast(_Update._AddRootNodeUpdate._PatchRenderInformation);

			break;
		}

		case TerrainUpdate::Type::RemoveRootNode:
		{
			_QuadTree._RootGridPoints[_Update._RemoveRootNodeUpdate._Index] = GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM);
			
			RemoveNode(&_QuadTree._RootNodes[_Update._RemoveRootNodeUpdate._Index]);

			break;
		}

		case TerrainUpdate::Type::CombineNode:
		{	
			//Destroy the existing child nodes.
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				const uint64 patchInformationIndex{ GetPatchInformationIndex(_Update._CombineNodeUpdate._PatchInformationIdentifiers[i]) };
				DestroyPatch(patchInformationIndex);
			}

			//Add the new patch information.
			_PatchInformations.EmplaceFast(_Update._CombineNodeUpdate._PatchInformation);
			_PatchRenderInformations.EmplaceFast(_Update._CombineNodeUpdate._PatchRenderInformation);

			break;
		}

		case TerrainUpdate::Type::SubdivideNode:
		{
			//Find the patch information for this node and destroy it.
			const uint64 patchInformationIndex{ GetPatchInformationIndex(_Update._SubdivideNodeUpdate._Node->_Identifier) };
			DestroyPatch(patchInformationIndex);

			//Add the new patch informations.
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				_PatchInformations.EmplaceFast(_Update._SubdivideNodeUpdate._PatchInformations[i]);
				_PatchRenderInformations.EmplaceFast(_Update._SubdivideNodeUpdate._PatchRenderInformations[i]);
			}

			break;
		}
	}

	//Process the borders updats.
	if (_Update._Type != TerrainUpdate::Type::Invalid)
	{
		for (Pair<const uint64, const int32> &pair : _Update._BordersUpdates)
		{
			const uint64 patchInformationIndex{ GetPatchInformationIndex(pair._First) };
			_PatchRenderInformations[patchInformationIndex]._Borders = pair._Second;
		}

		_Update._BordersUpdates.ClearFast();
	}
}

/*
*	Updates the terrain system asynchronously.
*/
void TerrainSystem::UpdateAsynchronous() NOEXCEPT
{
	//Reset the update.
	_Update._Type = TerrainUpdate::Type::Invalid;

	//Get the current perceiver position.
	const Vector3<float> perceiverPosition{ Perceiver::Instance->GetPosition() };

	//Calculate the perceiver grid point.
	const GridPoint2 currentGridPoint{ GridPoint2::WorldPositionToGridPoint(perceiverPosition, TerrainConstants::TERRAIN_PATCH_SIZE) };

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

			//Calculate new borders.
			CalculateNewborders();

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
							0,
							&_Update._AddRootNodeUpdate._PatchInformation,
							&_Update._AddRootNodeUpdate._PatchRenderInformation);

			//Calculate new borders.
			CalculateNewborders();

			return;
		}
	}

	//Check if a node should be subdivided.
	for (uint8 depth{ TerrainConstants::TERRAIN_QUAD_TREE_MAX_DEPTH }; depth > 0; --depth)
	{
		for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
		{
			if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
			{
				continue;
			}

			if (CheckCombination(depth, perceiverPosition, &_QuadTree._RootNodes[i]))
			{
				//Calculate new borders.
				CalculateNewborders();

				return;
			}
		}
	}

	//Check if a node should be subdivided.
	for (uint8 depth{ 0 }; depth < TerrainConstants::TERRAIN_QUAD_TREE_MAX_DEPTH; ++depth)
	{
		for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
		{
			if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
			{
				continue;
			}

			if (CheckSubdivision(depth, perceiverPosition, &_QuadTree._RootNodes[i]))
			{
				//Calculate new borders.
				CalculateNewborders();

				return;
			}
		}
	}
}

/*
*	Removes a quad tree node.
*/
void TerrainSystem::RemoveNode(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	//If this node is subdivided, remove it's children as well.
	if (node->_Subdivided)
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			RemoveNode(&node->_ChildNodes[i]);
		}
	}

	else
	{
		//Destroy the patch.
		const uint64 patchInformationIndex{ GetPatchInformationIndex(node->_Identifier) };
		DestroyPatch(patchInformationIndex);
	}
}

/*
*	Checks combination of a node. Returns whether or not the node was combined.
*/
bool TerrainSystem::CheckCombination(const uint8 depth, const Vector3<float> &perceiverPosition, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	//If this node is already subdivided, check all of it's child nodes.
	if (node->_Subdivided)
	{
		if (node->_Depth == depth && TerrainQuadTreeUtilities::ShouldBeCombined(*node, perceiverPosition))
		{
			CombineNode(node);

			return true;
		}

		else
		{
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				if (CheckCombination(depth, perceiverPosition, &node->_ChildNodes[i]))
				{
					return true;
				}
			}
		}
	}

	return false;
}

/*
*	Checks subdivisions of a node. Returns whether or not the node was subdivided.
*/
bool TerrainSystem::CheckSubdivision(const uint8 depth, const Vector3<float> &perceiverPosition, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	//Don't go further down than the depth.
	if (node->_Depth > depth)
	{
		return false;
	}

	//If this node is already subdivided, check all of it's child nodes.
	if (node->_Subdivided)
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			if (CheckSubdivision(depth, perceiverPosition, &node->_ChildNodes[i]))
			{
				return true;
			}
		}
	}

	//Else, check if this node should be subdivided.
	else
	{
		if (TerrainQuadTreeUtilities::ShouldBeSubdivided(*node, perceiverPosition))
		{
			SubdivideNode(node);

			return true;
		}
	}

	return false;
}

/*
*	Combine a node.
*/
void TerrainSystem::CombineNode(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	_Update._Type = TerrainUpdate::Type::CombineNode;
	_Update._CombineNodeUpdate._Node = node;

	const float patchSizeMultiplier{ TerrainQuadTreeUtilities::PatchSizeMultiplier(*node) };

	const Vector3<float> worldPosition{	node->_Minimum._X + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f,
										0.0f,
										node->_Minimum._Y + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f };

	GeneratePatch(	worldPosition,
					patchSizeMultiplier,
					node->_Depth,
					&_Update._CombineNodeUpdate._PatchInformation,
					&_Update._CombineNodeUpdate._PatchRenderInformation);

	//Set the node back to not subdivided.
	node->_Subdivided = false;

	//Add the patch information identifiers.
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		_Update._CombineNodeUpdate._PatchInformationIdentifiers[i] = node->_ChildNodes[i]._Identifier;
	}

	Memory::Free(node->_ChildNodes);

	node->_ChildNodes = nullptr;

	//Set the identifier.
	node->_Identifier = _Update._CombineNodeUpdate._PatchInformation._Identifier;
}

/*
*	Subdivides a node.
*/
void TerrainSystem::SubdivideNode(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	_Update._Type = TerrainUpdate::Type::SubdivideNode;
	_Update._SubdivideNodeUpdate._Node = node;

	const float patchSizeMultiplier{ TerrainQuadTreeUtilities::PatchSizeMultiplier(*node) * 0.5f };

	const StaticArray<Vector3<float>, 4> positions
	{
		Vector3<float>(	node->_Minimum._X + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 1.5f,
						0.0f,
						node->_Minimum._Y + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f),

		Vector3<float>(	node->_Minimum._X + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 1.5f,
						0.0f,
						node->_Minimum._Y + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 1.5f),

		Vector3<float>(	node->_Minimum._X + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f,
						0.0f,
						node->_Minimum._Y + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 1.5f),

		Vector3<float>(	node->_Minimum._X + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f,
						0.0f,
						node->_Minimum._Y + TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f)
	};

	node->_Subdivided = true;
	node->_ChildNodes = static_cast<TerrainQuadTreeNode *const RESTRICT>(Memory::Allocate(sizeof(TerrainQuadTreeNode) * 4));

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		GeneratePatch(	positions[i],
						patchSizeMultiplier,
						node->_Depth + 1,
						&_Update._SubdivideNodeUpdate._PatchInformations[i],
						&_Update._SubdivideNodeUpdate._PatchRenderInformations[i]);

		node->_ChildNodes[i]._Depth = _Update._SubdivideNodeUpdate._Node->_Depth + 1;
		node->_ChildNodes[i]._Subdivided = false;
		node->_ChildNodes[i]._Identifier = _Update._SubdivideNodeUpdate._PatchInformations[i]._Identifier;
		node->_ChildNodes[i]._ChildNodes = nullptr;
		node->_ChildNodes[i]._Minimum._X = _Update._SubdivideNodeUpdate._PatchInformations[i]._AxisAlignedBoundingBox._Minimum._X;
		node->_ChildNodes[i]._Minimum._Y = _Update._SubdivideNodeUpdate._PatchInformations[i]._AxisAlignedBoundingBox._Minimum._Z;
		node->_ChildNodes[i]._Maximum._X = _Update._SubdivideNodeUpdate._PatchInformations[i]._AxisAlignedBoundingBox._Maximum._X;
		node->_ChildNodes[i]._Maximum._Y = _Update._SubdivideNodeUpdate._PatchInformations[i]._AxisAlignedBoundingBox._Maximum._Z;
	}
}

/*
*	Generates a patch.
*/
void TerrainSystem::GeneratePatch(const Vector3<float> &worldPosition, const float patchSizeMultiplier, const uint8 depth, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT
{
	//Fill in the details about the patch information.
	patchInformation->_Identifier = TerrainGeneralUtilities::GeneratePatchIdentifier();
	patchInformation->_Valid = true;

	//Fill in the details about the patch render information.
	patchRenderInformation->_WorldPosition = Vector2<float>(worldPosition._X, worldPosition._Z);
	patchRenderInformation->_PatchSize = TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier;
	patchRenderInformation->_Borders = 0;

	//Generate the height texture.
	float minimumHeight;
	float maximumHeight;

	TerrainGeneralUtilities::GenerateHeightTexture(	_Properties,
													patchSizeMultiplier,
													worldPosition,
													&minimumHeight,
													&maximumHeight,
													&patchInformation->_HeightTexture);

	patchRenderInformation->_HeightTextureIndex = static_cast<int32>(RenderingSystem::Instance->AddTextureToGlobalRenderData(patchInformation->_HeightTexture));

	//Generate the terrain textures.
	//const uint32 textureResolution{ TerrainConstants::TERRAIN_TEXTURE_RESOLUTIONS[depth] };
	const uint32 textureResolution{ TerrainConstants::TERRAIN_TEXTURE_BASE_RESOLUTION };

	TerrainGeneralUtilities::GenerateTerrainTextures(	_Properties,
														patchSizeMultiplier,
														textureResolution,
														worldPosition,
														&patchInformation->_NormalTexture,
														&patchInformation->_MaterialTexture);

	patchRenderInformation->_NormalTextureIndex = static_cast<int32>(RenderingSystem::Instance->AddTextureToGlobalRenderData(patchInformation->_NormalTexture));
	patchRenderInformation->_MaterialTextureIndex = static_cast<int32>(RenderingSystem::Instance->AddTextureToGlobalRenderData(patchInformation->_MaterialTexture));
	patchRenderInformation->_MaterialTextureResolution = static_cast<float>(textureResolution);
	patchRenderInformation->_MaterialTextureResolutionSquared = static_cast<float>(textureResolution) * static_cast<float>(textureResolution);
	patchRenderInformation->_InverseMaterialTextureResolution = 1.0f / static_cast<float>(textureResolution);

	//Calculate the axis-aligned bounding box.
	patchInformation->_AxisAlignedBoundingBox._Minimum = Vector3<float>(worldPosition._X - (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f), minimumHeight, worldPosition._Z - (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f));
	patchInformation->_AxisAlignedBoundingBox._Maximum = Vector3<float>(worldPosition._X + (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f), maximumHeight, worldPosition._Z + (TerrainConstants::TERRAIN_PATCH_SIZE * patchSizeMultiplier * 0.5f));
}

/*
*	Destroys a patch.
*/
void TerrainSystem::DestroyPatch(const uint64 index) NOEXCEPT
{
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(_PatchRenderInformations[index]._HeightTextureIndex);
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(static_cast<uint32>(_PatchRenderInformations[index]._NormalTextureIndex));
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(static_cast<uint32>(_PatchRenderInformations[index]._MaterialTextureIndex));
	RenderingSystem::Instance->DestroyTexture2D(&_PatchInformations[index]._HeightTexture);
	RenderingSystem::Instance->DestroyTexture2D(&_PatchInformations[index]._NormalTexture);
	RenderingSystem::Instance->DestroyTexture2D(&_PatchInformations[index]._MaterialTexture);

	_PatchInformations.EraseAt(index);
	_PatchRenderInformations.EraseAt(index);
}

/*
*	Traverses the quad tree, calculates new borders for all nodes and fills in the update.
*/
void TerrainSystem::CalculateNewborders() NOEXCEPT
{
	//Iterate over all root nodes and calculate new borders for them.
	for (TerrainQuadTreeNode &rootNode : _QuadTree._RootNodes)
	{
		CalculateNewborders(&rootNode);
	}
}

/*
*	Calculates new borders for one node and fills in the update.
*/
void TerrainSystem::CalculateNewborders(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	//If this node is subdivided, calculate new borders for it's child nodes.
	if (node->_Subdivided)
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			CalculateNewborders(&node->_ChildNodes[i]);
		}
	}

	else
	{
		//Retrieve the neighboring nodes.
		StaticArray<const TerrainQuadTreeNode *RESTRICT, 4> neighboringNodes;
		TerrainQuadTreeUtilities::NeighboringNodes(_QuadTree, *node, &neighboringNodes);

		//Calculate new borders.
		int32 borders{ 0 };

		{
			const int32 delta{ neighboringNodes[0] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboringNodes[0]->_Depth) : 0 };

			if (delta >= 1)
			{
				borders |= BIT(0);
			}

			if (delta >= 2)
			{
				borders |= BIT(1);
			}

			if (delta >= 3)
			{
				ASSERT(false, "Add support for higher deltas!");
			}
		}

		{
			const int32 delta{ neighboringNodes[1] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboringNodes[1]->_Depth) : 0 };

			if (delta >= 1)
			{
				borders |= BIT(2);
			}

			if (delta >= 2)
			{
				borders |= BIT(3);
			}

			if (delta >= 3)
			{
				ASSERT(false, "Add support for higher deltas!");
			}
		}

		{
			const int32 delta{ neighboringNodes[2] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboringNodes[2]->_Depth) : 0 };

			if (delta >= 1)
			{
				borders |= BIT(4);
			}

			if (delta >= 2)
			{
				borders |= BIT(5);
			}

			if (delta >= 3)
			{
				ASSERT(false, "Add support for higher deltas!");
			}
		}

		{
			const int32 delta{ neighboringNodes[3] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboringNodes[3]->_Depth) : 0 };

			if (delta >= 1)
			{
				borders |= BIT(6);
			}

			if (delta >= 2)
			{
				borders |= BIT(7);
			}

			if (delta >= 3)
			{
				ASSERT(false, "Add support for higher deltas!");
			}
		}

		//Find the terrain patch render information index for this node.
		const uint64 patchRenderInformationIndex{ GetPatchInformationIndex(node->_Identifier) };

		//If the borders differ, add the new borders to the borders updates.
		if (patchRenderInformationIndex == UINT64_MAX || _PatchRenderInformations[patchRenderInformationIndex]._Borders != borders)
		{
			_Update._BordersUpdates.EmplaceSlow(Pair<const uint64, const int32>(node->_Identifier, borders));
		}
	}
}