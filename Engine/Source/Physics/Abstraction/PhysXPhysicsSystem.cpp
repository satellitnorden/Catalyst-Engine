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
	constexpr uint32 TERRAIN_HEIGHT_FIELD_HALF_SIZE{ TERRAIN_HEIGHT_FIELD_SIZE >> 1 };

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

	//The physics.
	physx::PxPhysics *RESTRICT _Physics{ nullptr };
	
	//The cooking.
	physx::PxCooking *RESTRICT _Cooking{ nullptr };

	//The dispatcher.
	physx::PxDefaultCpuDispatcher *RESTRICT _Dispatcher{ nullptr };

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

	//The terrain actor.
	physx::PxRigidStatic *RESTRICT _TerrainActor{ nullptr };

	//The new terrain shape.
	physx::PxShape *RESTRICT _NewTerrainShape{ nullptr };

	//The new terrain position.
	physx::PxVec3 _NewTerrainPosition{ 0.0f, 0.0f, 0.0f };

}

#include <File/Writers/PNGWriter.h>

//PhysX physics system logic.
namespace PhysXPhysicsSystemLogic
{

	/*
	*	Moves the terrain height field.
	*/
	FORCE_INLINE void MoveTerrainHeightField() NOEXCEPT
	{
		//If terrain can't be generated, just return. (:
		if (!TerrainSystem::Instance->CanTerrainBeGenerated())
		{
			return;
		}

		//Cache the perceiver current world position.
		Vector3<float32> perceiver_world_position{ Perceiver::Instance->GetWorldTransform().GetWorldPosition().GetAbsolutePosition() };

		//Round the position.
		perceiver_world_position._X = CatalystBaseMath::Round<float32>(perceiver_world_position._X);
		perceiver_world_position._Y = 0.0f;
		perceiver_world_position._Z = CatalystBaseMath::Round<float32>(perceiver_world_position._Z);

		//Construct the height map, whilst remembering the highest height.
		Texture2D<float32> height_map{ PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE };
		float32 highest_height{ -FLOAT32_MAXIMUM };

		for (uint32 Y{ 0 }; Y < PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE; ++Y)
		{
			for (uint32 X{ 0 }; X < PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE; ++X)
			{
				//Calculate the sample world position.
				const Vector3<float32> sample_world_position{	perceiver_world_position._X + static_cast<float32>(static_cast<int32>(X) - static_cast<int32>(PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_HALF_SIZE)),
																0.0f,
																perceiver_world_position._Z + static_cast<float32>(static_cast<int32>(Y) - static_cast<int32>(PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_HALF_SIZE)) };
			
				//Retrieve the terrain height.
				float32 terrain_height;
				TerrainSystem::Instance->GetTerrainHeightAtPosition(sample_world_position, &terrain_height);

				//Write the terrain height.
				height_map.At(X, Y) = terrain_height;
				highest_height = CatalystBaseMath::Maximum(highest_height, CatalystBaseMath::Absolute(terrain_height));
			}
		}

		/*
		//Write the height map, for debug purposes.
		{
			Texture2D<Vector4<float32>> export_height_map{ PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE };

			for (uint32 Y{ 0 }; Y < PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE; ++Y)
			{
				for (uint32 X{ 0 }; X < PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE; ++X)
				{
					const float32 height{ CatalystBaseMath::Scale(height_map.At(X, Y), -highest_height, highest_height, 0.0f, 1.0f) };
					export_height_map.At(X, Y) = Vector4<float32>(height, height, height, 1.0f);
				}
			}

			PNGWriter::Write(export_height_map, "Exported_Height_Map.png");
		}
		*/

		//Calculate the height scale.
		const float32 height_scale{ highest_height / static_cast<float32>(INT16_MAXIMUM) };

		//Construct the height field samples.
		Texture2D<physx::PxHeightFieldSample> height_field_samples{ PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE };
		Memory::Set(height_field_samples.Data(), 0, sizeof(physx::PxHeightFieldSample) * PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE * PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE);

		for (uint32 Y{ 0 }; Y < PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE; ++Y)
		{
			for (uint32 X{ 0 }; X < PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_SIZE; ++X)
			{
				height_field_samples.At(X, Y).height = static_cast<int16>(height_map.At(X, Y) / height_scale);
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

		physx::PxHeightField *const RESTRICT height_field{ PhysXPhysicsSystemData::_Cooking->createHeightField(height_field_description, PhysXPhysicsSystemData::_Physics->getPhysicsInsertionCallback()) };
	
		//Create the height field geometry.
		const physx::PxHeightFieldGeometry height_field_geometry{ height_field, physx::PxMeshGeometryFlags(), height_scale, 1.0f, 1.0f };

		//Create the new terrain shape.
		PhysXPhysicsSystemData::_NewTerrainShape = PhysXPhysicsSystemData::_Physics->createShape(height_field_geometry, *PhysXPhysicsSystemData::_DefaultMaterial, true);
	
		//Set the new terrain position.
		PhysXPhysicsSystemData::_NewTerrainPosition = physx::PxVec3(perceiver_world_position._X - static_cast<float32>(PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_HALF_SIZE), 0.0f, -perceiver_world_position._Z - static_cast<float32>(PhysXPhysicsSystemConstants::TERRAIN_HEIGHT_FIELD_HALF_SIZE));
	
		//Decrement the reference count of the height field, so that it gets automatically release with the shape later.
		height_field->release();
	}

	/*
	*	Updates the terrain height field.
	*/
	FORCE_INLINE void UpdateTerrainHeightField() NOEXCEPT
	{
		//Is there a new terrain shape?
		if (PhysXPhysicsSystemData::_NewTerrainShape)
		{
			//Create the actor, if it doesn't exist.
			if (!PhysXPhysicsSystemData::_TerrainActor)
			{
				const physx::PxTransform terrain_transform{ PhysXPhysicsSystemData::_NewTerrainPosition };
				PhysXPhysicsSystemData::_TerrainActor = PhysXPhysicsSystemData::_Physics->createRigidStatic(terrain_transform);
				PhysXPhysicsSystemData::_Scene->addActor(*PhysXPhysicsSystemData::_TerrainActor);
			}

			//Detach the current shape(s).
			const uint32 number_of_shapes{ PhysXPhysicsSystemData::_TerrainActor->getNbShapes() };

			if (number_of_shapes > 0)
			{
				ASSERT(number_of_shapes == 1, "The terrain actor should ever just have one shape!");

				StaticArray<physx::PxShape *RESTRICT, 1> shapes;
				PhysXPhysicsSystemData::_TerrainActor->getShapes(shapes.Data(), number_of_shapes);

				for (uint32 i{ 0 }; i < number_of_shapes; ++i)
				{
					PhysXPhysicsSystemData::_TerrainActor->detachShape(*shapes[i]);
				}
			}

			//Attach the new shape.
			PhysXPhysicsSystemData::_TerrainActor->attachShape(*PhysXPhysicsSystemData::_NewTerrainShape);

			//Update the transform.
			physx::PxTransform new_transform{ PhysXPhysicsSystemData::_NewTerrainPosition };
			PhysXPhysicsSystemData::_TerrainActor->setGlobalPose(new_transform);

			//Reset the new terrain shape.
			PhysXPhysicsSystemData::_NewTerrainShape->release();
			PhysXPhysicsSystemData::_NewTerrainShape = nullptr;
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

		PhysXPhysicsSystemData::_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *PhysXPhysicsSystemData::_Foundation, cooking_paramters);
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
		PhysXPhysicsSystemLogic::MoveTerrainHeightField();
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

	//Create the convex mesh.
	*collision_model = PhysXPhysicsSystemData::_Physics->createConvexMesh(memory_input_data);
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

		//Simulate the scene.
		PhysXPhysicsSystemData::_Scene->simulate(PhysXPhysicsSystemConstants::UPDATE_FREQUENCY);

		//Fetch the results.
		PhysXPhysicsSystemData::_Scene->fetchResults(true);

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

				//PhysX uses a right handed coordinate system, while the Catalyst engine uses a left handed coordinate system, so transform between those.
				position._Z = -position._Z;
				rotation._Z = -rotation._Z;
			}

			//Set up the new world transform.
			WorldTransform new_world_transform;

			new_world_transform.SetAbsolutePosition(position);

			const EulerAngles euler_angles{ rotation.ToEulerAngles() };
			new_world_transform.SetRotation(-euler_angles);

			//Set the world transform of the entity.
			data._Entity->SetWorldTransform(new_world_transform);
		}
	}

