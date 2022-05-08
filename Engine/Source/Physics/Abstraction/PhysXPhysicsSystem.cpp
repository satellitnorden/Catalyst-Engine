#if defined(CATALYST_PHYSICS_PHYSX)
//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Concurrency.
#include <Concurrency/Task.h>

//Entities.
#include <Entities/Types/DynamicModelEntity.h>
#include <Entities/Types/StaticModelEntity.h>

//Physics.
#include <Physics/Abstraction/PhysXCharacterControllerAbstractionData.h>
#include <Physics/Native/PhysicsUtilities.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>

//Third party.
#include <ThirdParty/PhysX/PxPhysicsAPI.h>
#include <ThirdParty/PhysX/extensions/PxRigidBodyExt.h>

//Macros.
#define PX_RELEASE(POINTER)	if(POINTER)	{ POINTER->release(); POINTER = nullptr; }

/*
*	Query filter callback class definition.
*/
class QueryFilterCallback final : public physx::PxQueryFilterCallback
{

public:

	/*
	*	Constructor taking in the raycast configuration.
	*/
	FORCE_INLINE QueryFilterCallback(const RaycastConfiguration &initial_raycast_configuration) NOEXCEPT
		:
		_RaycastConfiguration(initial_raycast_configuration)
	{

	}

	/*
	*	The pre filter callback.
	*/
	FORCE_INLINE physx::PxQueryHitType::Enum preFilter(const physx::PxFilterData &filter_data, const physx::PxShape *const RESTRICT shape, const physx::PxRigidActor *const RESTRICT actor, physx::PxHitFlags &queryFlags) NOEXCEPT
	{
		const bool is_character{ actor->userData == nullptr };
		const bool is_dynamic_model{ actor->userData && static_cast<Entity *const RESTRICT>(actor->userData)->_Type == EntityType::DynamicModel };
		const bool is_static_model{ actor->userData && static_cast<Entity *const RESTRICT>(actor->userData)->_Type == EntityType::StaticModel };

		physx::PxQueryHitType::Enum final_return{ physx::PxQueryHitType::eBLOCK };

		final_return = static_cast<physx::PxQueryHitType::Enum>(final_return * (is_character ? static_cast<int32>(TEST_BIT(_RaycastConfiguration._PhysicsChannels, PhysicsChannel::CHARACTERS)) : 1));
		final_return = static_cast<physx::PxQueryHitType::Enum>(final_return * (is_dynamic_model ? static_cast<int32>(TEST_BIT(_RaycastConfiguration._PhysicsChannels, PhysicsChannel::DYNAMIC_MODELS)) : 1));
		final_return = static_cast<physx::PxQueryHitType::Enum>(final_return * (is_static_model ? static_cast<int32>(TEST_BIT(_RaycastConfiguration._PhysicsChannels, PhysicsChannel::STATIC_MODELS)) : 1));

		return final_return;
	}

	/*
	*	The post filter callback.
	*/
	FORCE_INLINE physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData &filter_data, const physx::PxQueryHit &hit) NOEXCEPT
	{
		ASSERT(false, "postFilter function shouldn't run?");

		return physx::PxQueryHitType::eNONE;
	}

private:

	//The raycast configuration.
	RaycastConfiguration _RaycastConfiguration;

};

/*
*	Terrain data class definition.
*/
class TerrainData final
{

public:

	//The grid point.
	GridPoint2 _GridPoint;

	//The actor.
	physx::PxRigidStatic *RESTRICT _Actor{ nullptr };

	//The shape.
	physx::PxShape *RESTRICT _Shape{ nullptr };

};

/*
*	Dynamic model entity data class definition.
*/
class DynamicModelEntityData final
{

public:

	//The entity.
	DynamicModelEntity *RESTRICT _Entity;

	//The actor.
	physx::PxRigidDynamic *RESTRICT _Actor;

	//The translation offset.
	Vector3<float32> _TranslationOffset;

};

/*
*	Static model entity data class definition.
*/
class StaticModelEntityData final
{

public:

	//The entity.
	StaticModelEntity *RESTRICT _Entity;

	//The actor.
	physx::PxRigidStatic *RESTRICT _Actor;

};

//PhysX physics system constants.
namespace PhysXPhysicsSystemConstants
{

	//The update frequency.
	constexpr float32 UPDATE_FREQUENCY{ 1.0f / 60.0f };

	//The terrain height field size.
	constexpr uint32 TERRAIN_HEIGHT_FIELD_SIZE{ 256 }; //Must be a power of two. (:

	//The terrain height field half size.
	constexpr float32 TERRAIN_HEIGHT_FIELD_HALF_SIZE{ static_cast<float32>(TERRAIN_HEIGHT_FIELD_SIZE) * 0.5f };

}

//PhysX physics system data.
namespace PhysXPhysicsSystemData
{

	//The default allocator.
	physx::PxDefaultAllocator _DefaultAllocator;

	//The default error callback.
	physx::PxDefaultErrorCallback _DefaultErrorCallback;
	
	//The foundation.
	physx::PxFoundation *RESTRICT _Foundation{ nullptr };

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//The PVD.
	physx::PxPvd *RESTRICT _PVD{ nullptr };
#endif

	//The physics lock.
	Spinlock _PhysicsLock;

	//The physics.
	physx::PxPhysics *RESTRICT _Physics{ nullptr };
	
	//The cooking lock.
	Spinlock _CookingLock;

	//The cooking.
	physx::PxCooking *RESTRICT _Cooking{ nullptr };

	//The dispatcher.
	physx::PxDefaultCpuDispatcher *RESTRICT _Dispatcher{ nullptr };

	//The scene lock.
	Spinlock _SceneLock;

	//The scene.
	physx::PxScene *RESTRICT _Scene{ nullptr };

	//The default material.
	physx::PxMaterial *RESTRICT _DefaultMaterial{ nullptr };

