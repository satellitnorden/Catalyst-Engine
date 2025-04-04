//Header file.
#include <Components/Components/TerrainComponent.h>

//Path tracing.
#include <PathTracing/PathTracingCore.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/Culling.h>

//Systems.
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <Systems/DebugSystem.h>
#endif
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

//Terrain.
#include <Terrain/TerrainGeneralUtilities.h>
#include <Terrain/TerrainVertex.h>
#include <Terrain/TerrainQuadTreeUtilities.h>

#if !defined(CATALYST_CONFIGURATION_FINAL)
//Denotes whether or not terrain wireframe is enabled.
bool TERRAIN_WIREFRAME_ENABLED{ false };

/*
*	Terrain wireframe push constant data.
*/
class TerrainWireframePushConstantData final
{

public:

	Vector3<float32> _Color;
	Padding<4> _Padding;
	Vector2<float32> _WorldPosition;
	Vector2<float32> _MinimumHeightMapCoordinate;
	Vector2<float32> _MaximumHeightMapCoordinate;
	uint32 _Borders;
	float32 _PatchResolutionReciprocal;
	float32 _PatchSize;
	uint32 _HeightMapTextureIndex;
	uint32 _NormalMapTextureIndex;
	uint32 _IndexMapTextureIndex;
	uint32 _BlendMapTextureIndex;
	float32 _MapResolution;
	float32 _MapResolutionReciprocal;

};

/*
*	Gathers a terrain wireframe quad tree node.
*/
FORCE_INLINE void GatherTerrainWireframeQuadTreeNode
(
	const TerrainInstanceData &instance_data,
	const TerrainQuadTreeNode &node,
	RenderInputStream *const RESTRICT input_stream
) NOEXCEPT
{
	if (node.IsSubdivided())
	{
		for (const TerrainQuadTreeNode &child_node : node._ChildNodes)
		{
			GatherTerrainWireframeQuadTreeNode(instance_data, child_node, input_stream);
		}
	}

	else
	{
		//Ignore this node if it not visible.
		if (!node._Visible)
		{
			return;
		}

		//Add a new entry.
		input_stream->_Entries.Emplace();
		RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

		new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
		new_entry._VertexBuffer = instance_data._Buffer;
		new_entry._IndexBuffer = instance_data._Buffer;
		new_entry._IndexBufferOffset = instance_data._IndexOffset;
		new_entry._InstanceBuffer = EMPTY_HANDLE;
		new_entry._VertexCount = 0;
		new_entry._IndexCount = instance_data._IndexCount;
		new_entry._InstanceCount = 0;

		//Set up the push constant data.
		TerrainWireframePushConstantData push_constant_data;

		push_constant_data._Color = Vector3<float32>(1.0f, 1.0f, 1.0f);
		const Vector3<float32> component_world_position{ instance_data._WorldPosition.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell()) };
		push_constant_data._WorldPosition = Vector2<float32>(component_world_position._X, component_world_position._Z) + node._Position;
		push_constant_data._MinimumHeightMapCoordinate = node._MinimumHeightMapCoordinate;
		push_constant_data._MaximumHeightMapCoordinate = node._MaximumHeightMapCoordinate;
		push_constant_data._Borders = node._Borders;
		push_constant_data._PatchResolutionReciprocal = 1.0f / static_cast<float32>(instance_data._BaseResolution);
		push_constant_data._PatchSize = node._PatchSize;
		push_constant_data._HeightMapTextureIndex = instance_data._HeightMapTextureIndex;
		push_constant_data._NormalMapTextureIndex = instance_data._NormalMapTextureIndex;
		push_constant_data._IndexMapTextureIndex = instance_data._IndexMapTextureIndex;
		push_constant_data._BlendMapTextureIndex = instance_data._BlendMapTextureIndex;
		push_constant_data._MapResolution = static_cast<float32>(instance_data._HeightMap.GetResolution());
		push_constant_data._MapResolutionReciprocal = 1.0f / push_constant_data._MapResolution;

		for (uint64 i{ 0 }; i < sizeof(TerrainWireframePushConstantData); ++i)
		{
			input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
		}
	}
}

/*
*	Gathers a terrain wireframe render input stream.
*/
void GatherTerrainWireframeRenderInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	//Gather terrains.
	if (TERRAIN_WIREFRAME_ENABLED)
	{
		for (const TerrainInstanceData &instance_data : TerrainComponent::Instance->InstanceData())
		{
			//Is this terrain visible?
			if (!instance_data._Visibility)
			{
				continue;
			}

			//Walk the quad tree.
			GatherTerrainWireframeQuadTreeNode(instance_data, instance_data._QuadTree._RootNode, input_stream);
		}
	}
}
#endif

/*
*	Checks combination of a node.
*/
void CheckCombination(TerrainInstanceData &instance_data, const Vector3<float32> &camera_position, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
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
void CheckSubdivision(TerrainInstanceData &instance_data, const Vector3<float32> &camera_position, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
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
				TerrainQuadTreeNode &child_node{ node->_ChildNodes[current_node_index] };

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

				const float32 heightmap_coordinate_padding{ 1.0f / static_cast<float32>(instance_data._PatchSize) };

				child_node._MinimumHeightMapCoordinate *= 1.0f - heightmap_coordinate_padding;
				child_node._MaximumHeightMapCoordinate *= 1.0f - heightmap_coordinate_padding;

				child_node._PatchSize = static_cast<float32>(instance_data._PatchSize) * patch_size_multiplier;
			}
		}
	}
}

