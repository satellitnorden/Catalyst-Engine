#if defined(CATALYST_PHYSICS_JOLT)
//Header file.
#include <Systems/PhysicsSystem.h>

//Physics.
#include <Physics/Abstraction/JoltCharacterControllerAbstractionData.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/MemorySystem.h>

//Third party.
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>

//Enumeration covering all physics layers.
enum class PhysicsLayer : uint8
{
	STATIC,
	DYNAMIC,

	NUMBER_OF_PHYSICS_LAYERS
};

/*
*	Broadphase layer interface class definition.
*/
class BroadphaseLayerInterface final : public JPH::BroadPhaseLayerInterface
{

public:

	FORCE_INLINE BroadphaseLayerInterface() NOEXCEPT
	{
		for (uint8 i{ 0 }; i < UNDERLYING(PhysicsLayer::NUMBER_OF_PHYSICS_LAYERS); ++i)
		{
			_BroadphaseLayers[i] = JPH::BroadPhaseLayer(i);
		}
	}

	FORCE_INLINE JPH::uint GetNumBroadPhaseLayers() const override
	{
		return static_cast<JPH::uint>(PhysicsLayer::NUMBER_OF_PHYSICS_LAYERS);
	}

	FORCE_INLINE JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer object_layer) const override
	{
		return _BroadphaseLayers[object_layer];
	}

private:

	//The broadphase layers.
	StaticArray<JPH::BroadPhaseLayer, UNDERLYING(PhysicsLayer::NUMBER_OF_PHYSICS_LAYERS)> _BroadphaseLayers;

};

/*
*	Object vs. broadphase layer filter class definition.
*/
class ObjectVsBroadphaseLayerFilter final : public JPH::ObjectVsBroadPhaseLayerFilter
{

public:

	FORCE_INLINE bool ShouldCollide(JPH::ObjectLayer object_layer, JPH::BroadPhaseLayer broadphase_layer) const override
	{
		switch (static_cast<PhysicsLayer>(object_layer))
		{
			case PhysicsLayer::STATIC:
			{
				return static_cast<PhysicsLayer>(broadphase_layer.GetValue()) == PhysicsLayer::DYNAMIC;
			}

			case PhysicsLayer::DYNAMIC:
			{
				return true;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return false;
			}
		}
	}

};

/*
*	Object layer pair filter class definition.
*/
class ObjectLayerPairFilter : public JPH::ObjectLayerPairFilter
{
public:

	FORCE_INLINE bool ShouldCollide(JPH::ObjectLayer object_layer_A, JPH::ObjectLayer object_layer_B) const override
	{
		switch (static_cast<PhysicsLayer>(object_layer_A))
		{
			case PhysicsLayer::STATIC:
			{
				return static_cast<PhysicsLayer>(object_layer_B) == PhysicsLayer::DYNAMIC;
			}

			case PhysicsLayer::DYNAMIC:
			{
				return true;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return false;
			}
		}
	}
};

