//Header file.
#include <Systems/TerrainSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/CatalystProjectConfiguration.h>
#include <Core/General/Perceiver.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
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
void TerrainSystem::Initialize(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT
{
	//Copy over the properties.
	_Properties._PatchSize = CatalystEngineSystem::Instance->GetProjectConfiguration()->_WorldConfiguration._WorldGridSize;
	_Properties._PatchResolution = configuration._PatchResolution;
	_Properties._MaximumQuadTreeDepth = configuration._MaximumQuadTreeDepth;

	//Set the function for the update task.
	_UpdateTask._Function = [](void* const RESTRICT)
	{
		TerrainSystem::Instance->UpdateAsynchronous();
	};
	_UpdateTask._Arguments = nullptr;
	_UpdateTask._ExecutableOnSameThread = false;

	//Generate the terrain plane.
	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainGeneralUtilities::GenerateTerrainPlane(	_Properties,
													_Properties._PatchResolution,
													&vertices,
													&indices);

	StaticArray<void* RESTRICT, 2> bufferData;

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

	//Reserve an appropriate amount of memory.
	_PatchInformations.Reserve(UINT8_MAXIMUM);
	_PatchRenderInformations.Reserve(UINT8_MAXIMUM);
}

/*
*	Updates the terrain system during the sequential update phase.
*/
void TerrainSystem::SequentialUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT
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
*	Sets the world center.
*/
void TerrainSystem::SetWorldCenter(const WorldPosition &world_position) NOEXCEPT
{
	//Lock the world center.
	SCOPED_LOCK(_Properties._WorldCenterLock);

	//Set the world center.
	_Properties._WorldCenter = world_position;

	//There is now a world center!
	_Properties._HasWorldCenter = true;
}

/*
*	Sets the height map.
*/
void TerrainSystem::SetHeightMap(Texture2D<float32> &&height_map) NOEXCEPT
{
	//Create the texture.
	Texture2DHandle new_texture;
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(height_map), TextureFormat::R_FLOAT32), &new_texture);

	//Lock the height map.
	SCOPED_LOCK(_Properties._HeightMapLock);

	//If there already is a height map, destroy it.
	if (_Properties._HasHeightMap)
	{
		RenderingSystem::Instance->ReturnTextureToGlobalRenderData(_Properties._HeightMapTextureIndex);
		RenderingSystem::Instance->DestroyTexture2D(&_Properties._HeightMapTexture);
	}

	//Copy the height map.
	_Properties._HeightMap = std::move(height_map);

	//Set the texture.
	_Properties._HeightMapTexture = new_texture;

	//Add the texture to the global render data.
	_Properties._HeightMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(_Properties._HeightMapTexture);

	//There is now a height map!
	_Properties._HasHeightMap = true;
}

/*
*	Sets the index map.
*/
void TerrainSystem::SetIndexMap(Texture2D<Vector4<uint8>> &&index_map) NOEXCEPT
{
	//Create the texture.
	Texture2DHandle new_texture;
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(index_map), TextureFormat::RGBA_UINT8), &new_texture);

	//Lock the index map.
	SCOPED_LOCK(_Properties._IndexMapLock);

	//If there already is an index map, destroy it.
	if (_Properties._HasIndexMap)
	{
		RenderingSystem::Instance->ReturnTextureToGlobalRenderData(_Properties._IndexMapTextureIndex);
		RenderingSystem::Instance->DestroyTexture2D(&_Properties._IndexMapTexture);
	}

	//Copy the index map.
	_Properties._IndexMap = std::move(index_map);

	//Set the texture.
	_Properties._IndexMapTexture = new_texture;

	//Add the texture to the global render data.
	_Properties._IndexMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(_Properties._IndexMapTexture);

	//There is now an index map!
	_Properties._HasIndexMap = true;
}

/*
*	Sets the blend map.
*/
void TerrainSystem::SetBlendMap(Texture2D<Vector4<uint8>> &&blend_map) NOEXCEPT
{
	//Create the texture.
	Texture2DHandle new_texture;
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(blend_map), TextureFormat::RGBA_UINT8), &new_texture);

	//Lock the blend map.
	SCOPED_LOCK(_Properties._BlendMapLock);

	//If there already is a blend map, destroy it.
	if (_Properties._HasBlendMap)
	{
		RenderingSystem::Instance->ReturnTextureToGlobalRenderData(_Properties._BlendMapTextureIndex);
		RenderingSystem::Instance->DestroyTexture2D(&_Properties._BlendMapTexture);
	}

	//Copy the blend map.
	_Properties._BlendMap = std::move(blend_map);

	//Set the texture.
	_Properties._BlendMapTexture = new_texture;

	//Add the texture to the global render data.
	_Properties._BlendMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(_Properties._BlendMapTexture);

	//There is now a blend map!
	_Properties._HasBlendMap = true;
}