/*
*	Calculates borders for the given node.
*/
void CalculateBorders(TerrainInstanceData &instance_data, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
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
		/*
		*	TODO: When we can't find a neighboring node, put up a request to find one on other terrain components,
		*	and process them real quick in the post update.
		*	For now, assume that there's a depth difference of 1, should be right most of the time.
		*/

		//Retrieve the neighboring nodes.
		StaticArray<const TerrainQuadTreeNode *RESTRICT, 4> neighboring_nodes;
		TerrainQuadTreeUtilities::NeighboringNodes(instance_data._PatchSize, instance_data._QuadTree._RootNode, *node, &neighboring_nodes);

		//Calculate new borders.
		uint32 borders{ 0 };

		//Left.
		{
			const int32 delta{ neighboring_nodes[0] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboring_nodes[0]->_Depth) : 1 };

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
			const int32 delta{ neighboring_nodes[2] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboring_nodes[2]->_Depth) : 1 };

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
			const int32 delta{ neighboring_nodes[1] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboring_nodes[1]->_Depth) : 1 };

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
			const int32 delta{ neighboring_nodes[3] ? static_cast<int32>(node->_Depth) - static_cast<int32>(neighboring_nodes[3]->_Depth) : 1 };

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
*	Culls a single terrain quad tree node.
*/
void CullTerrainQuadTreeNode
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
			CullTerrainQuadTreeNode(world_grid_delta, frustum, &child_node);
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
*	Initializes this component.
*/
void TerrainComponent::Initialize() NOEXCEPT
{
#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Register the debug command.
	DebugSystem::Instance->RegisterCheckboxDebugCommand
	(
		"Rendering\\Terrain\\Wireframe",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			TERRAIN_WIREFRAME_ENABLED = !TERRAIN_WIREFRAME_ENABLED;
		},
		nullptr
	);

	//Register the input stream.
	{
		DynamicArray<VertexInputAttributeDescription> required_vertex_input_attribute_descriptions;

		required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, static_cast<uint32>(offsetof(TerrainVertex, _Position)));
		required_vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32UnsignedInt, static_cast<uint32>(offsetof(TerrainVertex, _Borders)));

		DynamicArray<VertexInputBindingDescription> required_vertex_input_binding_descriptions;

		required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(TerrainVertex)), VertexInputBindingDescription::InputRate::Vertex);

		RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
		(
			HashString("TerrainWireframe"),
			required_vertex_input_attribute_descriptions,
			required_vertex_input_binding_descriptions,
			sizeof(TerrainWireframePushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				GatherTerrainWireframeRenderInputStream(input_stream);
			},
			RenderInputStream::Mode::DRAW_INDEXED,
			nullptr
		);
	}
#endif
}

/*
*	Updates this component.
*/
void TerrainComponent::ParallelBatchUpdate(const UpdatePhase update_phase, const uint64 start_instance_index, const uint64 end_instance_index) NOEXCEPT
{
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
				TerrainInstanceData &instance_data{ _InstanceData[instance_index] };

				//Process the quad tree.
				{
					//Calculate the camera relative position.
	#if 0 //A good position for the two layers of stitching, useful for debugging. (:
					const WorldPosition fake_camera_world_position{ Vector3<int32>(0, 0, 0), Vector3<float32>(8.0f, 0.0f, 8.0f) };
					const Vector3<float32> camera_relative_position{ fake_camera_world_position.GetRelativePosition(instance_data._WorldPosition.GetCell()) };
	#else
					const Vector3<float32> camera_relative_position{ camera_world_position.GetRelativePosition(instance_data._WorldPosition.GetCell()) };
	#endif

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
						CullTerrainQuadTreeNode(world_grid_delta, frustum, &instance_data._QuadTree._RootNode);
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
void TerrainComponent::PreProcess(TerrainInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Calculate the world space axis aligned bounding box.
	AxisAlignedBoundingBox3D axis_aligned_bounding_box;

	axis_aligned_bounding_box._Minimum._X = -(static_cast<float32>(initialization_data->_PatchSize) * 0.5f);
	axis_aligned_bounding_box._Minimum._Z = -(static_cast<float32>(initialization_data->_PatchSize) * 0.5f);

	axis_aligned_bounding_box._Maximum._X = (static_cast<float32>(initialization_data->_PatchSize) * 0.5f);
	axis_aligned_bounding_box._Maximum._Z = (static_cast<float32>(initialization_data->_PatchSize) * 0.5f);

	for (uint32 Y{ 0 }; Y < initialization_data->_HeightMap.GetResolution(); ++Y)
	{
		for (uint32 X{ 0 }; X < initialization_data->_HeightMap.GetResolution(); ++X)
		{
			const float32 height{ initialization_data->_HeightMap.At(X, Y) };

			axis_aligned_bounding_box._Minimum._Y = BaseMath::Minimum<float32>(axis_aligned_bounding_box._Minimum._Y, height);
			axis_aligned_bounding_box._Maximum._Y = BaseMath::Maximum<float32>(axis_aligned_bounding_box._Maximum._Y, height);
		}
	}

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

	StaticArray<void *RESTRICT, 2> buffer_data;

	buffer_data[0] = vertices.Data();
	buffer_data[1] = indices.Data();

	StaticArray<uint64, 2> buffer_data_sizes;

	buffer_data_sizes[0] = sizeof(TerrainVertex) * vertices.Size();
	buffer_data_sizes[1] = sizeof(uint32) * indices.Size();

	RenderingSystem::Instance->CreateBuffer(buffer_data_sizes[0] + buffer_data_sizes[1], BufferUsage::IndexBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &initialization_data->_PreprocessedData._Buffer);
	RenderingSystem::Instance->UploadDataToBuffer(buffer_data.Data(), buffer_data_sizes.Data(), 2, &initialization_data->_PreprocessedData._Buffer);

	initialization_data->_PreprocessedData._VertexCount = static_cast<uint32>(vertices.Size());
	initialization_data->_PreprocessedData._IndexOffset = static_cast<uint32>(buffer_data_sizes[0]);
	initialization_data->_PreprocessedData._IndexCount = static_cast<uint32>(indices.Size());

	//Create the height map texture.
	{
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(initialization_data->_HeightMap), TextureFormat::R_FLOAT32, TextureUsage::NONE, false), &initialization_data->_PreprocessedData._HeightMapTexture);
		initialization_data->_PreprocessedData._HeightMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(initialization_data->_PreprocessedData._HeightMapTexture);
	}

	//Create the normal map texture.
	{
		initialization_data->_PreprocessedData._ConvertedNormalMapTexture.Initialize(initialization_data->_NormalMap.GetResolution());

		for (uint32 Y{ 0 }; Y < initialization_data->_PreprocessedData._ConvertedNormalMapTexture.GetResolution(); ++Y)
		{
			for (uint32 X{ 0 }; X < initialization_data->_PreprocessedData._ConvertedNormalMapTexture.GetResolution(); ++X)
			{
				const Vector3<float32> &actual_normal{ initialization_data->_NormalMap.At(X, Y) };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					initialization_data->_PreprocessedData._ConvertedNormalMapTexture.At(X, Y)[i] = static_cast<uint8>((actual_normal[i] * 0.5f + 0.5f) * static_cast<float32>(UINT8_MAXIMUM));
				}
			}
		}

		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(initialization_data->_PreprocessedData._ConvertedNormalMapTexture), TextureFormat::RGBA_UINT8, TextureUsage::NONE, false), &initialization_data->_PreprocessedData._NormalMapTexture);
		initialization_data->_PreprocessedData._NormalMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(initialization_data->_PreprocessedData._NormalMapTexture);
	}

	//Create the index map texture.
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(initialization_data->_IndexMap), TextureFormat::RGBA_UINT8, TextureUsage::NONE, false), &initialization_data->_PreprocessedData._IndexMapTexture);
	initialization_data->_PreprocessedData._IndexMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(initialization_data->_PreprocessedData._IndexMapTexture);

	//Create the blend map texture.
	{
		initialization_data->_PreprocessedData._ConvertedBlendMapTexture.Initialize(initialization_data->_BlendMap.GetResolution());

		for (uint32 Y{ 0 }; Y < initialization_data->_PreprocessedData._ConvertedBlendMapTexture.GetResolution(); ++Y)
		{
			for (uint32 X{ 0 }; X < initialization_data->_PreprocessedData._ConvertedBlendMapTexture.GetResolution(); ++X)
			{
				for (uint8 i{ 0 }; i < 4; ++i)
				{
					initialization_data->_PreprocessedData._ConvertedBlendMapTexture.At(X, Y)[i] = static_cast<uint8>(initialization_data->_BlendMap.At(X, Y)[i] * static_cast<float32>(UINT8_MAXIMUM));
				}
			}
		}

		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(initialization_data->_PreprocessedData._ConvertedBlendMapTexture), TextureFormat::RGBA_UINT8, TextureUsage::NONE, false), &initialization_data->_PreprocessedData._BlendMapTexture);
		initialization_data->_PreprocessedData._BlendMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(initialization_data->_PreprocessedData._BlendMapTexture);
	}

	//Create the physics actor.
	PhysicsSystem::Instance->CreateHeightFieldActor
	(
		initialization_data->_WorldPosition,
		initialization_data->_HeightMap,
		false,
		&initialization_data->_PreprocessedData._PhysicsActorHandle
	);
}

