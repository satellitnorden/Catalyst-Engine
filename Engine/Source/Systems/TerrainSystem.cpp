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
#include <Systems/ResourceSystem.h>
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
	_Properties._TerrainHeightFunction = configuration._TerrainHeightFunction;
	_Properties._TerrainMaterialFunction = configuration._TerrainMaterialFunction;
	_Properties._TerrainDataSaveFolder = configuration._TerrainDataSaveFolder;

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

	StaticArray<void* RESTRICT, 2> buffer_data;

	buffer_data[0] = vertices.Data();
	buffer_data[1] = indices.Data();

	StaticArray<uint64, 2> buffer_data_sizes;

	buffer_data_sizes[0] = sizeof(TerrainVertex) * vertices.Size();
	buffer_data_sizes[1] = sizeof(uint32) * indices.Size();

	RenderingSystem::Instance->CreateBuffer(buffer_data_sizes[0] + buffer_data_sizes[1], BufferUsage::IndexBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &_Properties._Buffer);
	RenderingSystem::Instance->UploadDataToBuffer(buffer_data.Data(), buffer_data_sizes.Data(), 2, &_Properties._Buffer);

	_Properties._IndexOffset = buffer_data_sizes[0];
	_Properties._IndexCount = static_cast<uint32>(indices.Size());

	//Initialize the quad tree.
	_QuadTree.Initialize();

	//Reserve an appropriate amount of memory.
	_PatchInformations.Reserve(UINT8_MAXIMUM);
	_PatchRenderInformations.Reserve(UINT8_MAXIMUM);
}