/*
*	Returns the terrain map coordinate at the given position.
*/
NO_DISCARD Vector2<float32> TerrainSystem::GetTerrainMapCoordinateAtPosition(const Vector3<float32> &position) const NOEXCEPT
{
	//Need that height map.
	if (_Properties._HasHeightMap)
	{
		//Apply the world center offset.
		Vector3<float32> offset_position;

		{
			SCOPED_LOCK(_Properties._WorldCenterLock);

			offset_position = position - _Properties._WorldCenter.GetAbsolutePosition();
		}

		//Calculate the coordinate. Assume that all maps has the same resolution.
		float32 half_resolution;
		float32 full_resolution;

		{
			SCOPED_LOCK(_Properties._HeightMapLock);

			half_resolution = static_cast<float32>(_Properties._HeightMap.GetWidth()) * 0.5f;
			full_resolution = static_cast<float32>(_Properties._HeightMap.GetWidth());
		}
		
		Vector2<float32> coordinate{ (offset_position._X - 0.5f + half_resolution) / full_resolution, (offset_position._Z - 0.5f + half_resolution) / full_resolution };

		//Clamp the coordinate.
		coordinate._X = CatalystBaseMath::Clamp<float32>(coordinate._X, 0.0f, 1.0f - FLOAT32_EPSILON);
		coordinate._Y = CatalystBaseMath::Clamp<float32>(coordinate._Y, 0.0f, 1.0f - FLOAT32_EPSILON);

		//Return the coordinate.
		return coordinate;
	}

	else
	{
		return Vector2<float>(0.0f, 0.0f);
	}
}

/*
*	Returns the terrain height at the given position.
*/
bool TerrainSystem::GetTerrainHeightAtPosition(const Vector3<float>& position, float* const RESTRICT height, const void* const RESTRICT context) const NOEXCEPT
{
	//If there's a height map, sample it.
	if (_Properties._HasHeightMap)
	{
		//Calculate the coordinate.
		const Vector2<float> coordinate{ GetTerrainMapCoordinateAtPosition(position) };

		//Sample the height map.
		{
			SCOPED_LOCK(_Properties._HeightMapLock);

			*height = _Properties._HeightMap.Sample(coordinate, AddressMode::CLAMP_TO_EDGE);
		}

		//Return that the retrieval succeeded.
		return true;
	}
	
	//Otherwise, just set to zero.
	else
	{
		*height = 0.0f;

		//Return that the retrieval failed.
		return false;
	}
}

/*
*	Returns the terrain normal at the given position.
*/
bool TerrainSystem::GetTerrainNormalAtPosition(const Vector3<float>& position, Vector3<float>* const RESTRICT normal, float* const RESTRICT height, const void* const RESTRICT context) const NOEXCEPT
{
	//Calculate the normal.
	float left;
	float right;
	float down;
	float up;

	GetTerrainHeightAtPosition(position + Vector3<float>(-1.0f, 0.0f, 0.0f), &left);
	GetTerrainHeightAtPosition(position + Vector3<float>(1.0f, 0.0f, 0.0f), &right);
	GetTerrainHeightAtPosition(position + Vector3<float>(0.0f, 0.0f, -1.0f), &down);
	GetTerrainHeightAtPosition(position + Vector3<float>(0.0f, 0.0f, 1.0f), &up);

	*normal = Vector3<float>::Normalize(Vector3<float>(left - right, 2.0f, down - up));

	if (height)
	{
		GetTerrainHeightAtPosition(position, height);
	}

	//Return that the retrieval succeeded.
	return true;
}