/*
*	Creates an instance.
*/
void TerrainComponent::CreateInstance(Entity *const RESTRICT entity, TerrainInitializationData *const RESTRICT initialization_data, TerrainInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	PROFILING_SCOPE("TerrainComponent::CreateInstance");

	//Set up the instance data.
	ASSERT(initialization_data->_HeightMap.GetWidth() == initialization_data->_HeightMap.GetHeight(), "Terrain height map width and height doesn't match - This isn't okay.");

	instance_data->_WorldPosition = initialization_data->_WorldPosition;
	instance_data->_WorldSpaceAxisAlignedBoundingBox = initialization_data->_PreprocessedData._WorldSpaceAxisAlignedBoundingBox;
	instance_data->_PatchSize = initialization_data->_PatchSize;
	instance_data->_HeightMap = std::move(initialization_data->_HeightMap);
	instance_data->_NormalMap = std::move(initialization_data->_PreprocessedData._ConvertedNormalMapTexture);
	instance_data->_IndexMap = std::move(initialization_data->_IndexMap);
	instance_data->_BlendMap = std::move(initialization_data->_PreprocessedData._ConvertedBlendMapTexture);
	instance_data->_BaseResolution = initialization_data->_BaseResolution;
	instance_data->_MaximumSubdivisionSteps = initialization_data->_MaximumSubdivisionSteps;

	instance_data->_Buffer = initialization_data->_PreprocessedData._Buffer;
	instance_data->_VertexCount = initialization_data->_PreprocessedData._VertexCount;
	instance_data->_IndexOffset = initialization_data->_PreprocessedData._IndexOffset;
	instance_data->_IndexCount = initialization_data->_PreprocessedData._IndexCount;
	instance_data->_HeightMapTexture = initialization_data->_PreprocessedData._HeightMapTexture;
	instance_data->_HeightMapTextureIndex = initialization_data->_PreprocessedData._HeightMapTextureIndex;
	instance_data->_NormalMapTexture = initialization_data->_PreprocessedData._NormalMapTexture;
	instance_data->_NormalMapTextureIndex = initialization_data->_PreprocessedData._NormalMapTextureIndex;
	instance_data->_IndexMapTexture = initialization_data->_PreprocessedData._IndexMapTexture;
	instance_data->_IndexMapTextureIndex = initialization_data->_PreprocessedData._IndexMapTextureIndex;
	instance_data->_BlendMapTexture = initialization_data->_PreprocessedData._BlendMapTexture;
	instance_data->_BlendMapTextureIndex = initialization_data->_PreprocessedData._BlendMapTextureIndex;

	instance_data->_QuadTree._RootNode._Depth = 0;
	instance_data->_QuadTree._RootNode._Borders = 0;
	instance_data->_QuadTree._RootNode._Minimum = instance_data->_QuadTree._RootNode._Maximum = Vector2<float32>(instance_data->_WorldPosition.GetLocalPosition()._X, instance_data->_WorldPosition.GetLocalPosition()._Z);
	instance_data->_QuadTree._RootNode._Minimum -= Vector2<float32>(static_cast<float32>(instance_data->_PatchSize) * 0.5f);
	instance_data->_QuadTree._RootNode._Maximum = Vector2<float32>(static_cast<float32>(instance_data->_PatchSize) * 0.5f);
	instance_data->_QuadTree._RootNode._AxisAlignedBoundingBox = instance_data->_WorldSpaceAxisAlignedBoundingBox.GetLocalAxisAlignedBoundingBox();
	instance_data->_QuadTree._RootNode._Position = Vector2<float32>(instance_data->_WorldPosition.GetLocalPosition()._X, instance_data->_WorldPosition.GetLocalPosition()._Z);
	instance_data->_QuadTree._RootNode._MinimumHeightMapCoordinate = Vector2<float32>(0.0f);
	instance_data->_QuadTree._RootNode._MaximumHeightMapCoordinate = Vector2<float32>(1.0f);
	instance_data->_QuadTree._RootNode._PatchSize = static_cast<float32>(instance_data->_PatchSize);

	instance_data->_PhysicsActorHandle = initialization_data->_PreprocessedData._PhysicsActorHandle;

	//Add the actor to the world.
	PhysicsSystem::Instance->AddToWorld(instance_data->_PhysicsActorHandle);
}

