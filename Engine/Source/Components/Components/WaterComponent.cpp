//Header file.
#include <Components/Components/WaterComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/Culling.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

//Terrain.
#include <Terrain/TerrainGeneralUtilities.h>
#include <Terrain/TerrainVertex.h>
#include <Terrain/TerrainQuadTreeUtilities.h>

/*
*	Checks combination of a node.
*/
void CheckCombination(WaterInstanceData &instance_data, const Vector3<float32> &camera_position, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	//If this node is already subdivided, check all of it's child nodes.
	if (node->IsSubdivided())
	{
		if (TerrainQuadTreeUtilities::ShouldBeCombined(instance_data._PatchSize, instance_data._MaximumSubdivisionSteps, *node, camera_position))
		{
			node->_ChildNodes.Clear();
		}

		else
		{
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				CheckCombination(instance_data, camera_position, &node->_ChildNodes[i]);
			}
		}
	}
}

/*
*	Checks subdivisions of a node.
*/
void CheckSubdivision(WaterInstanceData &instance_data, const Vector3<float32> &camera_position, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	//If this node is already subdivided, check all of it's child nodes.
	if (node->IsSubdivided())
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			CheckSubdivision(instance_data, camera_position, &node->_ChildNodes[i]);
		}
	}

	//Else, check if this node should be subdivided.
	else
	{
		if (TerrainQuadTreeUtilities::ShouldBeSubdivided(instance_data._PatchSize, instance_data._MaximumSubdivisionSteps, *node, camera_position))
		{
			//Calculate the patch size multiplier.
			const float32 patch_size_multiplier{ TerrainQuadTreeUtilities::PatchSizeMultiplier(*node) * 0.5f };

			//Set up the new nodes.
			node->_ChildNodes.Upsize<true>(4);

			for (uint64 current_node_index{ 0 }; current_node_index < 4; ++current_node_index)
			{
				TerrainQuadTreeNode& child_node{ node->_ChildNodes[current_node_index] };

				child_node._Depth = node->_Depth + 1;
				child_node._Borders = 0;

				switch (current_node_index)
				{
					case 0:
					{
						child_node._Minimum = child_node._Maximum = Vector2<float32>
						(
							node->_Minimum._X + static_cast<float32>(instance_data._PatchSize) * patch_size_multiplier * 1.5f,
							node->_Minimum._Y + static_cast<float32>(instance_data._PatchSize) * patch_size_multiplier * 0.5f
						);

						break;
					}

					case 1:
					{
						child_node._Minimum = child_node._Maximum = Vector2<float32>
						(
							node->_Minimum._X + static_cast<float32>(instance_data._PatchSize) * patch_size_multiplier * 1.5f,
							node->_Minimum._Y + static_cast<float32>(instance_data._PatchSize) * patch_size_multiplier * 1.5f
						);

						break;
					}

					case 2:
					{
						child_node._Minimum = child_node._Maximum = Vector2<float32>
						(
							node->_Minimum._X + static_cast<float32>(instance_data._PatchSize) * patch_size_multiplier * 0.5f,
							node->_Minimum._Y + static_cast<float32>(instance_data._PatchSize) * patch_size_multiplier * 1.5f
						);

						break;
					}

					case 3:
					{
						child_node._Minimum = child_node._Maximum = Vector2<float32>
						(
							node->_Minimum._X + static_cast<float32>(instance_data._PatchSize) * patch_size_multiplier * 0.5f,
							node->_Minimum._Y + static_cast<float32>(instance_data._PatchSize) * patch_size_multiplier * 0.5f
						);

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}

				child_node._Minimum -= Vector2<float32>(static_cast<float32>(instance_data._PatchSize) * patch_size_multiplier * 0.5f);
				child_node._Maximum += Vector2<float32>(static_cast<float32>(instance_data._PatchSize) * patch_size_multiplier * 0.5f);

				/*
				*	Use the parent node's min/max height, as it would be too time-consuming to compute for each individual node.
				*	Should be a good enough approximation.
				*/
				child_node._AxisAlignedBoundingBox._Minimum = Vector3<float32>(child_node._Minimum._X, node->_AxisAlignedBoundingBox._Minimum._Y, child_node._Minimum._Y);
				child_node._AxisAlignedBoundingBox._Maximum = Vector3<float32>(child_node._Maximum._X, node->_AxisAlignedBoundingBox._Maximum._Y, child_node._Maximum._Y);

				child_node._Position = TerrainQuadTreeUtilities::MiddlePoint(child_node);
				const Vector2<float32> heightmap_coordinate_offset{ static_cast<float32>(instance_data._PatchSize) * 0.5f };
				child_node._MinimumHeightMapCoordinate = (child_node._Minimum + heightmap_coordinate_offset) / static_cast<float32>(instance_data._PatchSize);
				child_node._MaximumHeightMapCoordinate = (child_node._Maximum + heightmap_coordinate_offset) / static_cast<float32>(instance_data._PatchSize);

				child_node._MinimumHeightMapCoordinate *= 1.0f - (1.0f / static_cast<float32>(instance_data._PatchSize));
				child_node._MaximumHeightMapCoordinate *= 1.0f - (1.0f / static_cast<float32>(instance_data._PatchSize));

				child_node._PatchSize = static_cast<float32>(instance_data._PatchSize) * patch_size_multiplier;
			}
		}
	}
}