/*
*	Post-initializes the terrain system.
*/
void TerrainSystem::PostInitialize(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT
{
	//Initialize the terrain height generation compute pipeline.
	if (!configuration._TerrainHeightGenerationShaderIdentifier.Empty())
	{
		_TerrainHeightGenerationComputePipeline.Initialize(ResourceSystem::Instance->GetShaderResource(configuration._TerrainHeightGenerationShaderIdentifier));
		RenderingSystem::Instance->InitializePipeline(&_TerrainHeightGenerationComputePipeline);
	}

	//Initialize the terrain materials generation compute pipeline.
	if (!configuration._TerrainMaterialsGenerationShaderIdentifier.Empty())
	{
		_TerrainMaterialsGenerationComputePipeline.Initialize(ResourceSystem::Instance->GetShaderResource(configuration._TerrainMaterialsGenerationShaderIdentifier));
		RenderingSystem::Instance->InitializePipeline(&_TerrainMaterialsGenerationComputePipeline);
	}

	//If the height and/or materials generation shaders exist, create the command buffer and event.
	if (!configuration._TerrainHeightGenerationShaderIdentifier.Empty()
		|| !configuration._TerrainMaterialsGenerationShaderIdentifier.Empty())
	{
		//Create the command buffer.
		CommandPoolHandle command_pool;
		RenderingSystem::Instance->CreateCommandPool(Pipeline::Type::Compute, &command_pool);

		_CommandBuffer = RenderingSystem::Instance->AllocateCommandBuffer(command_pool, CommandBufferLevel::PRIMARY);

#if TERRAIN_SYSTEM_TIMESTAMP_GPU_GENERATION
		//Create the query pool.
		RenderingSystem::Instance->CreateQueryPool(&_QueryPool);
#endif

		//Create the event.
		RenderingSystem::Instance->CreateEvent(&_TerrainGenerationEvent);
	}
}

/*
*	Updates the terrain system during the sequential update phase.
*/
void TerrainSystem::SequentialUpdate() NOEXCEPT
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
*	Returns if terrain can be generated.
*/
NO_DISCARD bool TerrainSystem::CanTerrainBeGenerated() const NOEXCEPT
{
	//Need the functions.
	return _Properties._TerrainHeightFunction && _Properties._TerrainMaterialFunction;
}

/*
*	Sets the maximum quad tree depth.
*/
void TerrainSystem::SetMaximumQuadTreeDepth(const uint8 value) NOEXCEPT
{
	_Properties._MaximumQuadTreeDepth = value;
}

/*
*	Returns the terrain map coordinate at the given position.
*/
NO_DISCARD Vector2<float32> TerrainSystem::GetTerrainMapCoordinateAtPosition(const Vector3<float32> &position) const NOEXCEPT
{
	/*
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
	*/
	{
		return Vector2<float>(0.0f, 0.0f);
	}
}

/*
*	Returns the terrain height at the given position.
*/
bool TerrainSystem::GetTerrainHeightAtPosition(const Vector3<float>& position, float* const RESTRICT height, const void* const RESTRICT context) const NOEXCEPT
{
	if (!_Properties._TerrainHeightFunction)
	{
		if (height)
		{
			*height = 0.0f;
		}

		return false;
	}

	else
	{
		if (height)
		{
			*height = _Properties._TerrainHeightFunction(WorldPosition(position));
		}

		return true;
	}
}

/*
*	Returns the terrain normal at the given position.
*/
bool TerrainSystem::GetTerrainNormalAtPosition(const Vector3<float>& position, Vector3<float>* const RESTRICT normal, float* const RESTRICT height, const void* const RESTRICT context) const NOEXCEPT
{
	//Calculate the normal.
	float32 left;
	float32 right;
	float32 down;
	float32 up;

	GetTerrainHeightAtPosition(position + Vector3<float32>(-1.0f, 0.0f, 0.0f), &left);
	GetTerrainHeightAtPosition(position + Vector3<float32>(1.0f, 0.0f, 0.0f), &right);
	GetTerrainHeightAtPosition(position + Vector3<float32>(0.0f, 0.0f, -1.0f), &down);
	GetTerrainHeightAtPosition(position + Vector3<float32>(0.0f, 0.0f, 1.0f), &up);

	*normal = Vector3<float32>::Normalize(Vector3<float32>(left - right, 2.0f, down - up));

	if (height)
	{
		GetTerrainHeightAtPosition(position, height);
	}

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

	for (Pair<Texture2DHandle, uint32> &texture_to_remove : _Update._TexturesToRemove)
	{
		RenderingSystem::Instance->DestroyTexture2D(&texture_to_remove._First);
		RenderingSystem::Instance->ReturnTextureToGlobalRenderData(texture_to_remove._Second);
	}

	_Update._TexturesToRemove.Clear();

	/*
	if (RenderingSystem::Instance->IsRayTracingActive())
	{
		//Update the terrain top level acceleration structure.
		RenderingSystem::Instance->GetRayTracingSystem()->SetTerrainBottomLevelAccelerationStructure(_TerrainRayTracingData._BottomLevelAccelerationStructures[_TerrainRayTracingData._CurrentBufferIndex]);
	
		//Update the current buffer index.
		_TerrainRayTracingData._CurrentBufferIndex ^= static_cast<uint8>(1);
	}
	*/
}

/*
*	Updates the terrain system asynchronously.
*/
void TerrainSystem::UpdateAsynchronous() NOEXCEPT
{
	//If there are no proper functions, just return.
	if (!_Properties._TerrainHeightFunction || !_Properties._TerrainMaterialFunction)
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

	//Find the highest depth of any node.
	uint8 highest_depth{ 0 };

	for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
	{
		if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
		{
			continue;
		}

		FindHighestDepth(_QuadTree._RootNodes[i], &highest_depth);
	}

	//Check if a node should be combined.
	for (uint8 depth{ highest_depth }; depth > 0; --depth)
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

	//Finish the terrain generation.
	FinishTerrainGeneration();
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
			_QuadTree._RootNodes[i]._Minimum = Vector2<float>(grid_point_world_position._X - (_Properties._PatchSize * 0.5f), grid_point_world_position._Z - (_Properties._PatchSize * 0.5f));
			_QuadTree._RootNodes[i]._Maximum = Vector2<float>(grid_point_world_position._X + (_Properties._PatchSize * 0.5f), grid_point_world_position._Z + (_Properties._PatchSize * 0.5f));
			GenerateMaps(&_QuadTree._RootNodes[i]);

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
	if (node->IsSubdivided())
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			RemoveNode(&node->_ChildNodes[i]);
		}

		node->_ChildNodes.Clear();
	}

	//Destroy the maps.
	DestroyMaps(node);
}

/*
*	Checks combination of a node.
*/
void TerrainSystem::CheckCombination(const uint8 depth, const Vector3<float>& perceiverPosition, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	//If this node is already subdivided, check all of it's child nodes.
	if (node->IsSubdivided())
	{
		if (node->_Depth == depth && TerrainQuadTreeUtilities::ShouldBeCombined(_Properties, *node, perceiverPosition))
		{
			CombineNode(node);
		}

		else
		{
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				CheckCombination(depth, perceiverPosition, &node->_ChildNodes[i]);
			}
		}
	}
}

