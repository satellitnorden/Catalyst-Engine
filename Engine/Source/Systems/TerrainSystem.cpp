//Header file.
#include <Systems/TerrainSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/CatalystProjectConfiguration.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Core/BinaryFile.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/MemorySystem.h>
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
	_Properties._MaximumMaterialMapsResolution = configuration._MaximumMaterialMapsResolution;
	_Properties._MaximumQuadTreeDepth = configuration._MaximumQuadTreeDepth;

	if (configuration._TerrainMaximumHeightFunction)
	{
		_Properties._TerrainMaximumHeight = configuration._TerrainMaximumHeightFunction();
	}

	else
	{
		_Properties._TerrainMaximumHeight = FLOAT32_MAXIMUM;
	}

	_Properties._TerrainHeightFunction = configuration._TerrainHeightFunction;
	_Properties._TerrainMaterialFunction = configuration._TerrainMaterialFunction;
	_Properties._TerrainDataSaveFolder = configuration._TerrainDataSaveFolder;

	//Generate the terrain plane.
	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainGeneralUtilities::GenerateTerrainPlane(	_Properties._PatchResolution,
													_Properties._PatchResolution,
													0.0f,
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

	//Determine the maximum number of updates in flight.
	_MaximumNumberOfUpdatesInFlight = TaskSystem::Instance->GetNumberOfTaskExecutors() / 2;

	//Set up the process updates task.
	_ProcessUpdatesTask._Function = [](void* const RESTRICT)
	{
		TerrainSystem::Instance->ProcessUpdatesAsynchronously();
	};
	_ProcessUpdatesTask._Arguments = nullptr;
	_ProcessUpdatesTask._ExecutableOnSameThread = false;
}

