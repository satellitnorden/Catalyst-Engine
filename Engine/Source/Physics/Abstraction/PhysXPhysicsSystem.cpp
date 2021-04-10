#if defined(CATALYST_PHYSICS_PHYSX)
//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

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

	//The actor.
	physx::PxRigidStatic *RESTRICT _Actor;

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
	physx::PxFoundation *RESTRICT _Foundation;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//The PVD.
	physx::PxPvd *RESTRICT _PVD;
#endif

	//The physics.
	physx::PxPhysics *RESTRICT _Physics;

	//The dispatcher.
	physx::PxDefaultCpuDispatcher *RESTRICT _Dispatcher;

	//The scene.
	physx::PxScene *RESTRICT _Scene;

	//The default material.
	physx::PxMaterial *RESTRICT _DefaultMaterial;

	//The dynamic model entity data.
	DynamicArray<DynamicModelEntityData> _DynamicModelEntityData;

	//The static model entity data.
	DynamicArray<StaticModelEntityData> _StaticModelEntityData;

	//The time since last update.
	float32 _TimeSinceLastUpdate{ 0.0f };

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

	//Create the ground plane.
	physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*PhysXPhysicsSystemData::_Physics, physx::PxPlane(0,1,0,0), *PhysXPhysicsSystemData::_DefaultMaterial);
	PhysXPhysicsSystemData::_Scene->addActor(*groundPlane);
}

/*
*	Updates the physics sub-system during the physics update phase.
*/
void PhysicsSystem::SubPhysicsUpdate() NOEXCEPT
{
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
			//Retrieve the transform.
			Matrix4x4 transform;

			{
				physx::PxShape *RESTRICT shape;
				data._Actor->getShapes(&shape, 1);
				const physx::PxTransform translation_offset{ physx::PxVec3(data._TranslationOffset._X, data._TranslationOffset._Y, data._TranslationOffset._Z) };
				const physx::PxMat44 final_transform{ data._Actor->getGlobalPose() * shape->getLocalPose() * translation_offset };
				Memory::Copy(&transform, &final_transform, sizeof(Matrix4x4));
			}

			//Set up the new world transform.
			WorldTransform new_world_transform;
			const Vector3<float32> shape_position{ transform.GetTranslation() };
			new_world_transform.SetAbsolutePosition(Vector3<float32>(shape_position._X, shape_position._Y, shape_position._Z));

			const Vector3<float32> rotation{ transform.GetRotation() };
			new_world_transform.SetRotation(rotation);

			//Set the world transform of the entity.
			*data._Entity->ModifyWorldTransform() = new_world_transform;
		}
	}
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
			RenderingUtilities::TransformAxisAlignedBoundingBox(model_space_axis_aligned_bounding_box, Matrix4x4(VectorConstants::ZERO, VectorConstants::ZERO, Vector3<float32>(component._CurrentWorldTransform.GetScale())), &scaled_model_space_axis_aligned_bounding_box);
			const Vector3<float32> translation_offset{ -AxisAlignedBoundingBox3D::CalculateCenter(scaled_model_space_axis_aligned_bounding_box) };

			//Create the actor.
			{
				//Create the shape.
				const AxisAlignedBoundingBox3D local_axis_aligned_bounding_box{ component._WorldSpaceAxisAlignedBoundingBox };
				const Vector3<float32> dimensions{ local_axis_aligned_bounding_box.Dimensions() };
				const physx::PxBoxGeometry geometry{ dimensions._X * 0.5f, dimensions._Y * 0.5f, dimensions._Z * 0.5f };
				physx::PxShape* const RESTRICT shape{ PhysXPhysicsSystemData::_Physics->createShape(geometry, *PhysXPhysicsSystemData::_DefaultMaterial) };
				//shape->setLocalPose(physx::PxTransform(physx::PxVec3(translation_offset._X, translation_offset._Y, translation_offset._Z)));

				//Set up the transform.
				const Vector3<float32> absolute_world_position{ component._CurrentWorldTransform.GetAbsolutePosition() };
				const physx::PxVec3 position{ absolute_world_position._X, absolute_world_position._Y, absolute_world_position._Z };
				const physx::PxTransform transform{ position };

				//Create the actor!
				data._Actor = PhysXPhysicsSystemData::_Physics->createRigidDynamic(transform);
			
				//Attach the shape.
				data._Actor->attachShape(*shape);

				//Set up the actor.
				physx::PxRigidBodyExt::updateMassAndInertia(*data._Actor, 10.0f);

				//Release the shape.
				shape->release();
			}

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

			//Set up the static model entity data.
			StaticModelEntityData data;

			//Create the actor.
			{
				//Set up the transform.
				const Vector3<float32> absolute_world_position{ component._WorldTransform.GetAbsolutePosition() };
				const physx::PxVec3 position{ absolute_world_position._X, absolute_world_position._Y, absolute_world_position._Z };
				const physx::PxTransform transform{ position };

				//Set up the geometry.
				const AxisAlignedBoundingBox3D local_axis_aligned_bounding_box{ component._WorldSpaceAxisAlignedBoundingBox.GetLocalAxisAlignedBoundingBox() };
				const Vector3<float32> dimensions{ local_axis_aligned_bounding_box.Dimensions() };
				const physx::PxBoxGeometry geometry{ dimensions._X * 0.5f, dimensions._Y * 0.5f, dimensions._Z * 0.5f };

				//Calculate the shape offset.
				const Vector3<float32> center{ AxisAlignedBoundingBox3D::CalculateCenter(local_axis_aligned_bounding_box) };
				const physx::PxTransform shape_offset{ physx::PxVec3(-center._X, -center._Y, -center._Z) };

				//Create the actor!
				data._Actor = physx::PxCreateStatic(*PhysXPhysicsSystemData::_Physics, transform, geometry, *PhysXPhysicsSystemData::_DefaultMaterial, shape_offset);
			}

			//Add the actor to the scene.
			PhysXPhysicsSystemData::_Scene->addActor(*data._Actor);

			//Add the static model entity data.
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
*	Terminates the physics sub-system.
*/
void PhysicsSystem::SubTerminate() NOEXCEPT
{
	//Release the default material.
	PX_RELEASE(PhysXPhysicsSystemData::_DefaultMaterial);

	//Release the scene.
	PX_RELEASE(PhysXPhysicsSystemData::_Scene);

	//Release the dispatcher.
	PX_RELEASE(PhysXPhysicsSystemData::_Dispatcher);

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
#endif