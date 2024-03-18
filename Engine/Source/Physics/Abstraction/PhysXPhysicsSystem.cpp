#if defined(CATALYST_PHYSICS_PHYSX)
//Header file.
#include <Systems/PhysicsSystem.h>

//Concurrency.
#include <Concurrency/Task.h>

//Physics.
#include <Physics/Abstraction/PhysXCharacterControllerAbstractionData.h>
#include <Physics/Native/PhysicsUtilities.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/TaskSystem.h>

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
		//const bool is_dynamic_model{ actor->userData && static_cast<Entity* const RESTRICT>(actor->userData)->_Type == EntityType::DynamicModel };
		const bool is_dynamic_model{ false };
		//const bool is_static_model{ actor->userData && static_cast<Entity *const RESTRICT>(actor->userData)->_Type == EntityType::StaticModel };
		const bool is_static_model{ false };

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

//PhysX physics system constants.
namespace PhysXPhysicsSystemConstants
{

	//The update frequency.
	constexpr float32 UPDATE_FREQUENCY{ 1.0f / 60.0f };

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

	//The time since last update.
	float32 _TimeSinceLastUpdate{ 0.0f };

	//Denotes if physics is being updated.
	bool _PhysicsIsUpdating{ false };

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
*	Creates a height field actor on the sub-system.
*/
void PhysicsSystem::SubCreateHeightFieldActor
(
	const WorldPosition &world_position,
	const Texture2D<float32> &height_field,
	ActorHandle *const RESTRICT actor_handle
) NOEXCEPT
{
	//Cache some properties.
	const float32 patch_size{ static_cast<float32>(height_field.GetResolution()) };
	const float32 half_patch_size{ patch_size * 0.5f };

	//Calculate the terrain patch position.
	const Vector3<float32> terrain_patch_position{ world_position.GetAbsolutePosition() };

	//Create the shape.
	physx::PxShape *RESTRICT shape{ nullptr };

	{
		//Calculate the height scale.
		float32 maximum_height{ -FLOAT32_MAXIMUM };

		for (uint32 Y{ 0 }; Y < height_field.GetResolution(); ++Y)
		{
			for (uint32 X{ 0 }; X < height_field.GetResolution(); ++X)
			{
				maximum_height = CatalystBaseMath::Maximum<float32>(maximum_height, height_field.At(X, Y));
			}
		}

		const float32 height_scale{ maximum_height / static_cast<float32>(INT16_MAXIMUM) };

		//Construct the height field samples.
		Texture2D<physx::PxHeightFieldSample> height_field_samples{ height_field.GetResolution() };
		Memory::Set(height_field_samples.Data(), 0, sizeof(physx::PxHeightFieldSample) * height_field.GetResolution() * height_field.GetResolution());

		for (uint32 Y{ 0 }; Y < height_field.GetResolution(); ++Y)
		{
			for (uint32 X{ 0 }; X < height_field.GetResolution(); ++X)
			{
				height_field_samples.At(Y, X).height = static_cast<int16>(height_field.At(X, Y) / height_scale);
			}
		}

		//Create the height field.
		physx::PxHeightFieldDesc height_field_description;

		height_field_description.setToDefault();

		height_field_description.nbRows = height_field.GetResolution();
		height_field_description.nbColumns = height_field.GetResolution();
		height_field_description.format = physx::PxHeightFieldFormat::eS16_TM;
		height_field_description.samples.data = height_field_samples.Data();
		height_field_description.samples.stride = sizeof(physx::PxHeightFieldSample);
		height_field_description.flags = physx::PxHeightFieldFlag::eNO_BOUNDARY_EDGES;

		physx::PxPhysicsInsertionCallback *RESTRICT physics_insertion_callback;

		{
			SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
			physics_insertion_callback = &PhysXPhysicsSystemData::_Physics->getPhysicsInsertionCallback();
		}

		physx::PxHeightField *RESTRICT _height_field;

		{
			SCOPED_LOCK(PhysXPhysicsSystemData::_CookingLock);

			_height_field = PhysXPhysicsSystemData::_Cooking->createHeightField(height_field_description, *physics_insertion_callback);
		}

		//Create the height field geometry.
		const physx::PxHeightFieldGeometry height_field_geometry{ _height_field, physx::PxMeshGeometryFlags(), height_scale, 1.0f + (1.0f / static_cast<float32>(height_field.GetResolution())), 1.0f + (1.0f / static_cast<float32>(height_field.GetResolution())) };

		//Create the new terrain shape.
		{
			SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
			shape = PhysXPhysicsSystemData::_Physics->createShape(height_field_geometry, *PhysXPhysicsSystemData::_DefaultMaterial, true);
		}

		//Decrement the reference count of the height field, so that it gets automatically release with the shape later.
		_height_field->release();
	}

	//Create the actor.
	physx::PxRigidStatic* RESTRICT actor{ nullptr };

	{
		const physx::PxTransform terrain_transform{ physx::PxVec3(terrain_patch_position._X - half_patch_size, 0.0f, terrain_patch_position._Z - half_patch_size) };

		{
			SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
			actor = PhysXPhysicsSystemData::_Physics->createRigidStatic(terrain_transform);
		}
	}

	//Attach the new shape.
	actor->attachShape(*shape);

	//Decrement the reference count of the shape, so that it gets automatically release with the actor later.
	shape->release();

	//Add the actor to the scene.
	{
		SCOPED_LOCK(PhysXPhysicsSystemData::_SceneLock);
		PhysXPhysicsSystemData::_Scene->addActor(*actor);
	}

	//Set the actor handle.
	*actor_handle = actor;
}

/*
*	Creates a model actor on the sub-system.
*/
void PhysicsSystem::SubCreateModelActor
(
	const WorldTransform &world_transform,
	const ModelCollisionType collision_type,
	const WorldSpaceAxisAlignedBoundingBox3D &world_space_axis_aligned_bounding_box,
	const CollisionModelHandle collision_model,
	const ModelSimulationConfiguration &simulation_configuration,
	ActorHandle *const RESTRICT actor_handle
) NOEXCEPT
{
	ASSERT(collision_type != ModelCollisionType::NONE, "Trying to create an actor with no collision!");

	//Create the shape.
	physx::PxShape *RESTRICT shape{ nullptr };

	switch (collision_type)
	{
		case ModelCollisionType::BOX:
		{
			const AxisAlignedBoundingBox3D local_axis_aligned_bounding_box{ world_space_axis_aligned_bounding_box.GetAbsoluteAxisAlignedBoundingBox() };
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
			//Cache the mesh.
			physx::PxBase *const RESTRICT mesh{ static_cast<physx::PxBase *const RESTRICT>(collision_model) };

			if (mesh->is<physx::PxTriangleMesh>())
			{
				physx::PxTriangleMesh *const RESTRICT triangle_mesh{ static_cast<physx::PxTriangleMesh *const RESTRICT>(collision_model) };
				const physx::PxTriangleMeshGeometry geometry{ triangle_mesh };

				{
					SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
					shape = PhysXPhysicsSystemData::_Physics->createShape(geometry, *PhysXPhysicsSystemData::_DefaultMaterial);
				}
			}

			else if (mesh->is<physx::PxConvexMesh>())
			{
				physx::PxConvexMesh *const RESTRICT convex_mesh{ static_cast<physx::PxConvexMesh *const RESTRICT>(collision_model) };
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

			break;
		}
	}

	//Set the shape flags.
	shape->setFlags(physx::PxShapeFlag::eSIMULATION_SHAPE | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eVISUALIZATION);

	//Set up the transform.
	const Vector3<float32> absolute_world_position{ world_transform.GetAbsolutePosition() };
	const physx::PxVec3 position{ absolute_world_position._X, absolute_world_position._Y, absolute_world_position._Z };
	const Quaternion rotation{ world_transform.GetRotation() };
	physx::PxQuat physx_rotation;
	physx_rotation.x = rotation._X;
	physx_rotation.y = rotation._Y;
	physx_rotation.z = rotation._Z;
	physx_rotation.w = rotation._W;
	const physx::PxTransform transform{ position, physx_rotation };

	//Create the actor!
	physx::PxRigidActor *RESTRICT actor{ nullptr };

	if (simulation_configuration._SimulatePhysics)
	{
		SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
		actor = PhysXPhysicsSystemData::_Physics->createRigidDynamic(transform);
	}
	
	else
	{
		SCOPED_LOCK(PhysXPhysicsSystemData::_PhysicsLock);
		actor = PhysXPhysicsSystemData::_Physics->createRigidStatic(transform);
	}

	//Attach the shape.
	actor->attachShape(*shape);

	//Set up the actor simulation.
	if (simulation_configuration._SimulatePhysics)
	{
		physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic *const RESTRICT>(actor), simulation_configuration._InitialMass);

		if (!simulation_configuration._InitialVelocity.IsZero())
		{
			const physx::PxVec3 phsyx_initial_velocity{ simulation_configuration._InitialVelocity._X, simulation_configuration._InitialVelocity._Y, simulation_configuration._InitialVelocity._Z };
			static_cast<physx::PxRigidDynamic *const RESTRICT>(actor)->setLinearVelocity(phsyx_initial_velocity);
		}
	}

	//Release the shape.
	shape->release();

	//Add the actor to the scene.
	{
		SCOPED_LOCK(PhysXPhysicsSystemData::_SceneLock);
		PhysXPhysicsSystemData::_Scene->addActor(*actor);
	}

	//Set the actor handle.
	*actor_handle = actor;
}

/*
*	Destroys an actor on the sub-system.
*/
void PhysicsSystem::SubDestroyActor(ActorHandle *const RESTRICT actor_handle) NOEXCEPT
{
	//Cache the actor.
	physx::PxActor *const RESTRICT actor{ static_cast<physx::PxActor *const RESTRICT>(*actor_handle) };

	//Remove the actor from the scene.
	{
		SCOPED_LOCK(PhysXPhysicsSystemData::_SceneLock);
		PhysXPhysicsSystemData::_Scene->removeActor(*actor);
	}

	//Release the actor.
	actor->release();

	//Reset the actor handle.
	*actor_handle = nullptr;
}

/*
*	Returns the world transform for the given actor on the sub-system.
*/
void PhysicsSystem::SubGetActorWorldTransform(const ActorHandle actor_handle, WorldTransform *const RESTRICT world_transform) NOEXCEPT
{
	//Cache the actor.
	physx::PxRigidActor *const RESTRICT actor{ static_cast<physx::PxRigidActor *const RESTRICT>(actor_handle) };

	//Retrieve the shape.
	physx::PxShape *RESTRICT shape;
	actor->getShapes(&shape, 1);
	
	//Calculate the transform.
	const physx::PxTransform transform{ actor->getGlobalPose() * shape->getLocalPose() };

	//Fill in the world transform.
	world_transform->SetAbsolutePosition(*static_cast<const Vector3<float32> *const RESTRICT>(static_cast<const void *const RESTRICT>(&transform.p)));
	world_transform->SetRotation(*static_cast<const Quaternion* const RESTRICT>(static_cast<const void* const RESTRICT>(&transform.q)));
	world_transform->SetScale(1.0f);
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

		/*
		if (raycast_buffer.block.actor->userData)
		{
			Entity *const RESTRICT entity{ static_cast<Entity *const RESTRICT>(raycast_buffer.block.actor->userData) };

			switch (entity->_Type)
			{
				case EntityType::Terrain:
				{
					result->_Type = RaycastResult::Type::TERRAIN;
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
		*/
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