/*
*	Post-initializes the terrain system.
*/
void TerrainSystem::PostInitialize(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT
{
	
}

/*
*	Updates the terrain system during the sequential update phase.
*/
void TerrainSystem::SequentialUpdate() NOEXCEPT
{
	//Return if terrain cannot be generated.
	if (!CanTerrainBeGenerated())
	{
		return;
	}

	//Update the current stage.
	switch (_CurrentUpdateStage)
	{
		case UpdateStage::GENERATE_UPDATES:
		{
			UpdateGenerateUpdatesStage();

			break;
		}

		case UpdateStage::WAIT_FOR_UPDATES:
		{
			UpdateWaitForUpdatesStage();

			break;
		}

		case UpdateStage::PROCESS_UPDATES:
		{
			UpdateProcessUpdatesStage();

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}

/*
*	Returns if terrain can be generated.
*/
NO_DISCARD bool TerrainSystem::CanTerrainBeGenerated() const NOEXCEPT
{
	//Need the functions.
	return _Properties._TerrainMaximumHeight != FLOAT32_MAXIMUM && _Properties._TerrainHeightFunction && _Properties._TerrainMaterialFunction;
}

/*
*	Sets the maximum quad tree depth.
*/
void TerrainSystem::SetMaximumQuadTreeDepth(const uint8 value) NOEXCEPT
{
	_Properties._MaximumQuadTreeDepth = value;
}

/*
*	Returns the maximum height.
*/
NO_DISCARD bool TerrainSystem::GetMaximumHeight(float32 *const RESTRICT maximum_height) const NOEXCEPT
{
	if (_Properties._TerrainMaximumHeight != FLOAT32_MAXIMUM)
	{
		if (maximum_height)
		{
			*maximum_height = _Properties._TerrainMaximumHeight;
		}

		return true;
	}
	
	else
	{
		if (maximum_height)
		{
			*maximum_height = 0.0f;
		}

		return false;
	}
}

/*
*	Returns the terrain height at the given position.
*/
bool TerrainSystem::GetTerrainHeightAtPosition(const Vector3<float32> &position, float32 *const RESTRICT height, const void *const RESTRICT context) const NOEXCEPT
{
	if (_Properties._TerrainHeightFunction)
	{
		if (height)
		{
			*height = _Properties._TerrainHeightFunction(WorldPosition(position));
		}

		return true;
	}

	else
	{
		if (height)
		{
			*height = 0.0f;
		}

		return false;
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
*	Updates the GENERATE_UPDATES stage.
*/
void TerrainSystem::UpdateGenerateUpdatesStage() NOEXCEPT
{
	//Cache the current camera position.
	const Vector3<float32> current_camera_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetAbsolutePosition() };

	//Calculate the camera grid point.
	const GridPoint2 current_grid_point{ GridPoint2::WorldPositionToGridPoint(current_camera_position, _Properties._PatchSize) };

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
			RemoveNode(&_QuadTree._RootNodes[i]);
			_QuadTree._RootGridPoints[i] = GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM);
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
			if (_CurrentNumberOfUpdatesInFlight < _MaximumNumberOfUpdatesInFlight)
			{
				//Create the new update.
				TerrainQuadTreeNodeUpdate *const RESTRICT new_update{ new (MemorySystem::Instance->TypeAllocate<TerrainQuadTreeNodeUpdate>()) TerrainQuadTreeNodeUpdate(TerrainQuadTreeNodeUpdate::Type::ADD_ROOT_NODE) };

				new_update->_Task._Function = [](void *const RESTRICT arguments)
				{
					TerrainSystem::Instance->PerformUpdate(static_cast<TerrainQuadTreeNodeUpdate *const RESTRICT>(arguments));
				};
				new_update->_Task._Arguments = new_update;
				new_update->_Task._ExecutableOnSameThread = false;
				new_update->_AddRootNodeData._GridPoint = valid_grid_point;

				_Updates.Emplace(new_update);

				TaskSystem::Instance->ExecuteTask(&new_update->_Task);

				++_CurrentNumberOfUpdatesInFlight;
			}
		}
	}

	//Check if a node should be combined.
	for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
	{
		if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
		{
			continue;
		}

		CheckCombination(current_camera_position, &_QuadTree._RootNodes[i]);
	}

	//Check if a node should be subdivided.
	for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
	{
		if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
		{
			continue;
		}

		CheckSubdivision(current_camera_position, &_QuadTree._RootNodes[i]);
	}

	//Update the current update stage.
	_CurrentUpdateStage = UpdateStage::WAIT_FOR_UPDATES;
}

/*
*	Updates the WAIT_FOR_UPDATES stage.
*/
void TerrainSystem::UpdateWaitForUpdatesStage() NOEXCEPT
{
	//Check if all updates are done. Return if not.
	for (TerrainQuadTreeNodeUpdate *const RESTRICT update : _Updates)
	{
		if (!update->_Task.IsExecuted())
		{
			return;
		}
	}

	//Fire off the process updates task.
	TaskSystem::Instance->ExecuteTask(&_ProcessUpdatesTask);

	//Update the current update stage.
	_CurrentUpdateStage = UpdateStage::PROCESS_UPDATES;
}

/*
*	Updates the PROCESS_UPDATES stage.
*/
void TerrainSystem::UpdateProcessUpdatesStage() NOEXCEPT
{
	//Wait for the process updates task to finish.
	if (!_ProcessUpdatesTask.IsExecuted())
	{
		return;
	}

	//Move the patch informations.
	_PatchInformations = std::move(_ProcessedUpdate._PatchInformations);
	_PatchRenderInformations = std::move(_ProcessedUpdate._PatchRenderInformations);

	//Destroy the maps for the nodes.
	for (TerrainQuadTreeNode &node : _ProcessedUpdate._NodesToDestroy)
	{
		DestroyMaps(&node);
	}

	_ProcessedUpdate._NodesToDestroy.Clear();

	//Reset the current number of updates in flight.
	_CurrentNumberOfUpdatesInFlight = 0;

	//Update the current update stage.
	_CurrentUpdateStage = UpdateStage::GENERATE_UPDATES;
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

	//Erase the patch informations.
	_PatchInformations.Erase<false>(node->_TerrainPatchInformation);
	_PatchRenderInformations.Erase<false>(node->_TerrainPatchRenderInformation);
}

/*
*	Checks combination of a node.
*/
void TerrainSystem::CheckCombination(const Vector3<float32> &camera_position, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	//If this node is already subdivided, check all of it's child nodes.
	if (node->IsSubdivided())
	{
		if (TerrainQuadTreeUtilities::ShouldBeCombined(_Properties, *node, camera_position))
		{
			if (_CurrentNumberOfUpdatesInFlight < _MaximumNumberOfUpdatesInFlight)
			{
				//Create the new update.
				TerrainQuadTreeNodeUpdate *const RESTRICT new_update{ new (MemorySystem::Instance->TypeAllocate<TerrainQuadTreeNodeUpdate>()) TerrainQuadTreeNodeUpdate(TerrainQuadTreeNodeUpdate::Type::COMBINE) };

				new_update->_Task._Function = [](void *const RESTRICT arguments)
				{
					TerrainSystem::Instance->PerformUpdate(static_cast<TerrainQuadTreeNodeUpdate *const RESTRICT>(arguments));
				};
				new_update->_Task._Arguments = new_update;
				new_update->_Task._ExecutableOnSameThread = false;
				new_update->_CombineData._Node = node;

				_Updates.Emplace(new_update);

				TaskSystem::Instance->ExecuteTask(&new_update->_Task);

				++_CurrentNumberOfUpdatesInFlight;
			}
		}

		else
		{
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				CheckCombination(camera_position, &node->_ChildNodes[i]);
			}
		}
	}
}

