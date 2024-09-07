//Header file.
#include <Components/Components/TerrainComponent.h>

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
	PROFILING_SCOPE("TerrainComponent::ParallelBatchUpdate");

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

	initialization_data->_PreprocessedData._IndexOffset = static_cast<uint32>(buffer_data_sizes[0]);
	initialization_data->_PreprocessedData._IndexCount = static_cast<uint32>(indices.Size());

	//Create the height map texture.
	{
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(initialization_data->_HeightMap), TextureFormat::R_FLOAT32, TextureUsage::NONE, false), &initialization_data->_PreprocessedData._HeightMapTexture);
		initialization_data->_PreprocessedData._HeightMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(initialization_data->_PreprocessedData._HeightMapTexture);
	}

	//Create the normal map texture.
	{
		Texture2D<Vector4<uint8>> converted_normal_map_texture{ initialization_data->_NormalMap.GetResolution() };

		for (uint32 Y{ 0 }; Y < converted_normal_map_texture.GetResolution(); ++Y)
		{
			for (uint32 X{ 0 }; X < converted_normal_map_texture.GetResolution(); ++X)
			{
				const Vector3<float32> &actual_normal{ initialization_data->_NormalMap.At(X, Y) };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					converted_normal_map_texture.At(X, Y)[i] = static_cast<uint8>((actual_normal[i] * 0.5f + 0.5f) * static_cast<float32>(UINT8_MAXIMUM));
				}
			}
		}

		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(converted_normal_map_texture), TextureFormat::RGBA_UINT8, TextureUsage::NONE, false), &initialization_data->_PreprocessedData._NormalMapTexture);
		initialization_data->_PreprocessedData._NormalMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(initialization_data->_PreprocessedData._NormalMapTexture);
	}

	//Create the index map texture.
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(initialization_data->_IndexMap), TextureFormat::RGBA_UINT8, TextureUsage::NONE, false), &initialization_data->_PreprocessedData._IndexMapTexture);
	initialization_data->_PreprocessedData._IndexMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(initialization_data->_PreprocessedData._IndexMapTexture);

	//Create the blend map texture.
	{
		Texture2D<Vector4<uint8>> converted_blend_map_texture{ initialization_data->_BlendMap.GetResolution() };

		for (uint32 Y{ 0 }; Y < converted_blend_map_texture.GetResolution(); ++Y)
		{
			for (uint32 X{ 0 }; X < converted_blend_map_texture.GetResolution(); ++X)
			{
				for (uint8 i{ 0 }; i < 4; ++i)
				{
					converted_blend_map_texture.At(X, Y)[i] = static_cast<uint8>(initialization_data->_BlendMap.At(X, Y)[i] * static_cast<float32>(UINT8_MAXIMUM));
				}
			}
		}

		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(converted_blend_map_texture), TextureFormat::RGBA_UINT8, TextureUsage::NONE, false), &initialization_data->_PreprocessedData._BlendMapTexture);
		initialization_data->_PreprocessedData._BlendMapTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(initialization_data->_PreprocessedData._BlendMapTexture);
	}
}

/*
*	Creates an instance.
*/
void TerrainComponent::CreateInstance(Entity *const RESTRICT entity, TerrainInitializationData *const RESTRICT initialization_data, TerrainInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	//Set up the instance data.
	ASSERT(initialization_data->_HeightMap.GetWidth() == initialization_data->_HeightMap.GetHeight(), "Terrain height map width and height doesn't match - This isn't okay.");

	instance_data->_WorldPosition = initialization_data->_WorldPosition;
	instance_data->_WorldSpaceAxisAlignedBoundingBox = initialization_data->_PreprocessedData._WorldSpaceAxisAlignedBoundingBox;
	instance_data->_PatchSize = initialization_data->_PatchSize;
	instance_data->_HeightMap = std::move(initialization_data->_HeightMap);
	instance_data->_NormalMap = std::move(initialization_data->_NormalMap);
	instance_data->_IndexMap = std::move(initialization_data->_IndexMap);
	instance_data->_BlendMap = std::move(initialization_data->_BlendMap);
	instance_data->_BaseResolution = initialization_data->_BaseResolution;
	instance_data->_MaximumSubdivisionSteps = initialization_data->_MaximumSubdivisionSteps;

	instance_data->_Buffer = initialization_data->_PreprocessedData._Buffer;
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

	//Create the physics actor.
	PhysicsSystem::Instance->CreateHeightFieldActor
	(
		instance_data->_WorldPosition,
		instance_data->_HeightMap,
		&instance_data->_PhysicsActorHandle
	);
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