/*
*	Checks subdivisions of a node.
*/
void TerrainSystem::CheckSubdivision(const uint8 depth, const Vector3<float>& perceiverPosition, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	//Don't go further down than the depth.
	if (node->_Depth > depth)
	{
		return;
	}

	//If this node is already subdivided, check all of it's child nodes.
	if (node->IsSubdivided())
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			CheckSubdivision(depth, perceiverPosition, &node->_ChildNodes[i]);
		}
	}

	//Else, check if this node should be subdivided.
	else
	{
		if (TerrainQuadTreeUtilities::ShouldBeSubdivided(_Properties, *node, perceiverPosition))
		{
			SubdivideNode(node);
		}
	}
}

/*
*	Combine a node.
*/
void TerrainSystem::CombineNode(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		DestroyMaps(&node->_ChildNodes[i]);
	}

	node->_ChildNodes.Clear();

	GenerateMaps(node);
}

/*
*	Subdivides a node.
*/
void TerrainSystem::SubdivideNode(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	DestroyMaps(node);

	const float32 patch_size_multiplier{ TerrainQuadTreeUtilities::PatchSizeMultiplier(*node) * 0.5f };

	const StaticArray<Vector3<float32>, 4> positions
	{
		Vector3<float32>(node->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 1.5f,
						0.0f,
						node->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 0.5f),

		Vector3<float32>(node->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 1.5f,
						0.0f,
						node->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 1.5f),

		Vector3<float32>(node->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 0.5f,
						0.0f,
						node->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 1.5f),

		Vector3<float32>(node->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 0.5f,
						0.0f,
						node->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 0.5f)
	};

	node->_ChildNodes.Upsize<true>(4);

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		node->_ChildNodes[i]._Depth = node->_Depth + 1;
		node->_ChildNodes[i]._Borders = 0;
		node->_ChildNodes[i]._Minimum = Vector2<float>(positions[i]._X - (_Properties._PatchSize * patch_size_multiplier * 0.5f), positions[i]._Z - (_Properties._PatchSize * patch_size_multiplier * 0.5f));
		node->_ChildNodes[i]._Maximum = Vector2<float>(positions[i]._X + (_Properties._PatchSize * patch_size_multiplier * 0.5f), positions[i]._Z + (_Properties._PatchSize * patch_size_multiplier * 0.5f));
		GenerateMaps(&node->_ChildNodes[i]);
	}
}