/*
*	Checks subdivisions of a node.
*/
void TerrainSystem::CheckSubdivision(const Vector3<float32> &camera_position, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	//If this node is already subdivided, check all of it's child nodes.
	if (node->IsSubdivided())
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			CheckSubdivision(camera_position, &node->_ChildNodes[i]);
		}
	}

	//Else, check if this node should be subdivided.
	else
	{
		if (_CurrentNumberOfUpdatesInFlight < _MaximumNumberOfUpdatesInFlight)
		{
			if (TerrainQuadTreeUtilities::ShouldBeSubdivided(_Properties, *node, camera_position))
			{
				//Create the new update.
				TerrainQuadTreeNodeUpdate *const RESTRICT new_update{ new (MemorySystem::Instance->TypeAllocate<TerrainQuadTreeNodeUpdate>()) TerrainQuadTreeNodeUpdate(TerrainQuadTreeNodeUpdate::Type::SUBDIVIDE) };

				new_update->_Task._Function = [](void *const RESTRICT arguments)
				{
					TerrainSystem::Instance->PerformUpdate(static_cast<TerrainQuadTreeNodeUpdate *const RESTRICT>(arguments));
				};
				new_update->_Task._Arguments = new_update;
				new_update->_Task._ExecutableOnSameThread = false;
				new_update->_SubdivideData._ParentNode = node;

				_Updates.Emplace(new_update);

				TaskSystem::Instance->ExecuteTask(&new_update->_Task);

				++_CurrentNumberOfUpdatesInFlight;
			}
		}
	}
}

/*
*	Traverses the quad tree, calculates new borders for all nodes and fills in the update.
*/
void TerrainSystem::CalculateNewBorders() NOEXCEPT
{
	//Iterate over all root nodes and calculate new borders for them.
	for (TerrainQuadTreeNode &root_node : _QuadTree._RootNodes)
	{
		CalculateNewBorders(&root_node);
	}
}