/*
*	Returns the terrain material at the given position.
*/
bool TerrainSystem::GetTerrainMaterialAtPosition(const Vector3<float> &position, Vector4<uint8> *const RESTRICT indices, Vector4<float> *const RESTRICT blend, const void *const RESTRICT context) const NOEXCEPT
{
	//If there's a index and blend map, sample them.
	if (_Properties._HasIndexMap && _Properties._HasBlendMap)
	{
		//Calculate the coordinate.
		const Vector2<float> coordinate{ GetTerrainMapCoordinateAtPosition(position) };

		//Sample the index map.
		{
			SCOPED_LOCK(_Properties._IndexMapLock);

			const uint32 x_coordinate{ static_cast<uint32>(coordinate._X * static_cast<float>(_Properties._IndexMap.GetWidth())) };
			const uint32 y_coordinate{ static_cast<uint32>(coordinate._Y * static_cast<float>(_Properties._IndexMap.GetWidth())) };

			*indices = _Properties._IndexMap.At(x_coordinate, y_coordinate);
		}

		//Sample the blend map.
		{
			SCOPED_LOCK(_Properties._BlendMapLock);

			const Vector4<uint8> blend_map_sample{ _Properties._BlendMap.Sample(coordinate, AddressMode::CLAMP_TO_EDGE) };

			*blend = Vector4<float32>(	static_cast<float32>(blend_map_sample._X) / static_cast<float32>(UINT8_MAXIMUM),
										static_cast<float32>(blend_map_sample._Y) / static_cast<float32>(UINT8_MAXIMUM),
										static_cast<float32>(blend_map_sample._Z) / static_cast<float32>(UINT8_MAXIMUM),
										static_cast<float32>(blend_map_sample._W) / static_cast<float32>(UINT8_MAXIMUM));
		}

		//Return that the retrieval succeeded.
		return true;
	}

	else
	{
		//Return that the retrieval failed.
		return false;
	}
}

/*
*	Processes the update.
*/
void TerrainSystem::ProcessUpdate() NOEXCEPT
{
	//Just copy the updated informations over.
	_PatchInformations = std::move(_Update._PatchInformations);
	_PatchRenderInformations = std::move(_Update._PatchRenderInformations);

	if (RenderingSystem::Instance->IsRayTracingActive())
	{
		//Update the terrain top level acceleration structure.
		RenderingSystem::Instance->GetRayTracingSystem()->SetTerrainBottomLevelAccelerationStructure(_TerrainRayTracingData._BottomLevelAccelerationStructures[_TerrainRayTracingData._CurrentBufferIndex]);
	
		//Update the current buffer index.
		_TerrainRayTracingData._CurrentBufferIndex ^= static_cast<uint8>(1);
	}
}

/*
*	Updates the terrain system asynchronously.
*/
void TerrainSystem::UpdateAsynchronous() NOEXCEPT
{
	//If there are no maps, just return.
	if (!_Properties._HasWorldCenter && !_Properties._HasHeightMap || !_Properties._HasIndexMap || !_Properties._HasBlendMap)
	{
		return;
	}

	//Cache the current perceiver position.
	const Vector3<float32> current_perceiver_position{ Perceiver::Instance->GetWorldTransform().GetAbsolutePosition() };

	//Calculate the perceiver grid point.
	const GridPoint2 current_grid_point{ GridPoint2::WorldPositionToGridPoint(current_perceiver_position, _Properties._PatchSize) };

	//Calculate the valid grid points.
	const StaticArray<GridPoint2, 9> valid_grid_points
	{
		GridPoint2(current_grid_point._X, current_grid_point._Y),

		GridPoint2(current_grid_point._X - 1, current_grid_point._Y - 1),
		GridPoint2(current_grid_point._X - 1, current_grid_point._Y),
		GridPoint2(current_grid_point._X - 1, current_grid_point._Y + 1),

		GridPoint2(current_grid_point._X, current_grid_point._Y - 1),
		GridPoint2(current_grid_point._X, current_grid_point._Y + 1),

		GridPoint2(current_grid_point._X + 1, current_grid_point._Y - 1),
		GridPoint2(current_grid_point._X + 1, current_grid_point._Y),
		GridPoint2(current_grid_point._X + 1, current_grid_point._Y + 1)
	};


	//If a root grid point does exist that isn't valid, remove it.
	for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
	{
		if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
		{
			continue;
		}

		bool exists{ false };

		for (const GridPoint2 valid_grid_point : valid_grid_points)
		{
			if (valid_grid_point == _QuadTree._RootGridPoints[i])
			{
				exists = true;

				break;
			}
		}

		if (!exists)
		{
			//Remove the root node.
			RemoveRootNode(_QuadTree._RootGridPoints[i]);
		}
	}

	//If a valid grid point does not exist, construct a new update.
	for (const GridPoint2 valid_grid_point : valid_grid_points)
	{
		bool exists{ false };

		for (const GridPoint2 root_grid_point : _QuadTree._RootGridPoints)
		{
			if (root_grid_point == valid_grid_point)
			{
				exists = true;

				break;
			}
		}

		if (!exists)
		{
			//Add the root node.
			AddRootNode(valid_grid_point);
		}
	}

	//Check if a node should be combined.
	for (uint8 depth{ _Properties._MaximumQuadTreeDepth }; depth > 0; --depth)
	{
		for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
		{
			if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
			{
				continue;
			}

			CheckCombination(depth, current_perceiver_position, &_QuadTree._RootNodes[i]);
		}
	}

	//Check if a node should be subdivided.
	for (uint8 depth{ 0 }; depth < _Properties._MaximumQuadTreeDepth; ++depth)
	{
		for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
		{
			if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
			{
				continue;
			}

			CheckSubdivision(depth, current_perceiver_position, &_QuadTree._RootNodes[i]);
		}
	}

	//Calculate the borders.
	CalculateNewBorders();

	//Generate the patch informations.
	for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
	{
		if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
		{
			continue;
		}

		GeneratePatchInformations(&_QuadTree._RootNodes[i]);
	}

	//Update the terrain ray tracing data.
	UpdateTerrainRayTracingData();
}