/*
*	Finds the highest depth.
*/
void TerrainSystem::FindHighestDepth(const TerrainQuadTreeNode &node, uint8 *const RESTRICT highest_depth) NOEXCEPT
{
	if (node.IsSubdivided())
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			FindHighestDepth(node._ChildNodes[i], highest_depth);
		}
	}

	else
	{
		*highest_depth = CatalystBaseMath::Maximum<uint8>(*highest_depth, node._Depth);
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
	if (node->IsSubdivided())
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
	if (node->IsSubdivided())
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			GeneratePatchInformations(&node->_ChildNodes[i]);
		}
	}

	else
	{
		//Calculate the world position.
		const float32 patch_size_multiplier{ TerrainQuadTreeUtilities::PatchSizeMultiplier(*node) };

		const Vector3<float32> world_position{	node->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 0.5f,
												0.0f,
												node->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 0.5f };

		//Generate the informations.
		_Update._PatchInformations.Emplace();
		_Update._PatchRenderInformations.Emplace();

		TerrainPatchInformation &information{ _Update._PatchInformations.Back() };
		TerrainPatchRenderInformation &render_information{ _Update._PatchRenderInformations.Back() };

		information._AxisAlignedBoundingBox._Minimum = Vector3<float32>(node->_Minimum._X, -FLOAT_MAXIMUM, node->_Minimum._Y);
		information._AxisAlignedBoundingBox._Maximum = Vector3<float32>(node->_Maximum._X, FLOAT_MAXIMUM, node->_Maximum._Y);
		render_information._WorldPosition = Vector2<float32>(world_position._X, world_position._Z);
		render_information._PatchSize = _Properties._PatchSize * patch_size_multiplier;
		render_information._Borders = node->_Borders;
		render_information._HeightMapTextureIndex = node->_HeightMapTextureIndex;
		render_information._HeightMapResolution = node->_HeightMapResolution;
		render_information._MaterialMapsResolution = node->_MaterialMapsResolution;
		render_information._NormalMapTextureIndex = node->_NormalMapTextureIndex;
		render_information._IndexMapTextureIndex = node->_IndexMapTextureIndex;
		render_information._BlendMapTextureIndex = node->_BlendMapTextureIndex;
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

/*
*	Generates the maps for the given node.
*/
void TerrainSystem::GenerateMaps(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	//Define constants.
	constexpr uint32 MAXIMUM_MATERIAL_MAPS_RESOLUTION{ 1'024 };

	//Initialize the command buffer, if needed.
	if (!_TerrainGenerationRunning
		&& (_TerrainHeightGenerationComputePipeline.IsInitialized() || _TerrainMaterialsGenerationComputePipeline.IsInitialized()))
	{
		//Begin the command buffer.
		_CommandBuffer->Begin(&_TerrainHeightGenerationComputePipeline);

#if TERRAIN_SYSTEM_TIMESTAMP_GPU_GENERATION
		//Write the timestamp.
		_CommandBuffer->WriteTimestamp(nullptr, _QueryPool, 0);
#endif

		_TerrainGenerationRunning = true;
	}

	//Calculate the patch size.
	const float32 patch_size{ _Properties._PatchSize * TerrainQuadTreeUtilities::PatchSizeMultiplier(*node) };

	//Calculate the height/material map(s) resolution.
	node->_HeightMapResolution = _Properties._PatchResolution;
	node->_MaterialMapsResolution = CatalystBaseMath::Clamp<uint32>(CatalystBaseMath::Round<uint32>(patch_size), 1, MAXIMUM_MATERIAL_MAPS_RESOLUTION);

	//Initialize the height/index/blend maps.
	Texture2D<float32> height_map{ node->_HeightMapResolution };
	Texture2D<Vector4<uint8>> normal_map{ node->_MaterialMapsResolution };
	Texture2D<Vector4<uint8>> index_map{ node->_MaterialMapsResolution };
	Texture2D<Vector4<uint8>> blend_map{ node->_MaterialMapsResolution };

	//Generate the height map.
	if (_TerrainHeightGenerationComputePipeline.IsInitialized())
	{
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(height_map), TextureFormat::R_FLOAT32, TextureUsage::STORAGE), &node->_HeightMapTexture);
		node->_HeightMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(node->_HeightMapTexture);

		_TerrainHeightGenerationComputePipeline.Execute(node->_Minimum,
														node->_Maximum,
														node->_HeightMapResolution,
														node->_HeightMapTexture,
														_CommandBuffer);
	}

	else
	{
		for (uint32 Y{ 0 }; Y < node->_HeightMapResolution; ++Y)
		{
			for (uint32 X{ 0 }; X < node->_HeightMapResolution; ++X)
			{
				//Calculate the normalized coordinate.
				const Vector2<float32> normalized_coordinate{ static_cast<float32>(X) / static_cast<float32>(node->_HeightMapResolution - 1), static_cast<float32>(Y) / static_cast<float32>(node->_HeightMapResolution - 1) };

				//Calculate the world position.
				const WorldPosition world_position{ Vector3<float32>(	CatalystBaseMath::LinearlyInterpolate(node->_Minimum._X, node->_Maximum._X, normalized_coordinate._X),
																		0.0f,
																		CatalystBaseMath::LinearlyInterpolate(node->_Minimum._Y, node->_Maximum._Y, normalized_coordinate._Y)
																		) };

				//Generate the height.
				height_map.At(X, Y) = _Properties._TerrainHeightFunction(world_position);
			}
		}

		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(height_map), TextureFormat::R_FLOAT32, TextureUsage::STORAGE), &node->_HeightMapTexture);
		
		node->_HeightMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(node->_HeightMapTexture);
	}

	if (_TerrainMaterialsGenerationComputePipeline.IsInitialized())
	{
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(normal_map), TextureFormat::RGBA_UINT8, TextureUsage::STORAGE), &node->_NormalMapTexture);
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(index_map), TextureFormat::RGBA_UINT8, TextureUsage::STORAGE), &node->_IndexMapTexture);
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(blend_map), TextureFormat::RGBA_UINT8, TextureUsage::STORAGE), &node->_BlendMapTexture);

		node->_NormalMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(node->_NormalMapTexture);
		node->_IndexMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(node->_IndexMapTexture);
		node->_BlendMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(node->_BlendMapTexture);

		_TerrainMaterialsGenerationComputePipeline.Execute(	node->_Minimum,
															node->_Maximum,
															node->_MaterialMapsResolution,
															node->_NormalMapTexture,
															node->_IndexMapTexture,
															node->_BlendMapTexture,
															_CommandBuffer);
	}

	else
	{
		//Generate the material maps.
		for (uint32 Y{ 0 }; Y < node->_MaterialMapsResolution; ++Y)
		{
			for (uint32 X{ 0 }; X < node->_MaterialMapsResolution; ++X)
			{
				//Calculate the normalized coordinate.
				const Vector2<float32> normalized_coordinate{ static_cast<float32>(X) / static_cast<float32>(node->_MaterialMapsResolution - 1), static_cast<float32>(Y) / static_cast<float32>(node->_MaterialMapsResolution - 1) };

				//Calculate the world position.
				const WorldPosition world_position{ Vector3<float32>(	CatalystBaseMath::LinearlyInterpolate(node->_Minimum._X, node->_Maximum._X, normalized_coordinate._X),
																		0.0f,
																		CatalystBaseMath::LinearlyInterpolate(node->_Minimum._Y, node->_Maximum._Y, normalized_coordinate._Y)
																		) };

				//Generate the normal.
				Vector3<float32> terrain_normal;
				GetTerrainNormalAtPosition(world_position.GetAbsolutePosition(), &terrain_normal);

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					normal_map.At(X, Y)[i] = static_cast<uint8>((terrain_normal[i] * 0.5f + 0.5f) * static_cast<float32>(UINT8_MAXIMUM));
				}

				//Generate the materials.
				_Properties._TerrainMaterialFunction(world_position, &index_map.At(X, Y), &blend_map.At(X, Y));
			}
		}

		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(normal_map), TextureFormat::RGBA_UINT8, TextureUsage::STORAGE), &node->_NormalMapTexture);
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(index_map), TextureFormat::RGBA_UINT8, TextureUsage::STORAGE), &node->_IndexMapTexture);
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(blend_map), TextureFormat::RGBA_UINT8, TextureUsage::STORAGE), &node->_BlendMapTexture);

		node->_NormalMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(node->_NormalMapTexture);
		node->_IndexMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(node->_IndexMapTexture);
		node->_BlendMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(node->_BlendMapTexture);
	}

	FinishTerrainGeneration();
}