/*
*	Calculates new borders for one node and fills in the update.
*/
void TerrainSystem::CalculateNewBorders(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
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
*	Gathers patch informations.
*/
void TerrainSystem::GatherPatchInformations(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	if (node->IsSubdivided())
	{
		for (TerrainQuadTreeNode &child_node : node->_ChildNodes)
		{
			GatherPatchInformations(&child_node);
		}
	}

	else
	{
		GeneratePatchInformations(node);

		_ProcessedUpdate._PatchInformations.Emplace(node->_TerrainPatchInformation);
		_ProcessedUpdate._PatchRenderInformations.Emplace(node->_TerrainPatchRenderInformation);
	}
}

/*
*	Generates the patch informations.
*/
void TerrainSystem::GeneratePatchInformations(TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	//Calculate the world position.
	const float32 patch_size_multiplier{ TerrainQuadTreeUtilities::PatchSizeMultiplier(*node) };

	const Vector3<float32> world_position{	node->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 0.5f,
											0.0f,
											node->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 0.5f };

	//Generate the informations.
	node->_TerrainPatchInformation._AxisAlignedBoundingBox._Minimum = Vector3<float32>(node->_Minimum._X, -FLOAT_MAXIMUM, node->_Minimum._Y);
	node->_TerrainPatchInformation._AxisAlignedBoundingBox._Maximum = Vector3<float32>(node->_Maximum._X, FLOAT_MAXIMUM, node->_Maximum._Y);
	node->_TerrainPatchRenderInformation._WorldPosition = Vector2<float32>(world_position._X, world_position._Z);
	node->_TerrainPatchRenderInformation._PatchSize = _Properties._PatchSize * patch_size_multiplier;
	node->_TerrainPatchRenderInformation._Borders = node->_Borders;
	node->_TerrainPatchRenderInformation._HeightMapTextureIndex = node->_HeightMapTextureIndex;
	node->_TerrainPatchRenderInformation._HeightMapResolution = node->_HeightMapResolution;
	node->_TerrainPatchRenderInformation._MaterialMapsResolution = node->_MaterialMapsResolution;
	node->_TerrainPatchRenderInformation._NormalMapTextureIndex = node->_NormalMapTextureIndex;
	node->_TerrainPatchRenderInformation._IndexMapTextureIndex = node->_IndexMapTextureIndex;
	node->_TerrainPatchRenderInformation._BlendMapTextureIndex = node->_BlendMapTextureIndex;
	node->_TerrainPatchRenderInformation._Visibility = false;
}

/*
*	Updates the terrain ray tracing data.
*/
void TerrainSystem::UpdateTerrainRayTracingData() NOEXCEPT
{
	/*
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

	TerrainGeneralUtilities::GenerateTerrainPlane(	_Properties._PatchResolution,
													&vertices,
													&indices);

	//Iterate over all terrain patches and add their data.
	DynamicArray<Vertex> master_vertices;
	DynamicArray<uint32> master_indices;

	const TerrainPatchInformation* RESTRICT patch_information{ _PatchInformations.Data() };
	const TerrainPatchRenderInformation* RESTRICT patch_render_information{ _PatchRenderInformations.Data() };

	for (uint64 i{ 0 }, size{ _PatchInformations.Size() }; i < size; ++i, ++patch_information, ++patch_render_information)
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
	*/
}

/*
*	Generates the maps for the given node.
*/
void TerrainSystem::GenerateMaps(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	//Calculate the patch size.
	const float32 patch_size{ _Properties._PatchSize * TerrainQuadTreeUtilities::PatchSizeMultiplier(*node) };

	//Calculate the height/material map(s) resolution.
	node->_HeightMapResolution = _Properties._PatchResolution;
	node->_MaterialMapsResolution = CatalystBaseMath::Clamp<uint32>(CatalystBaseMath::Round<uint32>(patch_size), 1, _Properties._MaximumMaterialMapsResolution);

	//Initialize the height/index/blend maps.
	Texture2D<float32> height_map{ node->_HeightMapResolution };
	Texture2D<Vector4<uint8>> normal_map{ node->_MaterialMapsResolution };
	Texture2D<Vector4<uint8>> index_map{ node->_MaterialMapsResolution };
	Texture2D<Vector4<uint8>> blend_map{ node->_MaterialMapsResolution };

	//Generate the height map.
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

	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(height_map), TextureFormat::R_FLOAT32, TextureUsage::STORAGE, false), &node->_HeightMapTexture);

	node->_HeightMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(node->_HeightMapTexture);

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

	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(normal_map), TextureFormat::RGBA_UINT8, TextureUsage::STORAGE, false), &node->_NormalMapTexture);
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(index_map), TextureFormat::RGBA_UINT8, TextureUsage::STORAGE, false), &node->_IndexMapTexture);
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(blend_map), TextureFormat::RGBA_UINT8, TextureUsage::STORAGE, false), &node->_BlendMapTexture);

	node->_NormalMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(node->_NormalMapTexture);
	node->_IndexMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(node->_IndexMapTexture);
	node->_BlendMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(node->_BlendMapTexture);
}