/*
*	Calculates borders for the given node.
*/
void CalculateBorders(WaterInstanceData& instance_data, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	//If this node is subdivided, calculate new borders for it's child nodes.
	if (node->IsSubdivided())
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			CalculateBorders(instance_data, &node->_ChildNodes[i]);
		}
	}

	else
	{
		//Retrieve the neighboring nodes.
		StaticArray<const TerrainQuadTreeNode *RESTRICT, 4> neighboring_nodes;
		TerrainQuadTreeUtilities::NeighboringNodes(instance_data._PatchSize, instance_data._QuadTree._RootNode, *node, &neighboring_nodes);

		//Calculate new borders.
		uint32 borders{ 0 };

		//Left.
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

		//Right.
		{
			const int32 delta{ neighboring_nodes[2] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboring_nodes[2]->_Depth) : 0 };

			if (delta >= 1)
			{
				borders |= BIT(2);
			}

			if (delta >= 2)
			{
				borders |= BIT(3);
			}
		}

		//Down.
		{
			const int32 delta{ neighboring_nodes[1] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboring_nodes[1]->_Depth) : 0 };

			if (delta >= 1)
			{
				borders |= BIT(4);
			}

			if (delta >= 2)
			{
				borders |= BIT(5);
			}
		}

		//Up.
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
*	Culls a single water quad tree node.
*/
void CullWaterQuadTreeNode
(
	const Vector3<float32> &world_grid_delta,
	const Frustum *const RESTRICT frustum,
	TerrainQuadTreeNode *const RESTRICT node
) NOEXCEPT
{
	//If this node is subdivided, cull each child node.
	if (node->IsSubdivided())
	{
		for (TerrainQuadTreeNode &child_node : node->_ChildNodes)
		{
			CullWaterQuadTreeNode(world_grid_delta, frustum, &child_node);
		}
	}

	else
	{
		//Transform the axis aligned bounding box into the camera's cell.
		AxisAlignedBoundingBox3D transformed_axis_aligned_bounding_box
		{
			node->_AxisAlignedBoundingBox._Minimum + world_grid_delta,
			node->_AxisAlignedBoundingBox._Maximum + world_grid_delta
		};

		//Cull!
		node->_Visible = Culling::IsWithinFrustum(transformed_axis_aligned_bounding_box, *frustum);
	}
}