/*
*	Destroys the maps for the given node.
*/
void TerrainSystem::DestroyMaps(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	if (node->_HeightMapTexture)
	{
		_Update._TexturesToRemove.Emplace(node->_HeightMapTexture, node->_HeightMapTextureIndex);

		node->_HeightMapTexture = EMPTY_HANDLE;
	}

	if (node->_NormalMapTexture)
	{
		_Update._TexturesToRemove.Emplace(node->_NormalMapTexture, node->_NormalMapTextureIndex);

		node->_NormalMapTexture = EMPTY_HANDLE;
	}
	
	if (node->_IndexMapTexture)
	{
		_Update._TexturesToRemove.Emplace(node->_IndexMapTexture, node->_IndexMapTextureIndex);

		node->_IndexMapTexture = EMPTY_HANDLE;
	}

	if (node->_BlendMapTexture)
	{
		_Update._TexturesToRemove.Emplace(node->_BlendMapTexture, node->_BlendMapTextureIndex);

		node->_BlendMapTexture = EMPTY_HANDLE;
	}
}

/*
*	Finishes terrain generation.
*/
void TerrainSystem::FinishTerrainGeneration() NOEXCEPT
{
	if (_TerrainGenerationRunning)
	{
		//Set the event.
		_CommandBuffer->SetEvent(nullptr, _TerrainGenerationEvent);

#if TERRAIN_SYSTEM_TIMESTAMP_GPU_GENERATION
		//Write the timestamp.
		_CommandBuffer->WriteTimestamp(nullptr, _QueryPool, 1);
#endif

		//End the command buffer.
		_CommandBuffer->End(nullptr);

		//Submit the command buffer.
		RenderingSystem::Instance->SubmitCommandBuffer(_CommandBuffer);

		//Wait for the event.
		RenderingSystem::Instance->WaitForEvent(_TerrainGenerationEvent);

#if TERRAIN_SYSTEM_TIMESTAMP_GPU_GENERATION
		//Retrieve the execution time.
		{
			static uint64 total_execution_time{ 0 };
			static uint64 total_executions{ 0 };

			total_execution_time += static_cast<uint64>(RenderingSystem::Instance->GetExecutionTime(_QueryPool));
			++total_executions;

			const uint64 average_execution_time{ total_execution_time / total_executions };

			PRINT_TO_OUTPUT("Average terrain GPU generation time: " << static_cast<float64>(average_execution_time) / 1'000'000.0 << " milliseconds.");
		}
#endif

		//Reset the event.
		RenderingSystem::Instance->ResetEvent(_TerrainGenerationEvent);

		//Destroy the render data tables.
		_TerrainHeightGenerationComputePipeline.DestroyRenderDataTables();
		_TerrainMaterialsGenerationComputePipeline.DestroyRenderDataTables();

		//Terrain generation is no longer running.
		_TerrainGenerationRunning = false;
	}
}