	//See if the terrain generation task is done.
	if (PhysXPhysicsSystemData::_TerrainGenerationTask.IsExecuted())
	{
		//Update the terrain.
		PhysXPhysicsSystemLogic::UpdateTerrainHeightField();

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
						const AxisAlignedBoundingBox3D local_axis_aligned_bounding_box{ component._WorldSpaceAxisAlignedBoundingBox };
						const Vector3<float32> dimensions{ local_axis_aligned_bounding_box.Dimensions() };
						const physx::PxBoxGeometry geometry{ dimensions._X * 0.5f, dimensions._Y * 0.5f, dimensions._Z * 0.5f };
						shape = PhysXPhysicsSystemData::_Physics->createShape(geometry, *PhysXPhysicsSystemData::_DefaultMaterial);

						break;
					}

					case ModelCollisionType::COLLISION_MODEL:
					{
						physx::PxConvexMesh *const RESTRICT convex_mesh{ static_cast<physx::PxConvexMesh *const RESTRICT>(component._ModelResource->_CollisionModel) };
						const physx::PxConvexMeshGeometry geometry{ convex_mesh };
						shape = PhysXPhysicsSystemData::_Physics->createShape(geometry, *PhysXPhysicsSystemData::_DefaultMaterial);

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
				const physx::PxVec3 position{ absolute_world_position._X, absolute_world_position._Y, -absolute_world_position._Z };
				Quaternion rotation;
				rotation.FromEulerAngles(component._CurrentWorldTransform.GetRotation());
				physx::PxQuat physx_rotation;
				physx_rotation.x = rotation._X;
				physx_rotation.y = rotation._Y;
				physx_rotation.z = -rotation._Z;
				physx_rotation.w = rotation._W;
				const physx::PxTransform transform{ position, physx_rotation };

				//Create the actor!
				data._Actor = PhysXPhysicsSystemData::_Physics->createRigidDynamic(transform);
			
				//Attach the shape.
				data._Actor->attachShape(*shape);

				//Set up the actor.
				physx::PxRigidBodyExt::updateMassAndInertia(*data._Actor, component._ModelSimulationConfiguration._InitialMass);

				if (!component._ModelSimulationConfiguration._InitialVelocity.IsZero())
				{
					const physx::PxVec3 phsyx_initial_velocity{ component._ModelSimulationConfiguration._InitialVelocity._X, component._ModelSimulationConfiguration._InitialVelocity._Y, -component._ModelSimulationConfiguration._InitialVelocity._Z };
					data._Actor->setLinearVelocity(phsyx_initial_velocity);
				}

				//Release the shape.
				shape->release();
			}