/*
*	Removes a quad tree root node.
*/
void TerrainSystem::RemoveRootNode(const GridPoint2 grid_point) NOEXCEPT
{
	//Iterate over all root grid points to see which one matches.
	for (uint64 i{ 0 }, size{ _QuadTree._RootGridPoints.Size() }; i < size; ++i)
	{
		if (_QuadTree._RootGridPoints[i] == grid_point)
		{
			//Set the root grid point to invalid.
			_QuadTree._RootGridPoints[i] = GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM);

			//Remove the node.
			RemoveNode(&_QuadTree._RootNodes[i]);

			break;
		}
	}
}

/*
*	Adds a quad tree root node.
*/
void TerrainSystem::AddRootNode(const GridPoint2 grid_point) NOEXCEPT
{
	for (uint64 i{ 0 }, size{ _QuadTree._RootGridPoints.Size() }; i < size; ++i)
	{
		if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
		{
			const Vector3<float> grid_point_world_position{ GridPoint2::GridPointToWorldPosition(grid_point, _Properties._PatchSize) };

			_QuadTree._RootGridPoints[i] = grid_point;
			_QuadTree._RootNodes[i]._Depth = 0;
			_QuadTree._RootNodes[i]._Borders = 0;
			_QuadTree._RootNodes[i]._Subdivided = false;
			_QuadTree._RootNodes[i]._ChildNodes = nullptr;
			_QuadTree._RootNodes[i]._Minimum = Vector2<float>(grid_point_world_position._X - (_Properties._PatchSize * 0.5f), grid_point_world_position._Z - (_Properties._PatchSize * 0.5f));
			_QuadTree._RootNodes[i]._Maximum = Vector2<float>(grid_point_world_position._X + (_Properties._PatchSize * 0.5f), grid_point_world_position._Z + (_Properties._PatchSize * 0.5f));

			break;
		}
	}
}

/*
*	Removes a node.
*/
void TerrainSystem::RemoveNode(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	//If this node is subdivided, remove it's children as well.
	if (node->_Subdivided)
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			RemoveNode(&node->_ChildNodes[i]);
		}

		Memory::Free(node->_ChildNodes);

		node->_Subdivided = false;
	}
}