/*
*	Destroys the maps for the given node.
*/
void TerrainSystem::DestroyMaps(TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	if (node->_HeightMapTexture)
	{
		RenderingSystem::Instance->DestroyTexture2D(&node->_HeightMapTexture);
		RenderingSystem::Instance->ReturnTextureToGlobalRenderData(node->_HeightMapTextureIndex);

		node->_HeightMapTexture = EMPTY_HANDLE;
	}

	else
	{
		ASSERT(false, "This is weird. /:");
	}

	if (node->_NormalMapTexture)
	{
		RenderingSystem::Instance->DestroyTexture2D(&node->_NormalMapTexture);
		RenderingSystem::Instance->ReturnTextureToGlobalRenderData(node->_NormalMapTextureIndex);

		node->_NormalMapTexture = EMPTY_HANDLE;
	}

	else
	{
		ASSERT(false, "This is weird. /:");
	}
	
	if (node->_IndexMapTexture)
	{
		RenderingSystem::Instance->DestroyTexture2D(&node->_IndexMapTexture);
		RenderingSystem::Instance->ReturnTextureToGlobalRenderData(node->_IndexMapTextureIndex);

		node->_IndexMapTexture = EMPTY_HANDLE;
	}

	else
	{
		ASSERT(false, "This is weird. /:");
	}

	if (node->_BlendMapTexture)
	{
		RenderingSystem::Instance->DestroyTexture2D(&node->_BlendMapTexture);
		RenderingSystem::Instance->ReturnTextureToGlobalRenderData(node->_BlendMapTextureIndex);

		node->_BlendMapTexture = EMPTY_HANDLE;
	}

	else
	{
		ASSERT(false, "This is weird. /:");
	}
}