/*
*	Destroys an instance.
*/
void TerrainComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Cache the instance index.
	const uint64 instance_index{ EntityToInstance(entity) };

	//Cache the instance data.
	TerrainInstanceData &instance_data{ _InstanceData[instance_index] };

	//Destroy stuff.
	RenderingSystem::Instance->DestroyBuffer(&instance_data._Buffer);
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(instance_data._HeightMapTextureIndex);
	RenderingSystem::Instance->DestroyTexture2D(&instance_data._HeightMapTexture);
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(instance_data._NormalMapTextureIndex);
	RenderingSystem::Instance->DestroyTexture2D(&instance_data._NormalMapTexture);
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(instance_data._IndexMapTextureIndex);
	RenderingSystem::Instance->DestroyTexture2D(&instance_data._IndexMapTexture);
	RenderingSystem::Instance->ReturnTextureToGlobalRenderData(instance_data._BlendMapTextureIndex);
	RenderingSystem::Instance->DestroyTexture2D(&instance_data._BlendMapTexture);

	PhysicsSystem::Instance->DestroyActor(&instance_data._PhysicsActorHandle);

	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Terrain path tracing user data class definition.
*/
class TerrainPathTracingUserData final
{

public:

	//The instance index.
	uint64 _InstanceIndex;

};