/*
*	Checks combination of a node. Returns whether or not the node was combined.
*/
bool TerrainSystem::CheckCombination(const uint8 depth, const Vector3<float>& perceiverPosition, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	//If this node is already subdivided, check all of it's child nodes.
	if (node->_Subdivided)
	{
		if (node->_Depth == depth && TerrainQuadTreeUtilities::ShouldBeCombined(_Properties, *node, perceiverPosition))
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
bool TerrainSystem::CheckSubdivision(const uint8 depth, const Vector3<float>& perceiverPosition, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
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
		if (TerrainQuadTreeUtilities::ShouldBeSubdivided(_Properties, *node, perceiverPosition))
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
void TerrainSystem::CombineNode(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	//Set the node back to not subdivided.
	node->_Subdivided = false;

	Memory::Free(node->_ChildNodes);

	node->_ChildNodes = nullptr;
}

/*
*	Subdivides a node.
*/
void TerrainSystem::SubdivideNode(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	const float patch_size_multiplier{ TerrainQuadTreeUtilities::PatchSizeMultiplier(*node) * 0.5f };

	const StaticArray<Vector3<float>, 4> positions
	{
		Vector3<float>(node->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 1.5f,
						0.0f,
						node->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 0.5f),

		Vector3<float>(node->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 1.5f,
						0.0f,
						node->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 1.5f),

		Vector3<float>(node->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 0.5f,
						0.0f,
						node->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 1.5f),

		Vector3<float>(node->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 0.5f,
						0.0f,
						node->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 0.5f)
	};

	node->_Subdivided = true;
	node->_ChildNodes = static_cast<TerrainQuadTreeNode* const RESTRICT>(Memory::Allocate(sizeof(TerrainQuadTreeNode) * 4));

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		node->_ChildNodes[i]._Depth = node->_Depth + 1;
		node->_ChildNodes[i]._Borders = 0;
		node->_ChildNodes[i]._Subdivided = false;
		node->_ChildNodes[i]._ChildNodes = nullptr;
		node->_ChildNodes[i]._Minimum = Vector2<float>(positions[i]._X - (_Properties._PatchSize * patch_size_multiplier * 0.5f), positions[i]._Z - (_Properties._PatchSize * patch_size_multiplier * 0.5f));
		node->_ChildNodes[i]._Maximum = Vector2<float>(positions[i]._X + (_Properties._PatchSize * patch_size_multiplier * 0.5f), positions[i]._Z + (_Properties._PatchSize * patch_size_multiplier * 0.5f));
	}
}

/*
*	Traverses the quad tree, calculates new borders for all nodes and fills in the update.
*/
void TerrainSystem::CalculateNewBorders() NOEXCEPT
{
	//Iterate over all root nodes and calculate new borders for them.
	for (TerrainQuadTreeNode& rootNode : _QuadTree._RootNodes)
	{
		CalculateNewBorders(&rootNode);
	}
}

/*
*	Calculates new borders for one node and fills in the update.
*/
void TerrainSystem::CalculateNewBorders(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	//If this node is subdivided, calculate new borders for it's child nodes.
	if (node->_Subdivided)
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			CalculateNewBorders(&node->_ChildNodes[i]);
		}
	}

	else
	{
		//Retrieve the neighboring nodes.
		StaticArray<const TerrainQuadTreeNode * RESTRICT, 4> neighboring_nodes;
		TerrainQuadTreeUtilities::NeighboringNodes(_Properties, _QuadTree, *node, &neighboring_nodes);

		//Calculate new borders.
		int32 borders{ 0 };

		{
			const int32 delta{ neighboring_nodes[0] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboring_nodes[0]->_Depth) : 0 };

			if (delta >= 1)
			{
				borders |= BIT(0);
			}

			if (delta >= 2)
			{
				borders |= BIT(1);
			}
		}

		{
			const int32 delta{ neighboring_nodes[1] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboring_nodes[1]->_Depth) : 0 };

			if (delta >= 1)
			{
				borders |= BIT(2);
			}

			if (delta >= 2)
			{
				borders |= BIT(3);
			}
		}

		{
			const int32 delta{ neighboring_nodes[2] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboring_nodes[2]->_Depth) : 0 };

			if (delta >= 1)
			{
				borders |= BIT(4);
			}

			if (delta >= 2)
			{
				borders |= BIT(5);
			}
		}

		{
			const int32 delta{ neighboring_nodes[3] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboring_nodes[3]->_Depth) : 0 };

			if (delta >= 1)
			{
				borders |= BIT(6);
			}

			if (delta >= 2)
			{
				borders |= BIT(7);
			}
		}

		//Update the node with the borders.
		node->_Borders = borders;
	}
}