	//The controller manager.
	physx::PxControllerManager *RESTRICT _ControllerManager{ nullptr };

	//The dynamic model entity data.
	DynamicArray<DynamicModelEntityData> _DynamicModelEntityData;

	//The static model entity data.
	DynamicArray<StaticModelEntityData> _StaticModelEntityData;

	//The time since last update.
	float32 _TimeSinceLastUpdate{ 0.0f };

	//Denotes if physics is being updated.
	bool _PhysicsIsUpdating{ false };

	//The terrain generation task.
	Task _TerrainGenerationTask;

	//The terrain data.
	StaticArray<TerrainData, 9> _TerrainData;

}

//PhysX physics system logic.
namespace PhysXPhysicsSystemLogic
{

	/*
	*	Updates the terrain height field.
	*/
	FORCE_INLINE void UpdateTerrainHeightField() NOEXCEPT
	{
		//Cache the perceiver current world position.
		const Vector3<float32> perceiver_world_position{ Perceiver::Instance->GetWorldTransform().GetWorldPosition().GetAbsolutePosition() };

		//Calculate the perceiver grid point.
		GridPoint2 perceiver_grid_point{ GridPoint2::WorldPositionToGridPoint(perceiver_world_position, static_cast<float32>(PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE)) };

		perceiver_grid_point._X = -perceiver_grid_point._X;
		//perceiver_grid_point._Y = -perceiver_grid_point._Y;

		//Cache the valid grid points.
		const StaticArray<GridPoint2, 9> valid_grid_points
		{
			perceiver_grid_point + GridPoint2(0, 0),

			perceiver_grid_point + GridPoint2(-1, 0),
			perceiver_grid_point + GridPoint2(1, 0),
			perceiver_grid_point + GridPoint2(0, -1),
			perceiver_grid_point + GridPoint2(0, 1),

			perceiver_grid_point + GridPoint2(-1, -1),
			perceiver_grid_point + GridPoint2(-1, 1),
			perceiver_grid_point + GridPoint2(1, -1),
			perceiver_grid_point + GridPoint2(1, 1)
		};

		//Remove terrain data with grid points that aren't valid.
		for (TerrainData &terrain_data : PhysXPhysicsSystemData::_TerrainData)
		{
			if (terrain_data._Actor
				&& terrain_data._Shape)
			{
				bool grid_point_is_valid{ false };

				for (const GridPoint2 valid_grid_point : valid_grid_points)
				{
					if (terrain_data._GridPoint == valid_grid_point)
					{
						grid_point_is_valid = true;

						break;
					}
				}

				if (!grid_point_is_valid)
				{
					terrain_data._Actor->detachShape(*terrain_data._Shape);

					terrain_data._Shape->release();
					terrain_data._Shape = nullptr;
				}
			}
		}

		//Add terrain data with grid points that are valid.
		for (const GridPoint2 valid_grid_point : valid_grid_points)
		{
			bool found_grid_point{ false };

			for (TerrainData &terrain_data : PhysXPhysicsSystemData::_TerrainData)
			{
				if (terrain_data._Actor
					&& terrain_data._Shape
					&& valid_grid_point == terrain_data._GridPoint)
				{
					found_grid_point = true;

					break;
				}
			}

			if (!found_grid_point)
			{
				for (TerrainData &terrain_data : PhysXPhysicsSystemData::_TerrainData)
				{
					if (!terrain_data._Shape)
					{
						//Calculate the terrain patch position.
						const Vector3<float32> terrain_patch_position{ GridPoint2::GridPointToWorldPosition(GridPoint2(-valid_grid_point._X, valid_grid_point._Y), static_cast<float32>(PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE)) };

						//Update the grid point.
						terrain_data._GridPoint = valid_grid_point;

						//Update the actor.
						{
							const physx::PxTransform terrain_transform{ physx::PxVec3(terrain_patch_position._X - PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_HALF_SIZE, 0.0f, terrain_patch_position._Z - static_cast<float32>(PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_HALF_SIZE)) };

							if (!terrain_data._Actor)
							{

								{
									SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
									terrain_data._Actor = PhysXPhysicsSystemData::_Physics->createRigidStatic(terrain_transform);
								}

								{
									SCOPED_LOCK(PhysXPhysicsSystemData::_SceneLock);
									PhysXPhysicsSystemData::_Scene->addActor(*terrain_data._Actor);
								}
							}

							else
							{
								terrain_data._Actor->setGlobalPose(terrain_transform);
							}
						}

						//Update the shape.
						{
							//Construct the height map, whilst remembering the highest height.
							Texture2D<float32> height_map{ PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE };

							for (uint32 Y{ 0 }; Y < PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE; ++Y)
							{
								for (uint32 X{ 0 }; X < PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE; ++X)
								{
									//Calculate the sample position scalar.
									const Vector2<float32> sample_position_scalar{	static_cast<float32>(X) / static_cast<float32>(PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE - 1),
																					static_cast<float32>(Y) / static_cast<float32>(PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE - 1) };

									//Calculate the sample world position.
									const Vector3<float32> sample_world_position{	terrain_patch_position._X + CatalystBaseMath::LinearlyInterpolate(-PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_HALF_SIZE, PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_HALF_SIZE, sample_position_scalar._X),
																					0.0f,
																					terrain_patch_position._Z + CatalystBaseMath::LinearlyInterpolate(-PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_HALF_SIZE, PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_HALF_SIZE, sample_position_scalar._Y) };

									//Retrieve the terrain height.
									float32 terrain_height;
									TerrainSystem::Instance->GetTerrainHeightAtPosition(sample_world_position, &terrain_height);

									//Write the terrain height.
									height_map.At(X, Y) = terrain_height;
								}
							}

							//Calculate the height scale.
							float32 maximum_height{ 0.0f };
							TerrainSystem::Instance->GetMaximumHeight(&maximum_height);

							const float32 height_scale{ maximum_height / static_cast<float32>(INT16_MAXIMUM) };

							//Construct the height field samples.
							Texture2D<physx::PxHeightFieldSample> height_field_samples{ PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE };
							Memory::Set(height_field_samples.Data(), 0, sizeof(physx::PxHeightFieldSample) * PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE * PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE);

							for (uint32 Y{ 0 }; Y < PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE; ++Y)
							{
								for (uint32 X{ 0 }; X < PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE; ++X)
								{
									height_field_samples.At(Y, X).height = static_cast<int16>(height_map.At(X, Y) / height_scale);
								}
							}

							//Create the height field.
							physx::PxHeightFieldDesc height_field_description;

							height_field_description.setToDefault();

							height_field_description.nbRows = PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE;
							height_field_description.nbColumns = PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE;
							height_field_description.format = physx::PxHeightFieldFormat::eS16_TM;
							height_field_description.samples.data = height_field_samples.Data();
							height_field_description.samples.stride = sizeof(physx::PxHeightFieldSample);
							height_field_description.flags = physx::PxHeightFieldFlag::eNO_BOUNDARY_EDGES;

							physx::PxHeightField *RESTRICT height_field;

							physx::PxPhysicsInsertionCallback *RESTRICT physics_insertion_callback;

							{
								SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
								physics_insertion_callback = &PhysXPhysicsSystemData::_Physics->getPhysicsInsertionCallback();
							}

							{
								SCOPED_LOCK(PhysXPhysicsSystemData::_CookingLock);

								height_field = PhysXPhysicsSystemData::_Cooking->createHeightField(height_field_description, *physics_insertion_callback);
							}

							//Create the height field geometry.
							const physx::PxHeightFieldGeometry height_field_geometry{ height_field, physx::PxMeshGeometryFlags(), height_scale, 1.0f + (1.0f / static_cast<float32>(PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE)), 1.0f + (1.0f / static_cast<float32>(PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE)) };

							//Create the new terrain shape.
							{
								SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
								terrain_data._Shape = PhysXPhysicsSystemData::_Physics->createShape(height_field_geometry, *PhysXPhysicsSystemData::_DefaultMaterial, true);
							}

							//Decrement the reference count of the height field, so that it gets automatically release with the shape later.
							height_field->release();

							//Attach the new shape.
							terrain_data._Actor->attachShape(*terrain_data._Shape);
						}

						break;
					}
				}
			}
		}
	}

}

