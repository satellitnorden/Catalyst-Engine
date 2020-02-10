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
void TerrainSystem::Initialize(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT
{
	//Copy over the properties.
	_Properties._PatchSize = configuration._PatchSize;
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
*	Sets the height map.
*/
void TerrainSystem::SetHeightMap(const Texture2D<float> &height_map) NOEXCEPT
{
	//Copy the height map.
	_Properties._HeightMap = height_map;

	//Create the texture.
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(_Properties._HeightMap), TextureFormat::R32_Float), &_Properties._HeightMapTexture);

	//Add the texture to the global render data.
	_Properties._HeightMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(_Properties._HeightMapTexture);

	//Generate the terrain shadow plane.
	DynamicArray<Vertex> vertices;
	DynamicArray<uint32> indices;

	TerrainGeneralUtilities::GenerateTerrainShadowPlane(_Properties._HeightMap,
														&vertices,
														&indices);

	BufferHandle vertex_buffer;
	BufferHandle index_buffer;

	{
		const void* const RESTRICT dataChunks[]{ vertices.Data() };
		const uint64 dataSizes[]{ sizeof(Vertex) * vertices.Size() };
		RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::StorageBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &vertex_buffer);
		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &vertex_buffer);
	}

	{
		const void* const RESTRICT dataChunks[]{ indices.Data() };
		const uint64 dataSizes[]{ sizeof(uint32) * indices.Size() };
		RenderingSystem::Instance->CreateBuffer(dataSizes[0], BufferUsage::IndexBuffer | BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &index_buffer);
		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &index_buffer);
	}

	/*
	//Create the bottom level acceleration structure.
	AccelerationStructureHandle acceleration_structure;

	RenderingSystem::Instance->CreateBottomLevelAccelerationStructure(vertex_buffer,
																		static_cast<uint32>(vertices.Size()),
		index_buffer,
																		static_cast<uint32>(indices.Size()),
																		&acceleration_structure);

	//Add the acceleration structure to the static top level acceleration structure.
	RenderingSystem::Instance->GetRayTracingSystem()->AddStaticInstance(TopLevelAccelerationStructureInstanceData(MatrixConstants::IDENTITY, acceleration_structure, 0));
	*/

	//There is now a height map!
	_Properties._HasHeightMap = true;
}

/*
*	Sets the index map.
*/
void TerrainSystem::SetIndexMap(const Texture2D<Vector4<uint8>> &index_map) NOEXCEPT
{
	//Copy the index map.
	_Properties._IndexMap = index_map;

	//Create the texture.
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(_Properties._IndexMap), TextureFormat::R8G8B8A8_Byte), &_Properties._IndexMapTexture);

	//Add the texture to the global render data.
	_Properties._IndexMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(_Properties._IndexMapTexture);

	//There is now an index map!
	_Properties._HasIndexMap = true;
}

/*
*	Sets the blend map.
*/
void TerrainSystem::SetBlendMap(const Texture2D<Vector4<uint8>> &blend_map) NOEXCEPT
{
	//Copy the blend map.
	_Properties._BlendMap = blend_map;

	//Create the texture.
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(_Properties._BlendMap), TextureFormat::R8G8B8A8_Byte), &_Properties._BlendMapTexture);

	//Add the texture to the global render data.
	_Properties._BlendMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(_Properties._BlendMapTexture);

	//There is now a blend map!
	_Properties._HasBlendMap = true;
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
		const float half_resolution{ static_cast<float>(_Properties._HeightMap.GetWidth()) * 0.5f };
		const float full_resolution{ static_cast<float>(_Properties._HeightMap.GetWidth()) };

		const Vector2<float> coordinate{ (position._X + half_resolution) / full_resolution, (position._Z + half_resolution) / full_resolution };

		//Sample the height map.
		*height = _Properties._HeightMap.Sample(coordinate, AddressMode::ClampToEdge);

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
*	Can optionally retrieve the height and the normal at the same time.
*/
bool TerrainSystem::GetTerrainMaterialAtPosition(const Vector3<float>& position, uint8* const RESTRICT material, float* const RESTRICT height, Vector3<float>* const RESTRICT normal, const void* const RESTRICT context) const NOEXCEPT
{
	//Temporary; Just set to zero.
	*material = 0;
	if (normal) *normal = VectorConstants::UP;
	if (height) *height = 0.0f;

	//Return that the retrieval succeeded.
	return true;
}

/*
*	Processes the update.
*/
void TerrainSystem::ProcessUpdate() NOEXCEPT
{
	//Just copy the updated informations over.
	_PatchInformations = std::move(_Update._PatchInformations);
	_PatchRenderInformations = std::move(_Update._PatchRenderInformations);
}

/*
*	Updates the terrain system asynchronously.
*/
void TerrainSystem::UpdateAsynchronous() NOEXCEPT
{
	//If there are no maps, just return.
	if (!_Properties._HasHeightMap || !_Properties._HasIndexMap || !_Properties._HasBlendMap)
	{
		return;
	}

	//Get the current perceiver position.
	const Vector3<float> current_perceiver_position{ Perceiver::Instance->GetPosition() };

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

	/*
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
	*/

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
			_QuadTree._RootGridPoints = GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM);

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