/*
*	Performs the given update.
*/
void TerrainSystem::PerformUpdate(TerrainQuadTreeNodeUpdate *const RESTRICT update) NOEXCEPT
{
	switch (update->_Type)
	{
		case TerrainQuadTreeNodeUpdate::Type::ADD_ROOT_NODE:
		{
			const Vector3<float32> grid_point_world_position{ GridPoint2::GridPointToWorldPosition(update->_AddRootNodeData._GridPoint, _Properties._PatchSize) };

			update->_AddRootNodeData._NewNode._Depth = 0;
			update->_AddRootNodeData._NewNode._Borders = 0;
			update->_AddRootNodeData._NewNode._Minimum = Vector2<float32>(grid_point_world_position._X - (_Properties._PatchSize * 0.5f), grid_point_world_position._Z - (_Properties._PatchSize * 0.5f));
			update->_AddRootNodeData._NewNode._Maximum = Vector2<float32>(grid_point_world_position._X + (_Properties._PatchSize * 0.5f), grid_point_world_position._Z + (_Properties._PatchSize * 0.5f));
			
			GenerateMaps(&update->_AddRootNodeData._NewNode);
			GeneratePatchInformations(&update->_AddRootNodeData._NewNode);

			break;
		}

		case TerrainQuadTreeNodeUpdate::Type::COMBINE:
		{
			//Set up the new node.
			update->_CombineData._NewNode._Depth = update->_CombineData._Node->_Depth;
			update->_CombineData._NewNode._Borders = 0;
			update->_CombineData._NewNode._Minimum = update->_CombineData._Node->_Minimum;
			update->_CombineData._NewNode._Maximum = update->_CombineData._Node->_Maximum;

			//Generate the maps.
			GenerateMaps(&update->_CombineData._NewNode);
			GeneratePatchInformations(&update->_CombineData._NewNode);

			break;
		}

		case TerrainQuadTreeNodeUpdate::Type::SUBDIVIDE:
		{
			//Calculate the patch size multiplier.
			const float32 patch_size_multiplier{ TerrainQuadTreeUtilities::PatchSizeMultiplier(*update->_SubdivideData._ParentNode) * 0.5f };

			//Set up the new nodes.
			for (uint64 current_node_index{ 0 }; current_node_index < 4; ++current_node_index)
			{
				//Set up the new node.
				update->_SubdivideData._NewNodes[current_node_index]._Depth = update->_SubdivideData._ParentNode->_Depth + 1;
				update->_SubdivideData._NewNodes[current_node_index]._Borders = 0;

				switch (current_node_index)
				{
					case 0:
					{
						update->_SubdivideData._NewNodes[current_node_index]._Minimum =
						update->_SubdivideData._NewNodes[current_node_index]._Maximum = Vector2<float32>(	update->_SubdivideData._ParentNode->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 1.5f,
																											update->_SubdivideData._ParentNode->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 0.5f);

						update->_SubdivideData._NewNodes[current_node_index]._Minimum -= Vector2<float32>(_Properties._PatchSize * patch_size_multiplier * 0.5f, _Properties._PatchSize * patch_size_multiplier * 0.5f);
						update->_SubdivideData._NewNodes[current_node_index]._Maximum += Vector2<float32>(_Properties._PatchSize * patch_size_multiplier * 0.5f, _Properties._PatchSize * patch_size_multiplier * 0.5f);

						break;
					}

					case 1:
					{
						update->_SubdivideData._NewNodes[current_node_index]._Minimum =
						update->_SubdivideData._NewNodes[current_node_index]._Maximum = Vector2<float32>(	update->_SubdivideData._ParentNode->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 1.5f,
																											update->_SubdivideData._ParentNode->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 1.5f);

						update->_SubdivideData._NewNodes[current_node_index]._Minimum -= Vector2<float32>(_Properties._PatchSize * patch_size_multiplier * 0.5f, _Properties._PatchSize * patch_size_multiplier * 0.5f);
						update->_SubdivideData._NewNodes[current_node_index]._Maximum += Vector2<float32>(_Properties._PatchSize * patch_size_multiplier * 0.5f, _Properties._PatchSize * patch_size_multiplier * 0.5f);

						break;
					}

					case 2:
					{
						update->_SubdivideData._NewNodes[current_node_index]._Minimum =
						update->_SubdivideData._NewNodes[current_node_index]._Maximum = Vector2<float32>(	update->_SubdivideData._ParentNode->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 0.5f,
																											update->_SubdivideData._ParentNode->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 1.5f);

						update->_SubdivideData._NewNodes[current_node_index]._Minimum -= Vector2<float32>(_Properties._PatchSize * patch_size_multiplier * 0.5f, _Properties._PatchSize * patch_size_multiplier * 0.5f);
						update->_SubdivideData._NewNodes[current_node_index]._Maximum += Vector2<float32>(_Properties._PatchSize * patch_size_multiplier * 0.5f, _Properties._PatchSize * patch_size_multiplier * 0.5f);

						break;
					}

					case 3:
					{
						update->_SubdivideData._NewNodes[current_node_index]._Minimum =
						update->_SubdivideData._NewNodes[current_node_index]._Maximum = Vector2<float32>(	update->_SubdivideData._ParentNode->_Minimum._X + _Properties._PatchSize * patch_size_multiplier * 0.5f,
																											update->_SubdivideData._ParentNode->_Minimum._Y + _Properties._PatchSize * patch_size_multiplier * 0.5f);

						update->_SubdivideData._NewNodes[current_node_index]._Minimum -= Vector2<float32>(_Properties._PatchSize * patch_size_multiplier * 0.5f, _Properties._PatchSize * patch_size_multiplier * 0.5f);
						update->_SubdivideData._NewNodes[current_node_index]._Maximum += Vector2<float32>(_Properties._PatchSize * patch_size_multiplier * 0.5f, _Properties._PatchSize * patch_size_multiplier * 0.5f);

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}

				//Generate the maps.
				GenerateMaps(&update->_SubdivideData._NewNodes[current_node_index]);

				//Generate the patch informations.
				GeneratePatchInformations(&update->_SubdivideData._NewNodes[current_node_index]);
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}

/*
*	Processes updates asynchronously.
*/
void TerrainSystem::ProcessUpdatesAsynchronously() NOEXCEPT
{
	//Process all updates.
	for (TerrainQuadTreeNodeUpdate *const RESTRICT update : _Updates)
	{
		ASSERT(update->_Task.IsExecuted(), "Task hasn't been executed yet... :c");

		switch (update->_Type)
		{
			case TerrainQuadTreeNodeUpdate::Type::ADD_ROOT_NODE:
			{
				for (uint64 i{ 0 }, size{ _QuadTree._RootGridPoints.Size() }; i < size; ++i)
				{
					if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
					{
						_QuadTree._RootGridPoints[i] = update->_AddRootNodeData._GridPoint;
						_QuadTree._RootNodes[i] = std::move(update->_AddRootNodeData._NewNode);

						break;
					}
				}

				break;
			}

			case TerrainQuadTreeNodeUpdate::Type::COMBINE:
			{
				//Destroy the maps for all the child nodes.
				for (TerrainQuadTreeNode &child_node : update->_CombineData._Node->_ChildNodes)
				{
					_ProcessedUpdate._NodesToDestroy.Emplace(child_node);
				}

				//Replace the node.
				*update->_CombineData._Node = std::move(update->_CombineData._NewNode);

				break;
			}

			case TerrainQuadTreeNodeUpdate::Type::SUBDIVIDE:
			{
				//Destroy the maps for the parent node.
				_ProcessedUpdate._NodesToDestroy.Emplace(*update->_SubdivideData._ParentNode);

				//Add the child nodes.
				for (uint8 i{ 0 }; i < 4; ++i)
				{
					update->_SubdivideData._ParentNode->_ChildNodes.Emplace(std::move(update->_SubdivideData._NewNodes[i]));
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		update->~TerrainQuadTreeNodeUpdate();
		MemorySystem::Instance->TypeFree<TerrainQuadTreeNodeUpdate>(update);
	}

	//Clear the updates.
	_Updates.Clear();

	//Calculate the borders.
	CalculateNewBorders();

	//Gather the patch informations.
	for (uint8 i{ 0 }, size{ static_cast<uint8>(_QuadTree._RootGridPoints.Size()) }; i < size; ++i)
	{
		if (_QuadTree._RootGridPoints[i] == GridPoint2(INT32_MAXIMUM, INT32_MAXIMUM))
		{
			continue;
		}

		GatherPatchInformations(&_QuadTree._RootNodes[i]);
	}

	//Update the terrain ray tracing data.
	UpdateTerrainRayTracingData();
}

/*
*	Saves the terrain data.
*/
void TerrainSystem::SaveTerrainData() NOEXCEPT
{
	//Check if the terrain sava data folder is set.
	if (!_Properties._TerrainDataSaveFolder)
	{
		return;
	}

	//Create the directory, if needed.
	File::CreateDirectory(_Properties._TerrainDataSaveFolder.Data());
}

/*
*	Loads the terrain data.
*/
void TerrainSystem::LoadTerrainData() NOEXCEPT
{
	//Check if the terrain sava data folder is set.
	if (!_Properties._TerrainDataSaveFolder)
	{
		return;
	}
}