/*
*	Initializes the physics sub-system.
*/
void PhysicsSystem::SubInitialize() NOEXCEPT
{
	//Create the foundation.
	PhysXPhysicsSystemData::_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, PhysXPhysicsSystemData::_DefaultAllocator, PhysXPhysicsSystemData::_DefaultErrorCallback);
	
	//Create the PVD.
#if !defined(CATALYST_CONFIGURATION_FINAL)
	PhysXPhysicsSystemData::_PVD = physx::PxCreatePvd(*PhysXPhysicsSystemData::_Foundation);
	physx::PxPvdTransport *const RESTRICT transport{ physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10) };
	PhysXPhysicsSystemData::_PVD->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
#endif

	//Create the physics.
#if !defined(CATALYST_CONFIGURATION_FINAL)
	PhysXPhysicsSystemData::_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *PhysXPhysicsSystemData::_Foundation, physx::PxTolerancesScale(), false, PhysXPhysicsSystemData::_PVD);
#else
	PhysXPhysicsSystemData::_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *PhysXPhysicsSystemData::_Foundation, physx::PxTolerancesScale(), false, nullptr);
#endif

	//Create the cooking.
	{
		physx::PxCookingParams cooking_paramters{ physx::PxTolerancesScale() };

		{
			SCOPED_LOCK(PhysXPhysicsSystemData::_CookingLock);

			PhysXPhysicsSystemData::_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *PhysXPhysicsSystemData::_Foundation, cooking_paramters);
		}
	}

	//Create the dispatcher.
	PhysXPhysicsSystemData::_Dispatcher = physx::PxDefaultCpuDispatcherCreate(2);

	//Create the scene.
	physx::PxSceneDesc scene_description{ PhysXPhysicsSystemData::_Physics->getTolerancesScale() };
	scene_description.gravity = physx::PxVec3(0.0f, -PhysicsConstants::GRAVITY, 0.0f);
	scene_description.cpuDispatcher	= PhysXPhysicsSystemData::_Dispatcher;
	scene_description.filterShader = physx::PxDefaultSimulationFilterShader;

	PhysXPhysicsSystemData::_Scene = PhysXPhysicsSystemData::_Physics->createScene(scene_description);

	//Create the default material.
	PhysXPhysicsSystemData::_DefaultMaterial = PhysXPhysicsSystemData::_Physics->createMaterial(0.5f, 0.5f, 0.6f);

	//Create the controller manager.
	PhysXPhysicsSystemData::_ControllerManager = PxCreateControllerManager(*PhysXPhysicsSystemData::_Scene);

	//Set up the terrain generation task.
	PhysXPhysicsSystemData::_TerrainGenerationTask._Function = [](void *const RESTRICT)
	{
		PhysXPhysicsSystemLogic::UpdateTerrainHeightField();
	};
	PhysXPhysicsSystemData::_TerrainGenerationTask._Arguments = nullptr;
	PhysXPhysicsSystemData::_TerrainGenerationTask._ExecutableOnSameThread = false;
}

