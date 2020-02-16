//Header file.
#include <Systems/PhysicsSystem.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

//Rendering.
#include <Rendering/Native/AccelerationStructure.h>

//Systems.
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainGeneralUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(PhysicsSystem);

//Physics system data.
namespace PhysicsSystemData
{
	//The terrain acceleration structure.
	AccelerationStructure _TerrainAccelerationStructure;

	//Denotes if the terrain acceleration structure has been built.
	bool _TerrainAccelerationStructureBuilt{ false };
}

/*
*	Updates the physics system during the physics update phase.
*/
void PhysicsSystem::PhysicsUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update all character movements.
	for (CharacterMovement *const RESTRICT movement : _CharacterMovements)
	{
		UpdateCharacterMovement(context, movement);
	}
}

/*
*	Casts a ray.
*/
void PhysicsSystem::CastRay(const Ray &ray, const PhysicsChannel channels, RaycastResult *const RESTRICT result) NOEXCEPT
{
	//Set the default properties of the result.
	result->_HasHit = false;
	result->_HitDistance = FLOAT_MAXIMUM;
	result->_Type = RaycastResult::Type::NONE;

	//Raycast against models.
	if (TEST_BIT(channels, PhysicsChannel::MODEL))
	{
		CastRayModels(ray, result);
	}

	//Raycast against the terrain.
	if (TEST_BIT(channels, PhysicsChannel::TERRAIN))
	{
		CastRayTerrain(ray, result);
	}
}

/*
*	Registers a character movement.
*/
void PhysicsSystem::RegisterCharacterMovement(CharacterMovement* const RESTRICT movement) NOEXCEPT
{
	//Add it to the container.
	_CharacterMovements.Emplace(movement);
}

/*
*	Registers model collision data.
*/
void PhysicsSystem::RegisterModelCollisionData(const uint64 entity_identifier, const ModelCollisionData& data) NOEXCEPT
{
	_ModelCollisionData[entity_identifier] = data;
}

/*
*	Unregisters model collision data.
*/
void PhysicsSystem::UnregisterModelCollisionData(const uint64 entity_identifer) NOEXCEPT
{
	
}

/*
*	Callback for when terrain has been initialized.
*/
void PhysicsSystem::OnTerrainInitialized() NOEXCEPT
{
	//Build the terrain acceleration structure.
	{
		//Cache relevant data.
		const Texture2D<float> &height_map{ TerrainSystem::Instance->GetTerrainProperties()->_HeightMap };
		const float height_map_resolution{ static_cast<float>(height_map.GetWidth()) };

		//Generate the terrain vertices and indices.
		DynamicArray<TerrainVertex> vertices;
		DynamicArray<uint32> indices;

		TerrainGeneralUtilities::GenerateTerrainPlane(	*TerrainSystem::Instance->GetTerrainProperties(),
														height_map.GetWidth() / 4,
														&vertices,
														&indices);

		//Construct all triangles.
		for (uint64 i{ 0 }, size{ indices.Size() }; i < size; i += 3)
		{
			//Construct the triangle.
			Triangle triangle;

			triangle._Vertices[0]._X = vertices[indices[i + 0]]._Position._X * height_map_resolution;
			triangle._Vertices[0]._Y = 0.0f;
			triangle._Vertices[0]._Z = vertices[indices[i + 0]]._Position._Y * height_map_resolution;

			triangle._Vertices[1]._X = vertices[indices[i + 1]]._Position._X * height_map_resolution;
			triangle._Vertices[1]._Y = 0.0f;
			triangle._Vertices[1]._Z = vertices[indices[i + 1]]._Position._Y * height_map_resolution;

			triangle._Vertices[2]._X = vertices[indices[i + 2]]._Position._X * height_map_resolution;
			triangle._Vertices[2]._Y = 0.0f;
			triangle._Vertices[2]._Z = vertices[indices[i + 2]]._Position._Y * height_map_resolution;

			TerrainSystem::Instance->GetTerrainHeightAtPosition(triangle._Vertices[0], &triangle._Vertices[0]._Y);
			TerrainSystem::Instance->GetTerrainHeightAtPosition(triangle._Vertices[1], &triangle._Vertices[1]._Y);
			TerrainSystem::Instance->GetTerrainHeightAtPosition(triangle._Vertices[2], &triangle._Vertices[2]._Y);

			//Add the triangle to the terrain acceleration structure.
			PhysicsSystemData::_TerrainAccelerationStructure.AddTriangleData(AccelerationStructure::TriangleData(triangle));
		}

		//Build the terrain acceleration structure.
		PhysicsSystemData::_TerrainAccelerationStructure.Build(4);

		//Signal that the terrain acceleration structure has been built.
		PhysicsSystemData::_TerrainAccelerationStructureBuilt = true;
	}
}