/*
*	Generates the patch informations.
*/
void TerrainSystem::GeneratePatchInformations(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	//If this node is subdivided, generate patch informations for it's children instead.
	if (node->_Subdivided)
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			GeneratePatchInformations(&node->_ChildNodes[i]);
		}
	}

	else
	{
		//Calculate the world position.
		const float patch_size_multiplier{ TerrainQuadTreeUtilities::PatchSizeMultiplier(*node) };

		const Vector3<float> world_position{	node->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 0.5f,
												0.0f,
												node->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 0.5f };

		//Generate the informations.
		_Update._PatchInformations.Emplace();
		_Update._PatchRenderInformations.Emplace();

		TerrainPatchInformation &information{ _Update._PatchInformations.Back() };
		TerrainPatchRenderInformation &render_information{ _Update._PatchRenderInformations.Back() };

		information._AxisAlignedBoundingBox._Minimum = Vector3<float>(node->_Minimum._X, -FLOAT_MAXIMUM, node->_Minimum._Y);
		information._AxisAlignedBoundingBox._Maximum = Vector3<float>(node->_Maximum._X, FLOAT_MAXIMUM, node->_Maximum._Y);
		render_information._WorldPosition = Vector2<float>(world_position._X, world_position._Z);
		render_information._PatchSize = _Properties._PatchSize * patch_size_multiplier;
		render_information._Borders = node->_Borders;
		render_information._Visibility = false;
	}
}