/*
*	Updates this component.
*/
void WaterComponent::ParallelBatchUpdate(const UpdatePhase update_phase, const uint64 start_instance_index, const uint64 end_instance_index) NOEXCEPT
{
	PROFILING_SCOPE("WaterComponent::ParallelBatchUpdate");

	switch (update_phase)
	{
		case UpdatePhase::PRE_RENDER:
		{
			//Cache data that will be used.
			const WorldPosition camera_world_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetWorldPosition() };
			const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
			const Frustum *const RESTRICT frustum{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustum() };

			//Iterate over the instances.
			for (uint64 instance_index{ start_instance_index }; instance_index < end_instance_index; ++instance_index)
			{
				//Cache the instance data.
				WaterInstanceData &instance_data{ _InstanceData[instance_index] };

				//Process the quad tree.
				{
					//Calculate the camera relative position.
					const Vector3<float32> camera_relative_position{ camera_world_position.GetRelativePosition(instance_data._WorldPosition.GetCell()) };

					//Check combination.
					CheckCombination(instance_data, camera_relative_position, &instance_data._QuadTree._RootNode);

					//Check subdivision.
					CheckSubdivision(instance_data, camera_relative_position, &instance_data._QuadTree._RootNode);

					//Calculate the borders.
					CalculateBorders(instance_data, &instance_data._QuadTree._RootNode);
				}

				//Do culling.
				{
					//Cull the instance.
					instance_data._Visibility = Culling::IsWithinFrustum(instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell), *frustum);

					//If this instance is visible, walk through the quad tree and determine the visibility of each node.
					if (instance_data._Visibility)
					{
						//Calculate the cell delta.
						const Vector3<int32> delta{ instance_data._WorldPosition.GetCell() - WorldSystem::Instance->GetCurrentWorldGridCell() };
						Vector3<float32> world_grid_delta;

						for (uint8 i{ 0 }; i < 3; ++i)
						{
							world_grid_delta[i] = static_cast<float32>(delta[i]) * WorldSystem::Instance->GetWorldGridSize();
						}

						//Start at the root node.
						CullWaterQuadTreeNode(world_grid_delta, frustum, &instance_data._QuadTree._RootNode);
					}
				}
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
*	Preprocessed initialization data an instance.
*/
void WaterComponent::PreProcess(WaterInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Calculate the world space axis aligned bounding box.
	AxisAlignedBoundingBox3D axis_aligned_bounding_box;

	axis_aligned_bounding_box._Minimum._X = -(static_cast<float32>(initialization_data->_PatchSize) * 0.5f);
	axis_aligned_bounding_box._Minimum._Y = initialization_data->_WorldPosition.GetLocalPosition()._Y;
	axis_aligned_bounding_box._Minimum._Z = -(static_cast<float32>(initialization_data->_PatchSize) * 0.5f);

	axis_aligned_bounding_box._Maximum._X = (static_cast<float32>(initialization_data->_PatchSize) * 0.5f);
	axis_aligned_bounding_box._Maximum._Y = initialization_data->_WorldPosition.GetLocalPosition()._Y;
	axis_aligned_bounding_box._Maximum._Z = (static_cast<float32>(initialization_data->_PatchSize) * 0.5f);

	initialization_data->_PreprocessedData._WorldSpaceAxisAlignedBoundingBox._Minimum = WorldPosition(initialization_data->_WorldPosition.GetCell(), axis_aligned_bounding_box._Minimum);
	initialization_data->_PreprocessedData._WorldSpaceAxisAlignedBoundingBox._Maximum = WorldPosition(initialization_data->_WorldPosition.GetCell(), axis_aligned_bounding_box._Maximum);

	//Construct the plane.
	DynamicArray<TerrainVertex> vertices;
	DynamicArray<uint32> indices;

	TerrainGeneralUtilities::GenerateTerrainPlane
	(
		initialization_data->_BaseResolution,
		&vertices,
		&indices
	);

	StaticArray<void* RESTRICT, 2> buffer_data;

	buffer_data[0] = vertices.Data();
	buffer_data[1] = indices.Data();

	StaticArray<uint64, 2> buffer_data_sizes;

	buffer_data_sizes[0] = sizeof(TerrainVertex) * vertices.Size();
	buffer_data_sizes[1] = sizeof(uint32) * indices.Size();

	RenderingSystem::Instance->CreateBuffer(buffer_data_sizes[0] + buffer_data_sizes[1], BufferUsage::IndexBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &initialization_data->_PreprocessedData._Buffer);
	RenderingSystem::Instance->UploadDataToBuffer(buffer_data.Data(), buffer_data_sizes.Data(), 2, &initialization_data->_PreprocessedData._Buffer);

	initialization_data->_PreprocessedData._IndexOffset = static_cast<uint32>(buffer_data_sizes[0]);
	initialization_data->_PreprocessedData._IndexCount = static_cast<uint32>(indices.Size());
}

/*
*	Creates an instance.
*/
void WaterComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	WaterInitializationData *const RESTRICT _initialization_data{ static_cast<WaterInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	WaterInstanceData &instance_data{ _InstanceData.Back() };

	instance_data._WorldPosition = _initialization_data->_WorldPosition;
	instance_data._WorldSpaceAxisAlignedBoundingBox = _initialization_data->_PreprocessedData._WorldSpaceAxisAlignedBoundingBox;
	instance_data._PatchSize = _initialization_data->_PatchSize;
	instance_data._BaseResolution = _initialization_data->_BaseResolution;
	instance_data._MaximumSubdivisionSteps = _initialization_data->_MaximumSubdivisionSteps;

	instance_data._Buffer = _initialization_data->_PreprocessedData._Buffer;
	instance_data._IndexOffset = _initialization_data->_PreprocessedData._IndexOffset;
	instance_data._IndexCount = _initialization_data->_PreprocessedData._IndexCount;
	instance_data._Texture = _initialization_data->_Texture;

	instance_data._QuadTree._RootNode._Depth = 0;
	instance_data._QuadTree._RootNode._Borders = 0;
	instance_data._QuadTree._RootNode._Minimum = instance_data._QuadTree._RootNode._Maximum = Vector2<float32>(instance_data._WorldPosition.GetLocalPosition()._X, instance_data._WorldPosition.GetLocalPosition()._Z);
	instance_data._QuadTree._RootNode._Minimum -= Vector2<float32>(static_cast<float32>(instance_data._PatchSize) * 0.5f);
	instance_data._QuadTree._RootNode._Maximum = Vector2<float32>(static_cast<float32>(instance_data._PatchSize) * 0.5f);
	instance_data._QuadTree._RootNode._AxisAlignedBoundingBox = instance_data._WorldSpaceAxisAlignedBoundingBox.GetLocalAxisAlignedBoundingBox();
	instance_data._QuadTree._RootNode._Position = Vector2<float32>(instance_data._WorldPosition.GetLocalPosition()._X, instance_data._WorldPosition.GetLocalPosition()._Z);
	instance_data._QuadTree._RootNode._MinimumHeightMapCoordinate = Vector2<float32>(0.0f);
	instance_data._QuadTree._RootNode._MaximumHeightMapCoordinate = Vector2<float32>(1.0f);
	instance_data._QuadTree._RootNode._PatchSize = static_cast<float32>(instance_data._PatchSize);
}

/*
*	Destroys an instance.
*/
void WaterComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Cache the instance index.
	const uint64 instance_index{ EntityToInstance(entity) };

	//Cache the instance data.
	WaterInstanceData &instance_data{ _InstanceData[instance_index] };

	//Destroy stuff.
	RenderingSystem::Instance->DestroyBuffer(&instance_data._Buffer);

	//Remove the instance.
	RemoveInstance(entity);
}