/*
*	The terrain path tracing shading function.
*/
FORCE_INLINE void TerrainPathTracingShadingFunction(const PathTracingShadingContext &context, PathTracingShadingResult *const RESTRICT result) NOEXCEPT
{
	/*
	*	Terrain material sample class definition.
	*/
	class TerrainMaterialSample final
	{

	public:

		//The albedo.
		Vector3<float32> _Albedo;

		//The thickness.
		float32 _Thickness;

		//The normal map.
		Vector3<float32> _NormalMap;

		//The displacement.
		float32 _Displacement;

		//The roughness.
		float32 _Roughness;

		//The metallic.
		float32 _Metallic;

		//The ambient occlusion.
		float32 _AmbientOcclusion;

		//The emissive.
		float32 _Emissive;

	};

	//Define constants.
	constexpr float32 MATERIAL_TEXTURE_SCALE{ 0.5f };
	constexpr float32 DISPLACEMENT_POWER{ 8.0f };
	constexpr float32 MINIMUM_SAMPLE_WEIGHT{ FLOAT32_EPSILON * 1.0f };

	constexpr auto ROTATION_FOR_TILE
	{
		[](const Vector2<uint32> &tile)
		{
			constexpr float32 DIVIDER{ 1.0f / static_cast<float32>(UINT32_MAXIMUM) };

			uint32 hash{ tile._X * tile._Y };
			hash = (hash ^ 61u) ^ (hash >> 16u);
			hash *= 9u;
			hash = hash ^ (hash >> 4u);
			hash *= 0x27d4eb2du;
			hash = hash ^ (hash >> 15u);

			return BaseMathConstants::DOUBLE_PI * static_cast<float32>(hash) * DIVIDER;
		}
	};

	//Cache the user data.
	const TerrainPathTracingUserData *const RESTRICT user_data{ context._UserData.Get<TerrainPathTracingUserData>() };

	//Cache the instance data.
	const TerrainInstanceData &instance_data{ TerrainComponent::Instance->InstanceData()[user_data->_InstanceIndex] };

	//Calculate the base coordinate.
	const Vector2<uint32> base_coordinate
	{
		BaseMath::Minimum<uint32>(static_cast<uint32>(context._TextureCoordinate._X * static_cast<float32>(instance_data._HeightMap.GetWidth())), instance_data._HeightMap.GetWidth() - 1),
		BaseMath::Minimum<uint32>(static_cast<uint32>(context._TextureCoordinate._Y * static_cast<float32>(instance_data._HeightMap.GetHeight())), instance_data._HeightMap.GetHeight() - 1)
	};

	//Set up the sample coordinates.
	StaticArray<Vector2<uint32>, 4> sample_coordinates;

	sample_coordinates[0] = base_coordinate;
	sample_coordinates[1] = Vector2<uint32>(BaseMath::Minimum<uint32>(base_coordinate._X + 1, instance_data._HeightMap.GetWidth() - 1), base_coordinate._Y);
	sample_coordinates[2] = Vector2<uint32>(base_coordinate._X, BaseMath::Minimum<uint32>(base_coordinate._Y + 1, instance_data._HeightMap.GetHeight() - 1));
	sample_coordinates[3] = Vector2<uint32>(BaseMath::Minimum<uint32>(base_coordinate._X + 1, instance_data._HeightMap.GetWidth() - 1), BaseMath::Minimum<uint32>(base_coordinate._Y + 1, instance_data._HeightMap.GetHeight() - 1));

	//Sample the materials!
	StaticArray<TerrainMaterialSample, 4> material_samples;

	for (uint8 sample_index{ 0 }; sample_index < 4; ++sample_index)
	{
		//Sample the index map.
		const Vector4<uint8> index_map_sample{ instance_data._IndexMap.At(sample_coordinates[sample_index]._X, sample_coordinates[sample_index]._Y)};

		//Sample the blend map.
		Vector4<float32> blend_map_sample;

		{
			const Vector4<uint8> &sample{ instance_data._BlendMap.At(sample_coordinates[sample_index]._X, sample_coordinates[sample_index]._Y) };

			blend_map_sample[0] = static_cast<float32>(sample[0]) / static_cast<float32>(UINT8_MAXIMUM);
			blend_map_sample[1] = static_cast<float32>(sample[1]) / static_cast<float32>(UINT8_MAXIMUM);
			blend_map_sample[2] = static_cast<float32>(sample[2]) / static_cast<float32>(UINT8_MAXIMUM);
			blend_map_sample[3] = static_cast<float32>(sample[3]) / static_cast<float32>(UINT8_MAXIMUM);
		}

		//Calculate the material texture coordinate.
		Vector2<float32> material_texture_coordinate;

		{
			//Do triplanar mapping.
			Vector3<float32> sample_normal;

			{
				const Vector4<uint8> &sample{ instance_data._NormalMap.At(sample_coordinates[sample_index]._X, sample_coordinates[sample_index]._Y) };

				sample_normal[0] = (static_cast<float32>(sample[0]) / static_cast<float32>(UINT8_MAXIMUM)) * 2.0f - 1.0f;
				sample_normal[1] = (static_cast<float32>(sample[1]) / static_cast<float32>(UINT8_MAXIMUM)) * 2.0f - 1.0f;
				sample_normal[2] = (static_cast<float32>(sample[2]) / static_cast<float32>(UINT8_MAXIMUM)) * 2.0f - 1.0f;
			}

			const float32 x_weight{ BaseMath::Absolute(Vector3<float32>::DotProduct(sample_normal, Vector3<float32>(1.0f, 0.0f, 0.0f))) };
			const float32 y_weight{ BaseMath::Absolute(Vector3<float32>::DotProduct(sample_normal, Vector3<float32>(0.0f, 1.0f, 0.0f))) };
			const float32 z_weight{ BaseMath::Absolute(Vector3<float32>::DotProduct(sample_normal, Vector3<float32>(0.0f, 0.0f, 1.0f))) };

			if (x_weight > y_weight && x_weight > z_weight)
			{
				material_texture_coordinate = Vector2<float32>(context._WorldPosition._Y, context._WorldPosition._Z) * MATERIAL_TEXTURE_SCALE;
			}

			else if (y_weight > z_weight)
			{
				material_texture_coordinate = Vector2<float32>(context._WorldPosition._X, context._WorldPosition._Z) * MATERIAL_TEXTURE_SCALE;
			}

			else
			{
				material_texture_coordinate = Vector2<float32>(context._WorldPosition._X, context._WorldPosition._Y) * MATERIAL_TEXTURE_SCALE;
			}

			//Calculate the material tile.
			Vector2<uint32> material_tile;

			{
				const Vector2<uint32> coordinate_offset{ sample_coordinates[sample_index]._X - base_coordinate._X , sample_coordinates[sample_index]._Y - base_coordinate._Y };
				const Vector2<float32> world_offset{ static_cast<float32>(coordinate_offset._X) * MATERIAL_TEXTURE_SCALE, static_cast<float32>(coordinate_offset._Y) * MATERIAL_TEXTURE_SCALE };
				const Vector2<float32> world_material_coordinate{ material_texture_coordinate + world_offset };
				material_tile = Vector2<uint32>(static_cast<uint32>(world_material_coordinate._X), static_cast<uint32>(world_material_coordinate._Y));
			}

			//Rotate it based on the coordinate.
			material_texture_coordinate.Rotate(ROTATION_FOR_TILE(material_tile));
		}

		//Sample the sub materials.
		StaticArray<TerrainMaterialSample, 4> sub_material_samples;

		for (uint8 material_index{ 0 }; material_index < 4; ++material_index)
		{
			//Cache data.
			const uint8 _material_index{ index_map_sample[material_index] };
			const MaterialAsset *const RESTRICT material{ RenderingSystem::Instance->GetMaterialSystem()->GetMaterial(_material_index) };

			//Retrieve the albedo/thickness.
			Vector4<float32> albedo_thickness;

			if (material->_AlbedoThicknessComponent._Type == MaterialAsset::Component::Type::COLOR)
			{
				albedo_thickness = material->_AlbedoThicknessComponent._Color.Get();
			}

			else
			{
				albedo_thickness = SampleConvert(material->_AlbedoThicknessComponent._Texture->_Texture2D, material_texture_coordinate);
			}

			//Retrieve the normal map/displacement.
			Vector4<float32> normal_map_displacement;

			if (material->_NormalMapDisplacementComponent._Type == MaterialAsset::Component::Type::COLOR)
			{
				normal_map_displacement = material->_NormalMapDisplacementComponent._Color.Get();
			}

			else
			{
				normal_map_displacement = SampleConvert(material->_NormalMapDisplacementComponent._Texture->_Texture2D, material_texture_coordinate);
			}

			//Retrieve the material properties.
			Vector4<float32> material_properties;

			if (material->_MaterialPropertiesComponent._Type == MaterialAsset::Component::Type::COLOR)
			{
				material_properties = material->_MaterialPropertiesComponent._Color.Get();
			}

			else
			{
				material_properties = SampleConvert(material->_MaterialPropertiesComponent._Texture->_Texture2D, material_texture_coordinate);
			}

			//Fill in the material sample.
			sub_material_samples[material_index]._Albedo = Vector3<float32>(albedo_thickness._R, albedo_thickness._G, albedo_thickness._B);
			sub_material_samples[material_index]._Thickness = albedo_thickness._W;
			sub_material_samples[material_index]._NormalMap = Vector3<float32>(normal_map_displacement._X, normal_map_displacement._Y, normal_map_displacement._Z);
			sub_material_samples[material_index]._Displacement = normal_map_displacement._W;
			sub_material_samples[material_index]._Roughness = material_properties._X;
			sub_material_samples[material_index]._Metallic = material_properties._Y;
			sub_material_samples[material_index]._AmbientOcclusion = material_properties._Z;
			sub_material_samples[material_index]._Emissive = material_properties._W;
		}

		//Calculate the weights.
		Vector4<float32> weights;
		float32 weights_sum{ 0.0f };

		for (uint8 weight_index{ 0 }; weight_index < 4; ++weight_index)
		{
			weights[weight_index] = BaseMath::Maximum<float32>(powf(blend_map_sample[weight_index] * sub_material_samples[weight_index]._Displacement, DISPLACEMENT_POWER), MINIMUM_SAMPLE_WEIGHT);
			weights_sum += weights[weight_index];
		}

		const float32 inverse_weights_sum{ 1.0f / weights_sum };

		for (uint8 weight_index{ 0 }; weight_index < 4; ++weight_index)
		{
			weights[weight_index] *= inverse_weights_sum;
		}

		//Calculate the final material sample.
		material_samples[sample_index]._Albedo =	sub_material_samples[0]._Albedo * weights[0]
													+ sub_material_samples[1]._Albedo * weights[1]
													+ sub_material_samples[2]._Albedo * weights[2]
													+ sub_material_samples[3]._Albedo * weights[3];
		material_samples[sample_index]._Thickness = sub_material_samples[0]._Thickness * weights[0]
													+ sub_material_samples[1]._Thickness * weights[1]
													+ sub_material_samples[2]._Thickness * weights[2]
													+ sub_material_samples[3]._Thickness * weights[3];
		material_samples[sample_index]._NormalMap = sub_material_samples[0]._NormalMap * weights[0]
													+ sub_material_samples[1]._NormalMap * weights[1]
													+ sub_material_samples[2]._NormalMap * weights[2]
													+ sub_material_samples[3]._NormalMap * weights[3];
		material_samples[sample_index]._Displacement =	sub_material_samples[0]._Displacement * weights[0]
														+ sub_material_samples[1]._Displacement * weights[1]
														+ sub_material_samples[2]._Displacement * weights[2]
														+ sub_material_samples[3]._Displacement * weights[3];
		material_samples[sample_index]._Roughness = sub_material_samples[0]._Roughness * weights[0]
													+ sub_material_samples[1]._Roughness * weights[1]
													+ sub_material_samples[2]._Roughness * weights[2]
													+ sub_material_samples[3]._Roughness * weights[3];
		material_samples[sample_index]._Metallic =	sub_material_samples[0]._Metallic * weights[0]
													+ sub_material_samples[1]._Metallic * weights[1]
													+ sub_material_samples[2]._Metallic * weights[2]
													+ sub_material_samples[3]._Metallic * weights[3];
		material_samples[sample_index]._AmbientOcclusion =	sub_material_samples[0]._AmbientOcclusion * weights[0]
															+ sub_material_samples[1]._AmbientOcclusion * weights[1]
															+ sub_material_samples[2]._AmbientOcclusion * weights[2]
															+ sub_material_samples[3]._AmbientOcclusion * weights[3];
		material_samples[sample_index]._Emissive =	sub_material_samples[0]._Emissive * weights[0]
													+ sub_material_samples[1]._Emissive * weights[1]
													+ sub_material_samples[2]._Emissive * weights[2]
													+ sub_material_samples[3]._Emissive * weights[3];
	}

	//Calculate the weights.
	Vector4<float32> weights;
	float32 weights_sum{ 0.0f };
	
	{
		const float32 horizontal_weight{ BaseMath::Fractional(context._TextureCoordinate._X * static_cast<float32>(instance_data._HeightMap.GetWidth())) };
		const float32 vertical_weight{ BaseMath::Fractional(context._TextureCoordinate._Y * static_cast<float32>(instance_data._HeightMap.GetHeight())) };

		weights[0] = (1.0f - horizontal_weight) * (1.0f - vertical_weight);
		weights[1] = (horizontal_weight) * (1.0f - vertical_weight);
		weights[2] = (1.0f - horizontal_weight) * (vertical_weight);
		weights[3] = (horizontal_weight) * (vertical_weight);
	}

	for (uint8 weight_index{ 0 }; weight_index < 4; ++weight_index)
	{
		weights[weight_index] = BaseMath::Maximum<float32>(powf(weights[weight_index] * material_samples[weight_index]._Displacement, DISPLACEMENT_POWER), MINIMUM_SAMPLE_WEIGHT);
		weights_sum += weights[weight_index];
	}

	const float32 inverse_weights_sum{ 1.0f / weights_sum };

	for (uint8 weight_index{ 0 }; weight_index < 4; ++weight_index)
	{
		weights[weight_index] *= inverse_weights_sum;
	}

	//Calculate the final material sample.
	TerrainMaterialSample final_material_sample;

	final_material_sample._Albedo = material_samples[0]._Albedo * weights[0]
									+ material_samples[1]._Albedo * weights[1]
									+ material_samples[2]._Albedo * weights[2]
									+ material_samples[3]._Albedo * weights[3];
	final_material_sample._Thickness =	material_samples[0]._Thickness * weights[0]
										+ material_samples[1]._Thickness * weights[1]
										+ material_samples[2]._Thickness * weights[2]
										+ material_samples[3]._Thickness * weights[3];
	final_material_sample._NormalMap =	material_samples[0]._NormalMap * weights[0]
										+ material_samples[1]._NormalMap * weights[1]
										+ material_samples[2]._NormalMap * weights[2]
										+ material_samples[3]._NormalMap * weights[3];
	final_material_sample._NormalMap = final_material_sample._NormalMap * 2.0f - 1.0f;
	final_material_sample._Displacement =	material_samples[0]._Displacement * weights[0]
											+ material_samples[1]._Displacement * weights[1]
											+ material_samples[2]._Displacement * weights[2]
											+ material_samples[3]._Displacement * weights[3];
	final_material_sample._Roughness =	material_samples[0]._Roughness * weights[0]
										+ material_samples[1]._Roughness * weights[1]
										+ material_samples[2]._Roughness * weights[2]
										+ material_samples[3]._Roughness * weights[3];
	final_material_sample._Metallic =	material_samples[0]._Metallic * weights[0]
										+ material_samples[1]._Metallic * weights[1]
										+ material_samples[2]._Metallic * weights[2]
										+ material_samples[3]._Metallic * weights[3];
	final_material_sample._AmbientOcclusion =	material_samples[0]._AmbientOcclusion * weights[0]
												+ material_samples[1]._AmbientOcclusion * weights[1]
												+ material_samples[2]._AmbientOcclusion * weights[2]
												+ material_samples[3]._AmbientOcclusion * weights[3];
	final_material_sample._Emissive =	material_samples[0]._Emissive * weights[0]
										+ material_samples[1]._Emissive * weights[1]
										+ material_samples[2]._Emissive * weights[2]
										+ material_samples[3]._Emissive * weights[3];

	//Calculate the tangent space matrix.
	const Matrix3x3 tangent_space_matrix{ context._GeometryTangent, Vector3<float32>::CrossProduct(context._GeometryNormal, context._GeometryTangent), context._GeometryNormal };

	//Fill in the result.
	result->_Albedo = final_material_sample._Albedo;
	result->_ShadingNormal = Vector3<float32>::Normalize(tangent_space_matrix * final_material_sample._NormalMap);
	result->_Roughness = final_material_sample._Roughness;
	result->_Metallic = final_material_sample._Metallic;
	result->_AmbientOcclusion = final_material_sample._AmbientOcclusion;
	result->_Emissive = final_material_sample._Emissive;
	result->_Thickness = final_material_sample._Thickness;
}

