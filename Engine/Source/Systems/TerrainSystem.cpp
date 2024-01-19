//Header file.
#include <Systems/TerrainSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>

//Terrain.
#include <Terrain/TerrainQuadTreeUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(TerrainSystem);

/*
*	Initializes the terrain system.
*/
void TerrainSystem::Initialize() NOEXCEPT
{
	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			static_cast<TerrainSystem *const RESTRICT>(arguments)->PreUpdate();
		},
		this,
		UpdatePhase::PRE,
		UpdatePhase::ENTITY,
		false,
		false
	);
}

/*
*	Updates the terrain system during the PRE update phase.
*/
void TerrainSystem::PreUpdate() NOEXCEPT
{
	//Cache the camera world position.
	const WorldPosition camera_world_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetWorldPosition() };
	//const WorldPosition camera_world_position{ }; //For debug purposes.

	//Update all terrain entities.
	const uint64 number_of_components{ ComponentManager::GetNumberOfTerrainComponents() };
	TerrainComponent *RESTRICT component{ ComponentManager::GetTerrainTerrainComponents() };

	for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
	{
		//Calculate the camera relative position.
		const Vector3<float32> camera_relative_position{ camera_world_position.GetRelativePosition(component->_WorldPosition.GetCell()) };
		
		//Check combination.
		CheckCombination(component, camera_relative_position, &component->_QuadTree._RootNode);

		//Check subdivision.
		CheckSubdivision(component, camera_relative_position, &component->_QuadTree._RootNode);

		//Calculate the borders.
		CalculateBorders(component, &component->_QuadTree._RootNode);
	}
}

/*
*	Checks combination of a node.
*/
void TerrainSystem::CheckCombination(TerrainComponent *const RESTRICT component, const Vector3<float32> &camera_position, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	//If this node is already subdivided, check all of it's child nodes.
	if (node->IsSubdivided())
	{
		if (TerrainQuadTreeUtilities::ShouldBeCombined(component, *node, camera_position))
		{
			node->_ChildNodes.Clear();
		}

		else
		{
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				CheckCombination(component, camera_position, &node->_ChildNodes[i]);
			}
		}
	}
}

/*
*	Checks subdivisions of a node.
*/
void TerrainSystem::CheckSubdivision(TerrainComponent *const RESTRICT component, const Vector3<float32>& camera_position, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT
{
	//If this node is already subdivided, check all of it's child nodes.
	if (node->IsSubdivided())
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			CheckSubdivision(component, camera_position, &node->_ChildNodes[i]);
		}
	}

	//Else, check if this node should be subdivided.
	else
	{
		if (TerrainQuadTreeUtilities::ShouldBeSubdivided(component, *node, camera_position))
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
							node->_Minimum._X + static_cast<float32>(component->_PatchSize) * patch_size_multiplier * 1.5f,
							node->_Minimum._Y + static_cast<float32>(component->_PatchSize) * patch_size_multiplier * 0.5f
						);

						break;
					}

					case 1:
					{
						child_node._Minimum = child_node._Maximum = Vector2<float32>
						(
							node->_Minimum._X + static_cast<float32>(component->_PatchSize) * patch_size_multiplier * 1.5f,
							node->_Minimum._Y + static_cast<float32>(component->_PatchSize) * patch_size_multiplier * 1.5f
						);
						
						break;
					}

					case 2:
					{
						child_node._Minimum = child_node._Maximum = Vector2<float32>
						(
							node->_Minimum._X + static_cast<float32>(component->_PatchSize) * patch_size_multiplier * 0.5f,
							node->_Minimum._Y + static_cast<float32>(component->_PatchSize) * patch_size_multiplier * 1.5f
						);

						break;
					}

					case 3:
					{
						child_node._Minimum = child_node._Maximum = Vector2<float32>
						(
							node->_Minimum._X + static_cast<float32>(component->_PatchSize) * patch_size_multiplier * 0.5f,
							node->_Minimum._Y + static_cast<float32>(component->_PatchSize) * patch_size_multiplier * 0.5f
						);
	
						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}

				child_node._Minimum -= Vector2<float32>(static_cast<float32>(component->_PatchSize) * patch_size_multiplier * 0.5f);
				child_node._Maximum += Vector2<float32>(static_cast<float32>(component->_PatchSize) * patch_size_multiplier * 0.5f);
			
				/*
				*	Use the parent node's min/max height, as it would be too time-consuming to compute for each individual node.
				*	Should be a good enough approximation.
				*/
				child_node._AxisAlignedBoundingBox._Minimum = Vector3<float32>(child_node._Minimum._X, node->_AxisAlignedBoundingBox._Minimum._Y, child_node._Minimum._Y);
				child_node._AxisAlignedBoundingBox._Maximum = Vector3<float32>(child_node._Maximum._X, node->_AxisAlignedBoundingBox._Maximum._Y, child_node._Maximum._Y);

				child_node._Position = TerrainQuadTreeUtilities::MiddlePoint(child_node);
				const Vector2<float32> heightmap_coordinate_offset{ static_cast<float32>(component->_PatchSize) * 0.5f };
				child_node._MinimumHeightMapCoordinate = (child_node._Minimum + heightmap_coordinate_offset) / static_cast<float32>(component->_PatchSize + 1);
				child_node._MaximumHeightMapCoordinate = (child_node._Maximum + heightmap_coordinate_offset) / static_cast<float32>(component->_PatchSize + 1);
				child_node._PatchSize = static_cast<float32>(component->_PatchSize) * patch_size_multiplier;
			}
		}
	}
}

/*
*	Calculates borders for the given node.
*/
void TerrainSystem::CalculateBorders(TerrainComponent *const RESTRICT component, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT
{
	//If this node is subdivided, calculate new borders for it's child nodes.
	if (node->IsSubdivided())
	{
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			CalculateBorders(component, &node->_ChildNodes[i]);
		}
	}

	else
	{
		//Retrieve the neighboring nodes.
		StaticArray<const TerrainQuadTreeNode *RESTRICT, 4> neighboring_nodes;
		TerrainQuadTreeUtilities::NeighboringNodes(component, *node, &neighboring_nodes);

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