//Jolt physics system constants.
namespace JoltPhysicsSystemConstants
{
	constexpr JPH::uint MAXIMUM_BODIES{ 1'024 };
	constexpr JPH::uint MAXIMUM_BODY_PAIRS{ 1'024 };
	constexpr JPH::uint MAXIMUM_CONTACT_CONSTRAINTS{ 1'024 };
}

//Jolt physics system data.
namespace JoltPhysicsSystemData
{
	//The broadphase layer interface.
	BroadphaseLayerInterface _BroadphaseLayerInterface;

	//The object vs. broadphase layer filter
	ObjectVsBroadphaseLayerFilter _ObjectVsBroadphaseLayerFilter;

	//The object layer pair filter.
	ObjectLayerPairFilter _ObjectLayerPairFilter;

	//The system.
	JPH::PhysicsSystem _System;

	//The temporary allocator.
	JPH::TempAllocatorImpl *RESTRICT _TemporaryAllocator;

	//The job system thread pool.
	JPH::JobSystemThreadPool *RESTRICT _JobSystemThreadPool;

	//Container for all characters.
	DynamicArray<JPH::Character *RESTRICT> _Characters;

}

/*
*	Initializes the physics sub-system.
*/
void PhysicsSystem::SubInitialize() NOEXCEPT
{
	//Register the default allocator.
	JPH::RegisterDefaultAllocator();

	//Allocate the factory.
	JPH::Factory::sInstance = new JPH::Factory();

	//Register the types.
	JPH::RegisterTypes();

	//Initialize the physics system.
	JoltPhysicsSystemData::_System.Init
	(
		JoltPhysicsSystemConstants::MAXIMUM_BODIES,
		0,
		JoltPhysicsSystemConstants::MAXIMUM_BODY_PAIRS,
		JoltPhysicsSystemConstants::MAXIMUM_CONTACT_CONSTRAINTS,
		JoltPhysicsSystemData::_BroadphaseLayerInterface,
		JoltPhysicsSystemData::_ObjectVsBroadphaseLayerFilter,
		JoltPhysicsSystemData::_ObjectLayerPairFilter
	);

	//Allocate the temporary allocator.
	JoltPhysicsSystemData::_TemporaryAllocator = new JPH::TempAllocatorImpl(1'024 * 1'024 * 10); //10MB should be enough.

	//Allocate the job system thread pool.
	JoltPhysicsSystemData::_JobSystemThreadPool = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, 1);
}

/*
*	Creates a collision model on the sub-system.
*/
void PhysicsSystem::SubCreateCollisionModel(const CollisionModelData &collision_model_data, CollisionModelHandle *const RESTRICT collision_model) NOEXCEPT
{
	
}

/*
*	Updates the physics sub-system during the physics update phase.
*/
void PhysicsSystem::SubPhysicsUpdate() NOEXCEPT
{
	//Retrieve the delta time.
	const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

	//Calculate the number of collision steps.
	const int32 number_of_collision_steps{ 1 }; //TODO!

	//Update the physics system!
	JoltPhysicsSystemData::_System.Update(delta_time, number_of_collision_steps, JoltPhysicsSystemData::_TemporaryAllocator, JoltPhysicsSystemData::_JobSystemThreadPool);

	//Post-update all characters.
	for (JPH::Character *const RESTRICT character : JoltPhysicsSystemData::_Characters)
	{
		character->PostSimulation(0.05f);
	}
}

/*
*	Terminates the physics sub-system.
*/
void PhysicsSystem::SubTerminate() NOEXCEPT
{
	//Unregister the types.
	JPH::UnregisterTypes();

	//Destroy the factory.
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;

	//Destroy the temporary allocator.
	delete JoltPhysicsSystemData::_TemporaryAllocator;
	JoltPhysicsSystemData::_TemporaryAllocator = nullptr;

	//Destroy the job system thread pool.
	delete JoltPhysicsSystemData::_JobSystemThreadPool;
	JoltPhysicsSystemData::_JobSystemThreadPool = nullptr;
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
	//Retrieve the absolute world position.
	const Vector3<float32> absolute_world_position{ world_position.GetAbsolutePosition() };

	//Calculate the offset.
	const float32 offset{ static_cast<float32>(height_field.GetResolution()) * 0.5f };

	//Retrieve the body interface.
	JPH::BodyInterface &body_interface = JoltPhysicsSystemData::_System.GetBodyInterface();

	//Set up the height field shape settings.
	const JPH::HeightFieldShapeSettings height_field_shape_settings
	{
		height_field.Data(),
		JPH::Vec3(absolute_world_position._X - offset, absolute_world_position._Y, absolute_world_position._Z - offset),
		JPH::Vec3(1.0f, 1.0f, 1.0f),
		height_field.GetResolution()
	};

	//Create the shape.
	const JPH::ShapeSettings::ShapeResult height_field_shape_result{ height_field_shape_settings.Create() };

	//Set up the body creationg settings.
	const JPH::BodyCreationSettings body_creation_settings{ height_field_shape_result.Get(), JPH::Vec3(0.0f, 0.0f, 0.0f), JPH::Quat::sIdentity(), JPH::EMotionType::Static, static_cast<JPH::ObjectLayer>(PhysicsLayer::STATIC) };

	//Create the body.
	const JPH::Body *const RESTRICT body{ body_interface.CreateBody(body_creation_settings) };

	//Add the body!
	body_interface.AddBody(body->GetID(), JPH::EActivation::DontActivate);
}

/*
*	Creates a model actor on the sub-system.
*/
void PhysicsSystem::SubCreateModelActor
(
	const WorldTransform &world_transform,
	const ModelCollisionType collision_type,
	const AxisAlignedBoundingBox3D &axis_aligned_bounding_box,
	const CollisionModelHandle collision_model,
	const ModelSimulationConfiguration &simulation_configuration,
	ActorHandle *const RESTRICT actor_handle
) NOEXCEPT
{
	
}

/*
*	Destroys an actor on the sub-system.
*/
void PhysicsSystem::SubDestroyActor(ActorHandle *const RESTRICT actor_handle) NOEXCEPT
{
	
}

/*
*	Returns the world transform for the given actor on the sub-system.
*/
void PhysicsSystem::SubGetActorWorldTransform(const ActorHandle actor_handle, WorldTransform *const RESTRICT world_transform) NOEXCEPT
{
	
}

/*
*	Updates the world transform of the given actor on the sub-system.
*/
void PhysicsSystem::SubUpdateWorldTransform(const WorldTransform &world_transform, ActorHandle *const RESTRICT actor_handle) NOEXCEPT
{
	
}

/*
*	Casts a sub-system ray.
*/
void PhysicsSystem::SubCastRay(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT
{
	result->_HasHit = false;
	result->_HitDistance = FLOAT32_MAXIMUM;
	result->_Type = RaycastResult::Type::NONE;
}

/*
*	Adds an sub-system impulse at the given world position with the given force.
*/
void PhysicsSystem::SubAddImpulse(const WorldPosition &world_position, const float32 force) NOEXCEPT
{
	
}

/*
*	Creates a sub-system character controller.
*/
RESTRICTED NO_DISCARD CharacterController *const RESTRICT PhysicsSystem::SubCreateCharacterController(const CharacterControllerConfiguration &configuration) NOEXCEPT
{
	//Set up the abstraction data.
	Any<CharacterController::ABSTRACTION_DATA_SIZE> abstraction_data;

	{
		//Set up the capsule shape settings.
		const JPH::CapsuleShapeSettings capsule_shape_settings
		{
			configuration._CapsuleHeight * 0.5f,
			configuration._CapsuleRadius
		};

		//Create the shape.
		const JPH::ShapeSettings::ShapeResult capsule_shape_result{ capsule_shape_settings.Create() };

		//Set up the character settings.
		JPH::CharacterSettings character_settings;

		character_settings.mShape = capsule_shape_result.Get();
		character_settings.mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -configuration._CapsuleRadius);
		character_settings.mMaxSlopeAngle = BaseMath::DegreesToRadians(45.0f);
		character_settings.mLayer = static_cast<JPH::ObjectLayer>(PhysicsLayer::DYNAMIC);
		character_settings.mMass = 80.0f;
		character_settings.mFriction = 1.0f;
		character_settings.mGravityFactor = 1.0f;

		//Allocate the character.
		abstraction_data.Get<JoltCharacterControllerAbstractionData>()->_Character = new JPH::Character(&character_settings, JPH::Vec3(0.0f, 0.0f, 0.0f), JPH::Quat::sIdentity(), 0, &JoltPhysicsSystemData::_System);
	
		//Add the character to the physics system.
		abstraction_data.Get<JoltCharacterControllerAbstractionData>()->_Character->AddToPhysicsSystem();
	}

	//Add the character.
	JoltPhysicsSystemData::_Characters.Emplace(abstraction_data.Get<JoltCharacterControllerAbstractionData>()->_Character);

	return new (MemorySystem::Instance->TypeAllocate<CharacterController>()) CharacterController(abstraction_data);
}

/*
*	Builds a collision model on the sub-system.
*/
void PhysicsSystem::SubBuildCollisionModel(const ModelFile &model_file, CollisionModelData *const RESTRICT collision_model_data) NOEXCEPT
{
	
}
#endif