/*
*	Gathers path tracing triangles.
*/
void TerrainComponent::GatherPathTracingTriangles(DynamicArray<Vertex> *const RESTRICT vertices, DynamicArray<PathTracingTriangle> *const RESTRICT triangles) NOEXCEPT
{
	//Iterate over all instances.
	for (uint64 instance_index{ 0 }; instance_index < NumberOfInstances(); ++instance_index)
	{
		//Cache data.
		const TerrainInstanceData &instance_data{ _InstanceData[instance_index] };

		//Cache the relative world position.
		const Vector3<float32> relative_world_position{ instance_data._WorldPosition.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell()) };

		//Triangulate the height map.
		for (uint32 Y{ 0 }; Y < instance_data._HeightMap.GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < instance_data._HeightMap.GetWidth(); ++X)
			{
				//Calculate the normalized coordinate.
				const Vector2<float32> normalized_coordinate
				{
					static_cast<float32>(X) / static_cast<float32>(instance_data._HeightMap.GetWidth() - 1),
					static_cast<float32>(Y) / static_cast<float32>(instance_data._HeightMap.GetHeight() - 1)
				};

				//Add the vertex.
				vertices->Emplace();
				Vertex &vertex{ vertices->Back() };

				vertex._Position = relative_world_position;
				vertex._Position._X += BaseMath::LinearlyInterpolate(static_cast<float32>(instance_data._PatchSize) * -0.5f, static_cast<float32>(instance_data._PatchSize) * 0.5f, normalized_coordinate._X);
				vertex._Position._Y += instance_data._HeightMap.At(X, Y);
				vertex._Position._Z += BaseMath::LinearlyInterpolate(static_cast<float32>(instance_data._PatchSize) * -0.5f, static_cast<float32>(instance_data._PatchSize) * 0.5f, normalized_coordinate._Y);

				{
					const Vector4<uint8> &sample{ instance_data._NormalMap.At(X, Y) };

					vertex._Normal[0] = (static_cast<float32>(sample[0]) / static_cast<float32>(UINT8_MAXIMUM)) * 2.0f - 1.0f;
					vertex._Normal[1] = (static_cast<float32>(sample[1]) / static_cast<float32>(UINT8_MAXIMUM)) * 2.0f - 1.0f;
					vertex._Normal[2] = (static_cast<float32>(sample[2]) / static_cast<float32>(UINT8_MAXIMUM)) * 2.0f - 1.0f;
				}

				{
					const float32 height_to_right{ instance_data._HeightMap.At(BaseMath::Minimum<uint32>(X + 1, instance_data._HeightMap.GetWidth() - 1), Y) };
					vertex._Tangent = Vector3<float32>::Normalize(Vector3<float32>(Vector3<float32>(relative_world_position._X + 1.0f, height_to_right, relative_world_position._Z) - relative_world_position));
				}

				vertex._TextureCoordinate = normalized_coordinate;

				//Add the triangle.
				if (X < (instance_data._HeightMap.GetWidth() - 1)
					&& Y < (instance_data._HeightMap.GetHeight() - 1))
				{
					{
						triangles->Emplace();
						PathTracingTriangle &triangle{ triangles->Back() };

						triangle._Indices[0] = vertices->LastIndex();
						triangle._Indices[1] = vertices->LastIndex() + instance_data._HeightMap.GetWidth();
						triangle._Indices[2] = vertices->LastIndex() + instance_data._HeightMap.GetWidth() + 1;
						triangle._DiscardFunction = nullptr;
						triangle._ShadingFunction = TerrainPathTracingShadingFunction;

						TerrainPathTracingUserData user_data;

						user_data._InstanceIndex = instance_index;

						(*triangle._UserData.Get<TerrainPathTracingUserData>()) = user_data;
					}

					{
						triangles->Emplace();
						PathTracingTriangle &triangle{ triangles->Back() };

						triangle._Indices[0] = vertices->LastIndex();
						triangle._Indices[1] = vertices->LastIndex() + instance_data._HeightMap.GetWidth() + 1;
						triangle._Indices[2] = vertices->LastIndex() + 1;
						triangle._DiscardFunction = nullptr;
						triangle._ShadingFunction = TerrainPathTracingShadingFunction;

						TerrainPathTracingUserData user_data;

						user_data._InstanceIndex = instance_index;

						(*triangle._UserData.Get<TerrainPathTracingUserData>()) = user_data;
					}
				}
			}
		}
	}
}

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Gathers  memory usage for the given terrain quad tree node.
*/
FORCE_INLINE NO_DISCARD uint64 TerrainQuadTreeNodeMemoryUsage(const TerrainQuadTreeNode &node) NOEXCEPT
{
	uint64 memory_usage{ 0 };

	memory_usage += sizeof(node._Depth);
	memory_usage += sizeof(node._Borders);
	memory_usage += sizeof(node._ChildNodes);
	memory_usage += sizeof(node._Minimum);
	memory_usage += sizeof(node._Maximum);
	memory_usage += sizeof(node._AxisAlignedBoundingBox);
	memory_usage += sizeof(node._Visible);
	memory_usage += sizeof(node._Position);
	memory_usage += sizeof(node._MinimumHeightMapCoordinate);
	memory_usage += sizeof(node._MaximumHeightMapCoordinate);
	memory_usage += sizeof(node._PatchSize);

	for (const TerrainQuadTreeNode &child : node._ChildNodes)
	{
		memory_usage += TerrainQuadTreeNodeMemoryUsage(child);
	}

	return memory_usage;
}

