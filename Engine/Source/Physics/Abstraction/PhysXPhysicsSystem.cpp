#if defined(CATALYST_PHYSICS_PHYSX)
//Header file.
#include <Systems/PhysicsSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Physics.
#include <Physics/Abstraction/PhysXCharacterControllerAbstractionData.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/MemorySystem.h>

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
	physx::PxFoundation *RESTRICT _Foundation{ nullptr };

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//The PVD.
	physx::PxPvd *RESTRICT _PVD{ nullptr };
#endif

	//The physics.
	physx::PxPhysics *RESTRICT _Physics{ nullptr };

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

	//Create the controller manager.
	PhysXPhysicsSystemData::_ControllerManager = PxCreateControllerManager(*PhysXPhysicsSystemData::_Scene);

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

			const Vector3<float32> euler_angles{ rotation.ToEulerAngles() };
			new_world_transform.SetRotation(-euler_angles);

			//Set the world transform of the entity.
			*data._Entity->ModifyWorldTransform() = new_world_transform;
		}
	}
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
#endif