/*
*	Updates the terrain ray tracing data.
*/
void TerrainSystem::UpdateTerrainRayTracingData() NOEXCEPT
{
	//No need to update if ray tracing isn't active.
	if (!RenderingSystem::Instance->IsRayTracingActive())
	{
		return;
	}

	//Define constants.
	constexpr StaticArray<float, 2> VERTEX_BORDER_OFFSETS{ 1.0f / 64.0f, 1.0f / 32.0f };

	//Destroy the current data.
	if (_TerrainRayTracingData._VertexBuffers[_TerrainRayTracingData._CurrentBufferIndex])
	{
		RenderingSystem::Instance->DestroyBuffer(&_TerrainRayTracingData._VertexBuffers[_TerrainRayTracingData._CurrentBufferIndex]);

		_TerrainRayTracingData._VertexBuffers[_TerrainRayTracingData._CurrentBufferIndex] = EMPTY_HANDLE;
	}

	if (_TerrainRayTracingData._IndexBuffers[_TerrainRayTracingData._CurrentBufferIndex])
	{
		RenderingSystem::Instance->DestroyBuffer(&_TerrainRayTracingData._IndexBuffers[_TerrainRayTracingData._CurrentBufferIndex]);

		_TerrainRayTracingData._IndexBuffers[_TerrainRayTracingData._CurrentBufferIndex] = EMPTY_HANDLE;
	}

	if (_TerrainRayTracingData._BottomLevelAccelerationStructures[_TerrainRayTracingData._CurrentBufferIndex])
	{
		RenderingSystem::Instance->DestroyAccelerationStructure(&_TerrainRayTracingData._BottomLevelAccelerationStructures[_TerrainRayTracingData._CurrentBufferIndex]);

		_TerrainRayTracingData._BottomLevelAccelerationStructures[_TerrainRayTracingData._CurrentBufferIndex] = EMPTY_HANDLE;
	}

	//Keep track of the indices offset.
	uint32 indices_offset{ 0 };

	//Generate the terrain vertices and indices.
	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainGeneralUtilities::GenerateTerrainPlane(	_Properties,
													_Properties._PatchResolution,
													&vertices,
													&indices);

	//Iterate over all terrain patches and add their data.
	DynamicArray<Vertex> master_vertices;
	DynamicArray<uint32> master_indices;

	const TerrainPatchInformation* RESTRICT patch_information{ _Update._PatchInformations.Data() };
	const TerrainPatchRenderInformation* RESTRICT patch_render_information{ _Update._PatchRenderInformations.Data() };

	for (uint64 i{ 0 }, size{ _Update._PatchInformations.Size() }; i < size; ++i, ++patch_information, ++patch_render_information)
	{
		//Cache relevant data.
		const int32 patch_borders{ patch_render_information->_Borders };

		//Add all vertex data.
		for (const TerrainVertex& vertex : vertices)
		{
			//Calculate the position.
			Vector3<float32> position;

			position._X = vertex._Position._X;
			position._Y = 0.0f;
			position._Z = vertex._Position._Y;

			//Apply the border offsets.
			{
				//Cache relevant data.
				const int32 vertex_borders{ vertex._Borders };

				//Calculate the horizontal border offset multiplier.
				float32 is_upper_multiplier{ static_cast<float>((vertex_borders & BIT(0)) & (patch_borders & BIT(0))) };
				float32 is_lower_multiplier{ static_cast<float>((vertex_borders & BIT(4)) & (patch_borders & BIT(4))) };
				float32 horizontal_border_offset_weight{ CatalystBaseMath::Minimum<float32>(is_upper_multiplier + is_lower_multiplier, 1.0f) };

				//Calculate the vertical border offset multiplier.
				float32 is_right_multiplier{ static_cast<float>((vertex_borders & BIT(2)) & (patch_borders & BIT(2))) };
				float32 is_left_multiplier{ static_cast<float>((vertex_borders & BIT(6)) & (patch_borders & BIT(6))) };
				float32 vertical_border_offset_weight{ CatalystBaseMath::Minimum<float32>(is_right_multiplier + is_left_multiplier, 1.0f) };

				position._X -= VERTEX_BORDER_OFFSETS[0] * horizontal_border_offset_weight;
				position._Z -= VERTEX_BORDER_OFFSETS[0] * vertical_border_offset_weight;

				//Calculate the horizontal border offset multiplier.
				is_upper_multiplier = static_cast<float>((vertex_borders& BIT(1))& (patch_borders& BIT(1)));
				is_lower_multiplier = static_cast<float>((vertex_borders& BIT(5))& (patch_borders& BIT(5)));
				horizontal_border_offset_weight = CatalystBaseMath::Minimum<float32>(is_upper_multiplier + is_lower_multiplier, 1.0f);

				//Calculate the vertical border offset multiplier.
				is_right_multiplier = static_cast<float>((vertex_borders& BIT(3))& (patch_borders& BIT(3)));
				is_left_multiplier = static_cast<float>((vertex_borders& BIT(7))& (patch_borders& BIT(7)));
				vertical_border_offset_weight = CatalystBaseMath::Minimum<float32>(is_right_multiplier + is_left_multiplier, 1.0f);

				position._X -= VERTEX_BORDER_OFFSETS[1] * horizontal_border_offset_weight;
				position._Z -= VERTEX_BORDER_OFFSETS[1] * vertical_border_offset_weight;
			}

			position._X = patch_render_information->_WorldPosition._X + position._X * patch_render_information->_PatchSize;
			position._Z = patch_render_information->_WorldPosition._Y + position._Z * patch_render_information->_PatchSize;

			TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &position._Y);

			//To alleviate discontinuities with displacement.
			position._Y -= 0.5f;

			//Add the vertex.
			master_vertices.Emplace(position, CatalystWorldCoordinateSpace::UP, CatalystWorldCoordinateSpace::RIGHT, Vector2<float>(0.0f, 0.0f));

		}

		//Add the indices
		for (uint64 j{ 0 }; j < indices.Size(); ++j)
		{
			master_indices.Emplace(indices[j] + indices_offset);
		}

		//Update the indices offset.
		indices_offset += static_cast<uint32>(vertices.Size());
	}

	//Create the buffers.
	{
		const void *const RESTRICT dataChunks[]{ master_vertices.Data() };
		const uint64 dataSizes[]{ sizeof(Vertex) * master_vertices.Size() };
		RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::StorageBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &_TerrainRayTracingData._VertexBuffers[_TerrainRayTracingData._CurrentBufferIndex]);
		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &_TerrainRayTracingData._VertexBuffers[_TerrainRayTracingData._CurrentBufferIndex]);
	}

	{
		const void *const RESTRICT dataChunks[]{ master_indices.Data() };
		const uint64 dataSizes[]{ sizeof(uint32) * master_indices.Size() };
		RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::IndexBuffer | BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &_TerrainRayTracingData._IndexBuffers[_TerrainRayTracingData._CurrentBufferIndex]);
		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &_TerrainRayTracingData._IndexBuffers[_TerrainRayTracingData._CurrentBufferIndex]);
	}

	//Create the bottom level acceleration structure.
	RenderingSystem::Instance->CreateBottomLevelAccelerationStructure(	_TerrainRayTracingData._VertexBuffers[_TerrainRayTracingData._CurrentBufferIndex],
																		static_cast<uint32>(master_vertices.Size()),
																		_TerrainRayTracingData._IndexBuffers[_TerrainRayTracingData._CurrentBufferIndex],
																		static_cast<uint32>(master_indices.Size()),
																		&_TerrainRayTracingData._BottomLevelAccelerationStructures[_TerrainRayTracingData._CurrentBufferIndex]);
}