			//Set the user data.
			data._Actor->userData = entity;

			//Add the actor to the scene.
			PhysXPhysicsSystemData::_Scene->addActor(*data._Actor);

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
						shape = PhysXPhysicsSystemData::_Physics->createShape(geometry, *PhysXPhysicsSystemData::_DefaultMaterial);

						break;
					}

					case ModelCollisionType::COLLISION_MODEL:
					{
						physx::PxConvexMesh *const RESTRICT convex_mesh{ static_cast<physx::PxConvexMesh *const RESTRICT>(component._ModelResource->_CollisionModel) };
						const physx::PxConvexMeshGeometry geometry{ convex_mesh };
						shape = PhysXPhysicsSystemData::_Physics->createShape(geometry, *PhysXPhysicsSystemData::_DefaultMaterial);

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");
					}
				}
				//Set up the transform.
				const Vector3<float32> absolute_world_position{ component._WorldTransform.GetAbsolutePosition() };
				const physx::PxVec3 position{ absolute_world_position._X, absolute_world_position._Y, -absolute_world_position._Z };
				Quaternion rotation;
				rotation.FromEulerAngles(component._WorldTransform.GetRotation());
				physx::PxQuat physx_rotation;
				physx_rotation.x = rotation._X;
				physx_rotation.y = rotation._Y;
				physx_rotation.z = -rotation._Z;
				physx_rotation.w = rotation._W;
				const physx::PxTransform transform{ position, physx_rotation };

				//Create the actor!
				data._Actor = PhysXPhysicsSystemData::_Physics->createRigidStatic(transform);

				//Attach the shape.
				data._Actor->attachShape(*shape);

				//Release the shape.
				shape->release();
			}

			//Set the user data.
			data._Actor->userData = entity;

			//Add the actor to the scene.
			PhysXPhysicsSystemData::_Scene->addActor(*data._Actor);

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
			const physx::PxVec3 physx_position{ absolute_world_position._X, absolute_world_position._Y, -absolute_world_position._Z };
			Quaternion rotation;
			rotation.FromEulerAngles(world_transform.GetRotation());
			const physx::PxQuat physx_rotation{ rotation._X, rotation._Y, -rotation._Z, rotation._W };
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
			const physx::PxVec3 physx_position{ absolute_world_position._X, absolute_world_position._Y, -absolute_world_position._Z };
			Quaternion rotation;
			rotation.FromEulerAngles(world_transform.GetRotation());
			const physx::PxQuat physx_rotation{ rotation._X, rotation._Y, -rotation._Z, rotation._W };
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


			break;
		}

		case EntityType::StaticModel:
		{


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
	const physx::PxVec3 physx_origin{ ray._Origin._X, ray._Origin._Y, -ray._Origin._Z };
	const physx::PxVec3 physx_direction{ ray._Direction._X, ray._Direction._Y, -ray._Direction._Z };
	physx::PxRaycastBuffer raycast_buffer;

	//Cast the ray!
	result->_HasHit = PhysXPhysicsSystemData::_Scene->raycast(	physx_origin,
																physx_direction,
																configuration._MaximumHitDistance,
																raycast_buffer);

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
	const physx::PxVec3 physx_position{ absolute_world_position._X, absolute_world_position._Y, -absolute_world_position._Z };

	//Apply impulses to all dynamic models.
	for (const DynamicModelEntityData &data : PhysXPhysicsSystemData::_DynamicModelEntityData)
	{
		//Calculate the force magnitude.
		const Vector3<float32> force_magnitude{ Vector3<float32>::Normalize(data._Entity->GetWorldTransform()->GetAbsolutePosition() - absolute_world_position) * force };
		const physx::PxVec3 physx_force_magnitude{ force_magnitude._X, force_magnitude._Y, -force_magnitude._Z };

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

	//First of all, try to build a convex mesh.
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
				points.Emplace(physx::PxVec3(vertex._Position._X, vertex._Position._Y, -vertex._Position._Z));
			}
		}

		convex_mesh_description.points.data = points.Data();
		convex_mesh_description.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		//Build the convex mesh.
		physx::PxDefaultMemoryOutputStream output_stream_buffer;
		physx::PxConvexMeshCookingResult::Enum result;

		PhysXPhysicsSystemData::_Cooking->cookConvexMesh(convex_mesh_description, output_stream_buffer, &result);

		ASSERT(result == physx::PxConvexMeshCookingResult::eSUCCESS, "Convex mesh building failed!");

		//Set the type.
		collision_model_data->_Type = CollisionModelData::Type::CONVEX;

		//Set the data.
		collision_model_data->_Data.Upsize<false>(static_cast<uint64>(output_stream_buffer.getSize()));
		Memory::Copy(collision_model_data->_Data.Data(), output_stream_buffer.getData(), sizeof(byte) * output_stream_buffer.getSize());

		//Building of the convex mesh was successful, so return!
		return;
	}

	//If this code executes, building a convex mesh failed, and a triangle mesh needs to be built instead.
	ASSERT(false, "well");
}
#endif