/*
*	Casts a ray against models.
*/
void PhysicsSystem::CastRayModels(const Ray &ray, RaycastResult *const RESTRICT result) NOEXCEPT
{
	for (const Pair<uint64, ModelCollisionData>& data : _ModelCollisionData)
	{
		switch (data._Second._Type)
		{
			case ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX:
			{
				float intersection_distance{ FLOAT_MAXIMUM };

				if (CatalystGeometryMath::RayBoxIntersection(ray, data._Second._AxisAlignedBoundingBoxData._AxisAlignedBoundingBox, &intersection_distance)
					&& result->_HitDistance > intersection_distance)
				{
					result->_HasHit = true;
					result->_HitDistance = intersection_distance;
					result->_Type = RaycastResult::Type::MODEL;
				}

				break;
			}
		}
	}
}

/*
*	Casts a ray against the terrain.
*/
void PhysicsSystem::CastRayTerrain(const Ray &ray, RaycastResult *const RESTRICT result) NOEXCEPT
{
	//Can´t trace against the terrain acceleration structure if it hasn´t been built.
	if (!PhysicsSystemData::_TerrainAccelerationStructureBuilt)
	{
		return;
	}

	//Trace the terrain acceleration structure.
	if (const AccelerationStructure::TriangleData *const RESTRICT triangle_data{ PhysicsSystemData::_TerrainAccelerationStructure.TraceSurface(ray, &result->_HitDistance) })
	{
		result->_HasHit = true;
		result->_Type = RaycastResult::Type::TERRAIN;
	}
}

/*
*	Updates one character movement.
*/
void PhysicsSystem::UpdateCharacterMovement(const UpdateContext* const RESTRICT context, CharacterMovement* const RESTRICT movement) NOEXCEPT
{
	//Apply different logic based on whether or not the character is jumping or not.
	if (movement->_IsJumping)
	{
		//Apply gravity.
		movement->_Velocity._Y -= PhysicsConstants::GRAVITY * context->_DeltaTime;

		//Apply the velocity to the position.
		movement->_Position += movement->_Velocity * context->_DeltaTime;

		//Clamp the position to the terrain height.
		float terrain_height{ 0.0f };

		TerrainSystem::Instance->GetTerrainHeightAtPosition(movement->_Position, &terrain_height);

		if (movement->_Position._Y <= terrain_height)
		{
			movement->_Position._Y = terrain_height;

			//Signal that this character is no longer jumping.
			movement->_IsJumping = false;
		}
	}

	else
	{
		//Apply the movement input to the velocity.
		movement->_Velocity += movement->_MovementInput;

		//Apply the jump input to the velocity.
		if (movement->_JumpInput > 0.0f)
		{
			movement->_Velocity += VectorConstants::UP * movement->_JumpInput;

			movement->_IsJumping = true;
		}

		//Apply the velocity to the position.
		movement->_Position += movement->_Velocity * context->_DeltaTime;

		//Clamp the position to the terrain height.
		if (!movement->_IsJumping)
		{
			float terrain_height{ 0.0f };

			TerrainSystem::Instance->GetTerrainHeightAtPosition(movement->_Position, &terrain_height);

			movement->_Position._Y = terrain_height;

			//Apply some ground damping when walking on terrain.
			movement->_Velocity *= 0.01f;
		}
	}

	//Reset the inputs.
	movement->_MovementInput = VectorConstants::ZERO;
	movement->_JumpInput = 0.0f;
}