/*
*	Creates a collision model on the sub-system.
*/
void PhysicsSystem::SubCreateCollisionModel(const CollisionModelData &collision_model_data, CollisionModelHandle *const RESTRICT collision_model) NOEXCEPT
{
	//Set up the memory input data.
	physx::PxDefaultMemoryInputData memory_input_data{ const_cast<byte *const RESTRICT>(collision_model_data._Data.Data()), static_cast<uint32>(collision_model_data._Data.Size()) };

	//Create the mesh.
	switch (collision_model_data._Type)
	{
		case CollisionModelData::Type::CONCAVE:
		{
			{
				SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
				*collision_model = PhysXPhysicsSystemData::_Physics->createTriangleMesh(memory_input_data);
			}

			break;
		}

		case CollisionModelData::Type::CONVEX:
		{
			{
				SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
				*collision_model = PhysXPhysicsSystemData::_Physics->createConvexMesh(memory_input_data);
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
*	Updates the physics sub-system during the physics update phase.
*/
void PhysicsSystem::SubPhysicsUpdate() NOEXCEPT
{
	//Physics is updating.
	PhysXPhysicsSystemData::_PhysicsIsUpdating = true;

	//Cache the delta time.
	const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

	//Update the time since last update.
	PhysXPhysicsSystemData::_TimeSinceLastUpdate += delta_time;

	//Should the physics be updated?
	bool physics_was_updated{ false };

	if (PhysXPhysicsSystemData::_TimeSinceLastUpdate >= PhysXPhysicsSystemConstants::UPDATE_FREQUENCY)
	{
		//Update the time since last update.
		PhysXPhysicsSystemData::_TimeSinceLastUpdate -= PhysXPhysicsSystemConstants::UPDATE_FREQUENCY;

		{
			SCOPED_LOCK(PhysXPhysicsSystemData::_SceneLock);

			//Simulate the scene.
			PhysXPhysicsSystemData::_Scene->simulate(PhysXPhysicsSystemConstants::UPDATE_FREQUENCY);

			//Fetch the results.
			PhysXPhysicsSystemData::_Scene->fetchResults(true);
		}

		//Physics was updated!
		physics_was_updated = true;
	}

	if (physics_was_updated)
	{
		//Update all dynamic models.
		for (const DynamicModelEntityData &data : PhysXPhysicsSystemData::_DynamicModelEntityData)
		{
			//Retrieve the position/rotation.
			Vector3<float32> position;
			Quaternion rotation;

			{
				physx::PxShape *RESTRICT shape;
				data._Actor->getShapes(&shape, 1);
				const physx::PxTransform translation_offset{ physx::PxVec3(data._TranslationOffset._X, data._TranslationOffset._Y, data._TranslationOffset._Z) };
				const physx::PxTransform final_transform{ data._Actor->getGlobalPose() * shape->getLocalPose() * translation_offset };

				Memory::Copy(&position, &final_transform.p, sizeof(Vector3<float32>));
				Memory::Copy(&rotation, &final_transform.q, sizeof(Quaternion));
			}

			//Set up the new world transform.
			WorldTransform new_world_transform;

			new_world_transform.SetAbsolutePosition(position);

			const EulerAngles euler_angles{ rotation.ToEulerAngles() };
			new_world_transform.SetRotation(euler_angles);

			//Set the world transform of the entity.
			data._Entity->SetWorldTransform(new_world_transform);
		}
	}

	//See if the terrain generation task is done.
	if (TerrainSystem::Instance->CanTerrainBeGenerated()
		&& PhysXPhysicsSystemData::_TerrainGenerationTask.IsExecuted())
	{
		//Fire off the task again.
		TaskSystem::Instance->ExecuteTask(&PhysXPhysicsSystemData::_TerrainGenerationTask);
	}

	//Physics is no longer updating.
	PhysXPhysicsSystemData::_PhysicsIsUpdating = false;
}

/*
*	Terminates the physics sub-system.
*/
void PhysicsSystem::SubTerminate() NOEXCEPT
{
	//Release the controller manager.
	PX_RELEASE(PhysXPhysicsSystemData::_ControllerManager);

	//Release the default material.
	PX_RELEASE(PhysXPhysicsSystemData::_DefaultMaterial);

	//Release the scene.
	PX_RELEASE(PhysXPhysicsSystemData::_Scene);

	//Release the dispatcher.
	PX_RELEASE(PhysXPhysicsSystemData::_Dispatcher);

	//Release the cooking.
	PX_RELEASE(PhysXPhysicsSystemData::_Cooking);

	//Release the physics.
	PX_RELEASE(PhysXPhysicsSystemData::_Physics);

	//Release the PVD.
#if !defined(CATALYST_CONFIGURATION_FINAL)
	if(PhysXPhysicsSystemData::_PVD)
	{
		physx::PxPvdTransport *RESTRICT transport{ PhysXPhysicsSystemData::_PVD->getTransport() };
		PhysXPhysicsSystemData::_PVD->release();
		PhysXPhysicsSystemData::_PVD = nullptr;
		PX_RELEASE(transport);
	}
#endif

	//Release the foundation.
	PX_RELEASE(PhysXPhysicsSystemData::_Foundation);
}

/*
*	Initializes the sub-system physics for the given entity.
*/
void PhysicsSystem::SubInitializeEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT
{
	//Which type is this entity?
	switch (entity->_Type)
	{
		case EntityType::DynamicModel:
		{
			//Cache the component.
			const DynamicModelComponent &component{ ComponentManager::GetDynamicModelDynamicModelComponents()[entity->_ComponentsIndex] };

			//Check if collision is requested.
			if (component._ModelCollisionConfiguration._Type == ModelCollisionType::NONE)
			{
				return;
			}

			//Set up the dynamic model entity data.
			DynamicModelEntityData data;

			//Set the entity.
			data._Entity = static_cast<DynamicModelEntity *RESTRICT>(entity);

			//Calculate the translation offset.
			const AxisAlignedBoundingBox3D model_space_axis_aligned_bounding_box{ component._ModelResource->_ModelSpaceAxisAlignedBoundingBox };
			AxisAlignedBoundingBox3D scaled_model_space_axis_aligned_bounding_box;
			RenderingUtilities::TransformAxisAlignedBoundingBox(model_space_axis_aligned_bounding_box, Matrix4x4(VectorConstants::ZERO, EulerAngles(), Vector3<float32>(component._CurrentWorldTransform.GetScale())), &scaled_model_space_axis_aligned_bounding_box);
			const Vector3<float32> translation_offset{ -AxisAlignedBoundingBox3D::CalculateCenter(scaled_model_space_axis_aligned_bounding_box) };

			//Create the actor.
			{
				//Create the shape.
				physx::PxShape *RESTRICT shape{ nullptr };

				switch (component._ModelCollisionConfiguration._Type)
				{
					case ModelCollisionType::BOX:
					{
						const AxisAlignedBoundingBox3D local_axis_aligned_bounding_box{ component._WorldSpaceAxisAlignedBoundingBox.GetAbsoluteAxisAlignedBoundingBox() };
						const Vector3<float32> dimensions{ local_axis_aligned_bounding_box.Dimensions() };
						const physx::PxBoxGeometry geometry{ dimensions._X * 0.5f, dimensions._Y * 0.5f, dimensions._Z * 0.5f };

						{
							SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
							shape = PhysXPhysicsSystemData::_Physics->createShape(geometry, *PhysXPhysicsSystemData::_DefaultMaterial);
						}

						break;
					}

					case ModelCollisionType::COLLISION_MODEL:
					{
						physx::PxBase *const RESTRICT mesh{ static_cast<physx::PxBase *const RESTRICT>(component._ModelResource->_CollisionModel) };

						if (mesh->is<physx::PxTriangleMesh>())
						{
							physx::PxTriangleMesh *const RESTRICT triangle_mesh{ static_cast<physx::PxTriangleMesh *const RESTRICT>(component._ModelResource->_CollisionModel) };
							const physx::PxTriangleMeshGeometry geometry{ triangle_mesh };

							{
								SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
								shape = PhysXPhysicsSystemData::_Physics->createShape(geometry, *PhysXPhysicsSystemData::_DefaultMaterial);
							}
						}

						else if (mesh->is<physx::PxConvexMesh>())
						{
							physx::PxConvexMesh *const RESTRICT convex_mesh{ static_cast<physx::PxConvexMesh *const RESTRICT>(component._ModelResource->_CollisionModel) };
							const physx::PxConvexMeshGeometry geometry{ convex_mesh };

							{
								SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
								shape = PhysXPhysicsSystemData::_Physics->createShape(geometry, *PhysXPhysicsSystemData::_DefaultMaterial);
							}
						}

						else
						{
							ASSERT(false, "What happened here?");
						}

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");
					}
				}

				ASSERT(shape, "Couldn't create shape!");

				//Set up the transform.
				const Vector3<float32> absolute_world_position{ component._CurrentWorldTransform.GetAbsolutePosition() };
				const physx::PxVec3 position{ absolute_world_position._X, absolute_world_position._Y, absolute_world_position._Z };
				Quaternion rotation;
				rotation.FromEulerAngles(component._CurrentWorldTransform.GetRotation());
				physx::PxQuat physx_rotation;
				physx_rotation.x = rotation._X;
				physx_rotation.y = rotation._Y;
				physx_rotation.z = rotation._Z;
				physx_rotation.w = rotation._W;
				const physx::PxTransform transform{ position, physx_rotation };

				//Create the actor!
				{
					SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
					data._Actor = PhysXPhysicsSystemData::_Physics->createRigidDynamic(transform);
				}
			
				//Attach the shape.
				data._Actor->attachShape(*shape);

				//Set up the actor.
				physx::PxRigidBodyExt::updateMassAndInertia(*data._Actor, component._ModelSimulationConfiguration._InitialMass);

				if (!component._ModelSimulationConfiguration._InitialVelocity.IsZero())
				{
					const physx::PxVec3 phsyx_initial_velocity{ component._ModelSimulationConfiguration._InitialVelocity._X, component._ModelSimulationConfiguration._InitialVelocity._Y, component._ModelSimulationConfiguration._InitialVelocity._Z };
					data._Actor->setLinearVelocity(phsyx_initial_velocity);
				}

				//Release the shape.
				shape->release();
			}

			//Set the user data.
			data._Actor->userData = entity;

			//Add the actor to the scene.
			{
				SCOPED_LOCK(PhysXPhysicsSystemData::_SceneLock);
				PhysXPhysicsSystemData::_Scene->addActor(*data._Actor);
			}

			//Set the translation offset.
			data._TranslationOffset = translation_offset;

			//Add the dynamic model entity data.
			PhysXPhysicsSystemData::_DynamicModelEntityData.Emplace(data);

			break;
		}

		case EntityType::StaticModel:
		{
			//Cache the component.
			const StaticModelComponent &component{ ComponentManager::GetStaticModelStaticModelComponents()[entity->_ComponentsIndex] };

			//Check if collision is requested.
			if (component._ModelCollisionConfiguration._Type == ModelCollisionType::NONE)
			{
				return;
			}

			//Set up the static model entity data.
			StaticModelEntityData data;

			//Set the entity.
			data._Entity = static_cast<StaticModelEntity *RESTRICT>(entity);

			//Create the actor.
			{
				//Create the shape.
				physx::PxShape *RESTRICT shape{ nullptr };

				switch (component._ModelCollisionConfiguration._Type)
				{
					case ModelCollisionType::BOX:
					{
						const AxisAlignedBoundingBox3D local_axis_aligned_bounding_box{ component._WorldSpaceAxisAlignedBoundingBox.GetAbsoluteAxisAlignedBoundingBox() };
						const Vector3<float32> dimensions{ local_axis_aligned_bounding_box.Dimensions() };
						const physx::PxBoxGeometry geometry{ dimensions._X * 0.5f, dimensions._Y * 0.5f, dimensions._Z * 0.5f };

						{
							SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
							shape = PhysXPhysicsSystemData::_Physics->createShape(geometry, *PhysXPhysicsSystemData::_DefaultMaterial);
						}

						break;
					}

					case ModelCollisionType::COLLISION_MODEL:
					{
						physx::PxBase *const RESTRICT mesh{ static_cast<physx::PxBase *const RESTRICT>(component._ModelResource->_CollisionModel) };

						if (mesh->is<physx::PxTriangleMesh>())
						{
							physx::PxTriangleMesh *const RESTRICT triangle_mesh{ static_cast<physx::PxTriangleMesh *const RESTRICT>(component._ModelResource->_CollisionModel) };
							const physx::PxTriangleMeshGeometry geometry{ triangle_mesh };

							{
								SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
								shape = PhysXPhysicsSystemData::_Physics->createShape(geometry, *PhysXPhysicsSystemData::_DefaultMaterial);
							}
						}

						else if (mesh->is<physx::PxConvexMesh>())
						{
							physx::PxConvexMesh *const RESTRICT convex_mesh{ static_cast<physx::PxConvexMesh *const RESTRICT>(component._ModelResource->_CollisionModel) };
							const physx::PxConvexMeshGeometry geometry{ convex_mesh };

							{
								SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
								shape = PhysXPhysicsSystemData::_Physics->createShape(geometry, *PhysXPhysicsSystemData::_DefaultMaterial);
							}
						}

						else
						{
							ASSERT(false, "What happened here?");
						}

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");
					}
				}

				//Set up the transform.
				const Vector3<float32> absolute_world_position{ component._WorldTransform.GetAbsolutePosition() };
				const physx::PxVec3 position{ absolute_world_position._X, absolute_world_position._Y, absolute_world_position._Z };
				Quaternion rotation;

				rotation.FromEulerAngles(component._WorldTransform.GetRotation());

				const physx::PxQuat physx_rotation{ rotation._X, rotation._Y, rotation._Z, rotation._W };
				const physx::PxTransform transform{ position, physx_rotation };

				//Create the actor!
				{
					SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
					data._Actor = PhysXPhysicsSystemData::_Physics->createRigidStatic(transform);
				}

				//Attach the shape.
				data._Actor->attachShape(*shape);

				//Release the shape.
				shape->release();
			}

			//Set the user data.
			data._Actor->userData = entity;

			//Add the actor to the scene.
			{
				SCOPED_LOCK(PhysXPhysicsSystemData::_SceneLock);
				PhysXPhysicsSystemData::_Scene->addActor(*data._Actor);
			}

			//Add the dynamic model entity data.
			PhysXPhysicsSystemData::_StaticModelEntityData.Emplace(data);

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
*	Updates the sub-system world transform for the given entity.
*/
void PhysicsSystem::SubUpdateEntityWorldTransform(Entity *const RESTRICT entity, const WorldTransform &world_transform) NOEXCEPT
{
	//Don't update world transforms while physics is being updated.
	if (PhysXPhysicsSystemData::_PhysicsIsUpdating)
	{
		return;
	}

	//Which type is this entity?
	switch (entity->_Type)
	{
		case EntityType::DynamicModel:
		{
			//Find the actor.
			physx::PxActor* RESTRICT actor{ nullptr };

			for (const DynamicModelEntityData &data : PhysXPhysicsSystemData::_DynamicModelEntityData)
			{
				if (data._Entity == entity)
				{
					actor = data._Actor;

					break;
				}
			}

			ASSERT(actor, "Couldn't find the actor for this entity!");

			//Set up the PhysX transform.
			const Vector3<float32> absolute_world_position{ world_transform.GetAbsolutePosition() };
			const physx::PxVec3 physx_position{ absolute_world_position._X, absolute_world_position._Y, absolute_world_position._Z };
			Quaternion rotation;
			rotation.FromEulerAngles(world_transform.GetRotation());
			const physx::PxQuat physx_rotation{ rotation._X, rotation._Y, rotation._Z, rotation._W };
			const physx::PxTransform physx_transform{ physx_position, physx_rotation };

			//Set the global pose.
			static_cast<physx::PxRigidBody *const RESTRICT>(actor)->setGlobalPose(physx_transform);

			break;
		}

		case EntityType::StaticModel:
		{
			//Find the actor.
			physx::PxActor* RESTRICT actor{ nullptr };

			for (const StaticModelEntityData &data : PhysXPhysicsSystemData::_StaticModelEntityData)
			{
				if (data._Entity == entity)
				{
					actor = data._Actor;

					break;
				}
			}

			ASSERT(actor, "Couldn't find the actor for this entity!");

			//Set up the PhysX transform.
			const Vector3<float32> absolute_world_position{ world_transform.GetAbsolutePosition() };
			const physx::PxVec3 physx_position{ absolute_world_position._X, absolute_world_position._Y, absolute_world_position._Z };
			Quaternion rotation;
			rotation.FromEulerAngles(world_transform.GetRotation());
			const physx::PxQuat physx_rotation{ rotation._X, rotation._Y, rotation._Z, rotation._W };
			const physx::PxTransform physx_transform{ physx_position, physx_rotation };

			//Set the global pose.
			static_cast<physx::PxRigidBody *const RESTRICT>(actor)->setGlobalPose(physx_transform);

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
*	Terminates the sub-system physics for the given entity.
*/
void PhysicsSystem::SubTerminateEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT
{
	//Which type is this entity?
	switch (entity->_Type)
	{
		case EntityType::DynamicModel:
		{
			//Cast to the type.
			DynamicModelEntity *const RESTRICT dynamic_model_entity{ static_cast<DynamicModelEntity *const RESTRICT>(entity) };

			//Find the dynamic model entity data.
			for (uint64 i{ 0 }, size{ PhysXPhysicsSystemData::_DynamicModelEntityData.Size() }; i < size; ++i)
			{
				if (PhysXPhysicsSystemData::_DynamicModelEntityData[i]._Entity == entity)
				{
					//Remove the actor from the scene.
					{
						SCOPED_LOCK(PhysXPhysicsSystemData::_SceneLock);
						PhysXPhysicsSystemData::_Scene->removeActor(*PhysXPhysicsSystemData::_DynamicModelEntityData[i]._Actor);
					}

					//Release the actor.
					PhysXPhysicsSystemData::_DynamicModelEntityData[i]._Actor->release();

					//Remove the dynamic model entity data.
					PhysXPhysicsSystemData::_DynamicModelEntityData.EraseAt<false>(i);

					break;
				}
			}

			break;
		}

		case EntityType::StaticModel:
		{
			//Cast to the type.
			StaticModelEntity *const RESTRICT static_model_entity{ static_cast<StaticModelEntity *const RESTRICT>(entity) };

			//Find the static model entity data.
			for (uint64 i{ 0 }, size{ PhysXPhysicsSystemData::_StaticModelEntityData.Size() }; i < size; ++i)
			{
				if (PhysXPhysicsSystemData::_StaticModelEntityData[i]._Entity == entity)
				{
					//Remove the actor from the scene.
					{
						SCOPED_LOCK(PhysXPhysicsSystemData::_SceneLock);
						PhysXPhysicsSystemData::_Scene->removeActor(*PhysXPhysicsSystemData::_StaticModelEntityData[i]._Actor);
					}

					//Release the actor.
					PhysXPhysicsSystemData::_StaticModelEntityData[i]._Actor->release();

					//Remove the static model entity data.
					PhysXPhysicsSystemData::_StaticModelEntityData.EraseAt<false>(i);

					break;
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
*	Casts a sub-system ray.
*/
void PhysicsSystem::SubCastRay(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT
{
	//Construct the PhysX properties.
	const physx::PxVec3 physx_origin{ ray._Origin._X, ray._Origin._Y, ray._Origin._Z };
	const physx::PxVec3 physx_direction{ ray._Direction._X, ray._Direction._Y, ray._Direction._Z };
	physx::PxRaycastBuffer raycast_buffer;
	const physx::PxHitFlags flags{ 0 };
	physx::PxQueryFilterData filter_data;
	QueryFilterCallback query_filter_callback{ configuration };

	filter_data.flags |= physx::PxQueryFlag::ePREFILTER;

	//Cast the ray!
	{
		SCOPED_LOCK(PhysXPhysicsSystemData::_SceneLock);
		result->_HasHit = PhysXPhysicsSystemData::_Scene->raycast(	physx_origin,
																	physx_direction,
																	configuration._MaximumHitDistance,
																	raycast_buffer,
																	flags,
																	filter_data,
																	&query_filter_callback);
	}

	//Fill out the result.
	if (result->_HasHit)
	{
		result->_HitDistance = raycast_buffer.block.distance;

		if (raycast_buffer.block.actor->userData)
		{
			Entity *const RESTRICT entity{ static_cast<Entity *const RESTRICT>(raycast_buffer.block.actor->userData) };

			switch (entity->_Type)
			{
				case EntityType::DynamicModel:
				{
					result->_Type = RaycastResult::Type::DYNAMIC_MODEL;
					result->_DynamicModelRaycastResult._Entity = entity;

					break;
				}

				case EntityType::StaticModel:
				{
					result->_Type = RaycastResult::Type::STATIC_MODEL;
					result->_StaticModelRaycastResult._Entity = entity;

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		else
		{
			result->_Type = RaycastResult::Type::NONE;
		}
	}

	else
	{
		result->_Type = RaycastResult::Type::NONE;
	}
}

/*
*	Adds an sub-system impulse at the given world position with the given force.
*/
void PhysicsSystem::SubAddImpulse(const WorldPosition &world_position, const float32 force) NOEXCEPT
{
	//Calculate the PhysX position.
	const Vector3<float32> absolute_world_position{ world_position.GetAbsolutePosition() };
	const physx::PxVec3 physx_position{ absolute_world_position._X, absolute_world_position._Y, absolute_world_position._Z };

	//Apply impulses to all dynamic models.
	for (const DynamicModelEntityData &data : PhysXPhysicsSystemData::_DynamicModelEntityData)
	{
		//Calculate the force magnitude.
		const Vector3<float32> force_magnitude{ Vector3<float32>::Normalize(data._Entity->GetWorldTransform()->GetAbsolutePosition() - absolute_world_position) * force };
		const physx::PxVec3 physx_force_magnitude{ force_magnitude._X, force_magnitude._Y, force_magnitude._Z };

		physx::PxRigidBodyExt::addForceAtPos(*data._Actor, physx_force_magnitude, physx_position, physx::PxForceMode::Enum::eIMPULSE);
	}
}

/*
*	Creates a sub-system character controller.
*/
RESTRICTED NO_DISCARD CharacterController *const RESTRICT PhysicsSystem::SubCreateCharacterController(const CharacterControllerConfiguration &configuration) NOEXCEPT
{
	//Set up the abstraction data.
	Any<CharacterController::ABSTRACTION_DATA_SIZE> abstraction_data;

	//Create the controller.
	{
		physx::PxCapsuleControllerDesc description;

		description.setToDefault();

		description.stepOffset = 0.25f;
		description.material = PhysXPhysicsSystemData::_DefaultMaterial;
		description.radius = configuration._CapsuleRadius;
		description.height = configuration._CapsuleHeight;
		description.climbingMode = physx::PxCapsuleClimbingMode::Enum::eCONSTRAINED;

		ASSERT(description.isValid(), "Description is invalid!");

		abstraction_data.Get<PhysXCharacterControllerAbstractionData>()->_Controller = PhysXPhysicsSystemData::_ControllerManager->createController(description);
	}

	return new (MemorySystem::Instance->TypeAllocate<CharacterController>()) CharacterController(abstraction_data);
}

/*
*	Builds a collision model on the sub-system.
*/
void PhysicsSystem::SubBuildCollisionModel(const ModelFile &model_file, CollisionModelData *const RESTRICT collision_model_data) NOEXCEPT
{
	//Cooking needs to be initialized, so initialize now.
	Initialize();

	//First of all, check the convexity of the model.
	bool is_convex;

	{
		//Calculate the number of vertices.
		uint64 number_of_vertices{ 0 };

		for (const ModelFile::Mesh &mesh : model_file._Meshes)
		{
			number_of_vertices += mesh._Vertices.Size();
		}

		//Calculate the number of indices.
		uint64 number_of_indices{ 0 };

		for (const ModelFile::Mesh &mesh : model_file._Meshes)
		{
			number_of_indices += mesh._Indices.Size();
		}

		//Fill up the points/indices.
		DynamicArray<Vector3<float32>> points;
		DynamicArray<uint32> indices;

		points.Reserve(number_of_vertices);
		indices.Reserve(number_of_indices);

		for (const ModelFile::Mesh &mesh : model_file._Meshes)
		{
			for (const Vertex &vertex : mesh._Vertices)
			{
				points.Emplace(vertex._Position);
			}

			for (const uint32 index : mesh._Indices)
			{
				indices.Emplace(index);
			}
		}

		//Check the convexity.
		is_convex = PhysicsUtilities::IsConvexSetOfPoints(points, indices);
	}

	//Try to build a convex mesh.
	if (is_convex)
	{
		//Calculate the number of vertices.
		uint64 number_of_vertices{ 0 };

		for (const ModelFile::Mesh &mesh : model_file._Meshes)
		{
			number_of_vertices += mesh._Vertices.Size();
		}

		//Create the convex mesh description.
		physx::PxConvexMeshDesc convex_mesh_description;

		convex_mesh_description.points.count = static_cast<uint32>(number_of_vertices);
		convex_mesh_description.points.stride = sizeof(physx::PxVec3);

		DynamicArray<physx::PxVec3> points;
		points.Reserve(number_of_vertices);

		for (const ModelFile::Mesh &mesh : model_file._Meshes)
		{
			for (const Vertex &vertex : mesh._Vertices)
			{
				points.Emplace(physx::PxVec3(vertex._Position._X, vertex._Position._Y, vertex._Position._Z));
			}
		}

		convex_mesh_description.points.data = points.Data();
		convex_mesh_description.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		//Build the convex mesh.
		physx::PxDefaultMemoryOutputStream output_stream_buffer;
		physx::PxConvexMeshCookingResult::Enum result;

		{
			SCOPED_LOCK(PhysXPhysicsSystemData::_CookingLock);

			PhysXPhysicsSystemData::_Cooking->cookConvexMesh(convex_mesh_description, output_stream_buffer, &result);
		}

		ASSERT(result == physx::PxConvexMeshCookingResult::eSUCCESS, "Convex mesh building failed!");

		//Set the type.
		collision_model_data->_Type = CollisionModelData::Type::CONVEX;

		//Set the data.
		collision_model_data->_Data.Upsize<false>(static_cast<uint64>(output_stream_buffer.getSize()));
		Memory::Copy(collision_model_data->_Data.Data(), output_stream_buffer.getData(), sizeof(byte) * output_stream_buffer.getSize());
	}

	else
	{
		//Calculate the number of vertices/indices.
		uint64 number_of_vertices{ 0 };
		uint64 number_of_indices{ 0 };

		for (const ModelFile::Mesh &mesh : model_file._Meshes)
		{
			number_of_vertices += mesh._Vertices.Size();
			number_of_indices += mesh._Indices.Size();
		}

		//Gather the vertices/indices.
		DynamicArray<physx::PxVec3> vertices;
		DynamicArray<physx::PxU32> indices;

		vertices.Reserve(number_of_vertices);
		indices.Reserve(number_of_vertices);

		for (const ModelFile::Mesh &mesh : model_file._Meshes)
		{
			for (const Vertex &vertex : mesh._Vertices)
			{
				vertices.Emplace(physx::PxVec3(vertex._Position._X, vertex._Position._Y, vertex._Position._Z));
			}

			for (const uint32 index : mesh._Indices)
			{
				indices.Emplace(index);
			}
		}

		//Create the convex mesh description.
		physx::PxTriangleMeshDesc triangle_mesh_description;

		triangle_mesh_description.points.count = static_cast<uint32>(number_of_vertices);
		triangle_mesh_description.points.stride = sizeof(physx::PxVec3);
		triangle_mesh_description.points.data = vertices.Data();

		triangle_mesh_description.triangles.count = static_cast<uint32>(number_of_indices);
		triangle_mesh_description.triangles.stride = sizeof(physx::PxU32) * 3;
		triangle_mesh_description.triangles.data = indices.Data();

		triangle_mesh_description.flags = static_cast<physx::PxMeshFlag::Enum>(0);

		ASSERT(triangle_mesh_description.isValid(), "Triangle mesh description isn't valid!");

		//Build the convex mesh.
		physx::PxDefaultMemoryOutputStream output_stream_buffer;
		physx::PxTriangleMeshCookingResult::Enum result;

		{
			SCOPED_LOCK(PhysXPhysicsSystemData::_CookingLock);

			PhysXPhysicsSystemData::_Cooking->cookTriangleMesh(triangle_mesh_description, output_stream_buffer, &result);
		}

		ASSERT(result == physx::PxTriangleMeshCookingResult::eSUCCESS, "Concave mesh building failed!");

		//Set the type.
		collision_model_data->_Type = CollisionModelData::Type::CONCAVE;

		//Set the data.
		collision_model_data->_Data.Upsize<false>(static_cast<uint64>(output_stream_buffer.getSize()));
		Memory::Copy(collision_model_data->_Data.Data(), output_stream_buffer.getData(), sizeof(byte) * output_stream_buffer.getSize());
	}
}
#endif