/*
*	Gathers statistics.
*/
void TerrainComponent::Statistics(ComponentStatistics *const RESTRICT statistics) NOEXCEPT
{
	//Gather from all instances.
	for (const TerrainInstanceData &instance_data : _InstanceData)
	{
		statistics->_CPUMemoryUsage += sizeof(instance_data._WorldPosition);
		statistics->_CPUMemoryUsage += sizeof(instance_data._WorldSpaceAxisAlignedBoundingBox);
		statistics->_CPUMemoryUsage += sizeof(instance_data._PatchSize);
		statistics->_CPUMemoryUsage += sizeof(instance_data._HeightMap);
		statistics->_CPUMemoryUsage += sizeof(float32) * instance_data._HeightMap.GetWidth() * instance_data._HeightMap.GetHeight();
		statistics->_CPUMemoryUsage += sizeof(instance_data._NormalMap);
		statistics->_CPUMemoryUsage += sizeof(Vector4<uint8>) * instance_data._NormalMap.GetWidth() * instance_data._NormalMap.GetHeight();
		statistics->_CPUMemoryUsage += sizeof(instance_data._IndexMap);
		statistics->_CPUMemoryUsage += sizeof(Vector4<uint8>) * instance_data._IndexMap.GetWidth() * instance_data._IndexMap.GetHeight();
		statistics->_CPUMemoryUsage += sizeof(instance_data._BlendMap);
		statistics->_CPUMemoryUsage += sizeof(Vector4<uint8>) * instance_data._BlendMap.GetWidth() * instance_data._BlendMap.GetHeight();
		statistics->_CPUMemoryUsage += sizeof(instance_data._BaseResolution);
		statistics->_CPUMemoryUsage += sizeof(instance_data._MaximumSubdivisionSteps);
		statistics->_CPUMemoryUsage += sizeof(instance_data._Buffer);
		statistics->_CPUMemoryUsage += sizeof(instance_data._IndexOffset);
		statistics->_CPUMemoryUsage += sizeof(instance_data._IndexCount);
		statistics->_CPUMemoryUsage += sizeof(instance_data._HeightMapTexture);
		statistics->_CPUMemoryUsage += sizeof(instance_data._HeightMapTextureIndex);
		statistics->_CPUMemoryUsage += sizeof(instance_data._NormalMapTexture);
		statistics->_CPUMemoryUsage += sizeof(instance_data._NormalMapTextureIndex);
		statistics->_CPUMemoryUsage += sizeof(instance_data._IndexMapTexture);
		statistics->_CPUMemoryUsage += sizeof(instance_data._IndexMapTextureIndex);
		statistics->_CPUMemoryUsage += sizeof(instance_data._BlendMapTexture);
		statistics->_CPUMemoryUsage += sizeof(instance_data._BlendMapTextureIndex);
		statistics->_CPUMemoryUsage += sizeof(instance_data._Visibility);
		statistics->_CPUMemoryUsage += TerrainQuadTreeNodeMemoryUsage(instance_data._QuadTree._RootNode);
		statistics->_CPUMemoryUsage += sizeof(instance_data._PhysicsActorHandle);

		statistics->_GPUMemoryUsage += sizeof(float32) * instance_data._HeightMap.GetWidth() * instance_data._HeightMap.GetHeight();
		statistics->_GPUMemoryUsage += sizeof(Vector4<uint8>) * instance_data._NormalMap.GetWidth() * instance_data._NormalMap.GetHeight();
		statistics->_GPUMemoryUsage += sizeof(Vector4<uint8>) * instance_data._IndexMap.GetWidth() * instance_data._IndexMap.GetHeight();
		statistics->_GPUMemoryUsage += sizeof(Vector4<uint8>) * instance_data._BlendMap.GetWidth() * instance_data._BlendMap.GetHeight();
		statistics->_GPUMemoryUsage += sizeof(TerrainVertex) * instance_data._VertexCount;
		statistics->_GPUMemoryUsage += sizeof(uint32) * instance_data._IndexCount;
	}
}
#endif