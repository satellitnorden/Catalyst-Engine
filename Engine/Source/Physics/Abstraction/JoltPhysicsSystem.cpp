#if defined(CATALYST_PHYSICS_JOLT)
//Header file.
#include <Systems/PhysicsSystem.h>

//Core.
#include <Core/Containers/StreamArchive.h>

//Physics.
#include <Physics/Abstraction/JoltCharacterControllerAbstractionData.h>

//Profiling.
#include <Profiling/Profiling.h>

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
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
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

/*
*	Stream archive out class definition.
*/
class StreamArchiveOut final : public JPH::StreamOut
{

public:

	/*
	*	Writes a number of bytes.
	*/
	FORCE_INLINE void WriteBytes(const void *data, size_t number_of_bytes) NOEXCEPT override
	{
		_StreamArchive.Write(data, number_of_bytes);
	}

	
	/*
	*	Returns if there was an IO failure.
	*/
	FORCE_INLINE bool IsFailed() const NOEXCEPT override
	{
		return false;
	}

	/*
	*	Returns the data.
	*/
	FORCE_INLINE NO_DISCARD const void *const RESTRICT Data() const NOEXCEPT
	{
		return _StreamArchive.Data();
	}

	/*
	*	Returns the size.
	*/
	FORCE_INLINE NO_DISCARD uint64 Size() const NOEXCEPT
	{
		return _StreamArchive.Size();
	}

private:

	//The underlying stream archive.
	StreamArchive _StreamArchive;

};

/*
*	Stream in class definition.
*/
class StreamIn final : public JPH::StreamIn
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE StreamIn(const byte *const RESTRICT data, const uint64 size) NOEXCEPT
		:
		_Data(data),
		_Size(size)
	{

	}

	/*
	*	Reads a number of bytes.
	*/
	FORCE_INLINE void ReadBytes(void *data, size_t number_of_bytes) NOEXCEPT override
	{
		Memory::Copy(data, &_Data[_CurrentPosition], number_of_bytes);
		_CurrentPosition += number_of_bytes;
	}

	/*
	*	Returns when an attempt has been made to read past the end of the file
	*/
	FORCE_INLINE bool IsEOF() const NOEXCEPT override
	{
		return _CurrentPosition == _Size;
	}

	/*
	*	Returns if there was an IO failure.
	*/
	FORCE_INLINE bool IsFailed() const NOEXCEPT override
	{
		return _CurrentPosition > _Size;
	}

private:

	//The underlying data.
	const byte *RESTRICT _Data;

	//The underlying size.
	uint64 _Size;

	//The current position.
	uint64 _CurrentPosition{ 0 };

};

//Jolt physics system constants.
namespace JoltPhysicsSystemConstants
{
	constexpr JPH::uint MAXIMUM_BODIES{ 8'192 };
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

	//Container for all convex hull shapes.
	DynamicArray<JPH::ConvexHullShape *RESTRICT> _ConvexHullShapes;

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
	//Fill up the points.
	JPH::Array<JPH::Vec3> points;

	{
		const uint64 number_of_points{ collision_model_data._Data.Size() / sizeof(JPH::Vec3) };
		points.resize(number_of_points);
		Memory::Copy(points.data(), collision_model_data._Data.Data(), number_of_points * sizeof(JPH::Vec3));
	}

	//Fill in the convex hull shape settings.
	JPH::ConvexHullShapeSettings convex_hull_shape_settings{ points };

	//Create the shape.
	JPH::ConvexHullShape::ShapeResult convex_hull_shape_result{ convex_hull_shape_settings.Create() };

	//Retrieve the shape.
	JPH::ConvexHullShape *const RESTRICT shape{ static_cast<JPH::ConvexHullShape*const RESTRICT>(convex_hull_shape_result.Get().GetPtr()) };
	
	//Add a ref so it gets kept around.
	shape->AddRef();

	//Add the convex hull shape.
	JoltPhysicsSystemData::_ConvexHullShapes.Emplace(shape);

	//Set the shape!
	*collision_model = shape;
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

	//Destroy all convex hull shapes.
	for (JPH::ConvexHullShape *const RESTRICT convex_hull_shape : JoltPhysicsSystemData::_ConvexHullShapes)
	{
		convex_hull_shape->Release();
	}

	JoltPhysicsSystemData::_ConvexHullShapes.Clear();

	//Destroy all characters.
	for (JPH::Character *const RESTRICT character : JoltPhysicsSystemData::_Characters)
	{
		character->Release();
	}

	JoltPhysicsSystemData::_Characters.Clear();
}

/*
*	Creates a height field actor on the sub-system.
*/
void PhysicsSystem::SubCreateHeightFieldActor
(
	const WorldPosition &world_position,
	const Texture2D<float32> &height_field,
	const bool add_to_world,
	ActorHandle *const RESTRICT actor_handle
) NOEXCEPT
{
	PROFILING_SCOPE("PhysicsSystem::SubCreateHeightFieldActor");

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
	JPH::Body *const RESTRICT body{ body_interface.CreateBody(body_creation_settings) };

	//Add the body!
	if (add_to_world)
	{
		body_interface.AddBody(body->GetID(), JPH::EActivation::DontActivate);
	}

	//Set the actor handle.
	*actor_handle = body;
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
	switch (collision_type)
	{
		case ModelCollisionType::BOX:
		{
			//Cache the absolute world position.
			const Vector3<float32> absolute_world_position{ world_transform.GetAbsolutePosition() };

			//Cache the rotation.
			const Quaternion &rotation{ world_transform.GetRotation() };

			//Cache a local copy of the axis aligned bounding box.
			AxisAlignedBoundingBox3D _axis_aligned_bounding_box{ axis_aligned_bounding_box };

			//Apply the scale.
			_axis_aligned_bounding_box._Minimum *= world_transform.GetScale();
			_axis_aligned_bounding_box._Maximum *= world_transform.GetScale();

			//For shapes like planes and stuff like that, the axis aligned bounding box might be the same on one axis, so fix that by thickening it slightly.
			for (uint8 axis_index{ 0 }; axis_index < 3; ++axis_index)
			{
				if (_axis_aligned_bounding_box._Minimum[axis_index] == _axis_aligned_bounding_box._Maximum[axis_index])
				{
					_axis_aligned_bounding_box._Minimum[axis_index] -= FLOAT32_EPSILON;
					_axis_aligned_bounding_box._Maximum[axis_index] += FLOAT32_EPSILON;
				}
			}

			//Retrieve the dimensions.
			const Vector3<float32> dimensions{ _axis_aligned_bounding_box.Dimensions() };

			//Set up the half extent.
			const JPH::Vec3 half_extent{ dimensions._X * 0.5f, dimensions._Y * 0.5f, dimensions._Z * 0.5f };

			//Set up the shape extent.
			JPH::BoxShapeSettings shape_settings{ half_extent, 0.0f };

			//Create the shape!
			const JPH::Shape::ShapeResult shape_result{ shape_settings.Create() };

			//Set up the body creation settings.
			const JPH::BodyCreationSettings body_creation_settings{ shape_result.Get(), JPH::Vec3(absolute_world_position._X, absolute_world_position._Y + half_extent.GetY(), absolute_world_position._Z), JPH::Quat(rotation._X, rotation._Y, rotation._Z, rotation._W), simulation_configuration._SimulatePhysics ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static, simulation_configuration._SimulatePhysics ? static_cast<JPH::ObjectLayer>(PhysicsLayer::DYNAMIC) : static_cast<JPH::ObjectLayer>(PhysicsLayer::STATIC) };

			//Retrieve the body interface.
			JPH::BodyInterface &body_interface{ JoltPhysicsSystemData::_System.GetBodyInterface() };

			//Create the body.
			JPH::Body *const RESTRICT body{ body_interface.CreateBody(body_creation_settings) };

			//Add the body!
			body_interface.AddBody(body->GetID(), JPH::EActivation::DontActivate);

			//Set the actor handle.
			*actor_handle = body;

			break;
		}

		case ModelCollisionType::COLLISION_MODEL:
		{
			//Cache the absolute world position.
			const Vector3<float32> absolute_world_position{ world_transform.GetAbsolutePosition() };

			//Cache the rotation.
			const Quaternion &rotation{ world_transform.GetRotation() };

			//Cache the convex hull shape.
			const JPH::ConvexHullShape *const RESTRICT convex_hull_shape{ static_cast<const JPH::ConvexHullShape *const RESTRICT>(collision_model) };

			//Retrieve the body interface.
			JPH::BodyInterface &body_interface = JoltPhysicsSystemData::_System.GetBodyInterface();

			//Set up the body creation settings.
			JPH::BodyCreationSettings body_creation_settings{ convex_hull_shape, JPH::Vec3(absolute_world_position._X, absolute_world_position._Y, absolute_world_position._Z), JPH::Quat(rotation._X, rotation._Y, rotation._Z, rotation._W), simulation_configuration._SimulatePhysics ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static, simulation_configuration._SimulatePhysics ? static_cast<JPH::ObjectLayer>(PhysicsLayer::DYNAMIC) : static_cast<JPH::ObjectLayer>(PhysicsLayer::STATIC) };

			//Create the body.
			JPH::Body *const RESTRICT body{ body_interface.CreateBody(body_creation_settings) };

			//Add the body!
			body_interface.AddBody(body->GetID(), JPH::EActivation::DontActivate);

			//Set the actor handle.
			*actor_handle = body;

			break;
		}
	}
}

/*
*	Adds the given actor to the world on the sub-system.
*/
void PhysicsSystem::SubAddToWorld(const ActorHandle actor_handle) NOEXCEPT
{
	//Retrieve the body interface.
	JPH::BodyInterface &body_interface = JoltPhysicsSystemData::_System.GetBodyInterface();

	//Cache the body.
	JPH::Body *const RESTRICT body{ static_cast<JPH::Body *const RESTRICT>(actor_handle) };

	//Add the body!
	body_interface.AddBody(body->GetID(), JPH::EActivation::DontActivate);

}

/*
*	Destroys an actor on the sub-system.
*/
void PhysicsSystem::SubDestroyActor(ActorHandle *const RESTRICT actor_handle) NOEXCEPT
{
	//Retrieve the body interface.
	JPH::BodyInterface &body_interface{ JoltPhysicsSystemData::_System.GetBodyInterface() };

	//Cache the body.
	JPH::Body *const RESTRICT body{ static_cast<JPH::Body *const RESTRICT>(*actor_handle) };

	//Remove and destroy the body.
	body_interface.RemoveBody(body->GetID());
	body_interface.DestroyBody(body->GetID());
}

/*
*	Returns the world transform for the given actor on the sub-system.
*/
void PhysicsSystem::SubGetActorWorldTransform(const ActorHandle actor_handle, WorldTransform *const RESTRICT world_transform) NOEXCEPT
{
	//Cache the body.
	JPH::Body *const RESTRICT body{ static_cast<JPH::Body *const RESTRICT>(actor_handle) };

	//Cache the shape.
	const JPH::Shape *const RESTRICT shape{ body->GetShape() };

	//Retrieve the transform.
	const JPH::RMat44 transform{ body->GetWorldTransform() };

	//Write in the translation.
	{
		JPH::Vec3 translation{ transform.GetTranslation() };

		//If this is a box, apply the appropriate offset.
		if (shape->GetSubType() == JPH::EShapeSubType::Box)
		{
			const JPH::BoxShape *const RESTRICT box_shape{ static_cast<const JPH::BoxShape *const RESTRICT>(shape) };
			const JPH::Vec3 offset{ transform.Multiply3x3(JPH::Vec3(0.0f, box_shape->GetHalfExtent().GetY() , 0.0f)) };
			translation -= offset;
		}

		world_transform->SetAbsolutePosition(Vector3<float32>(translation.GetX(), translation.GetY(), translation.GetZ()));
	}

	//Write in the rotation.
	{
		const JPH::Quat rotation{ transform.GetQuaternion() };

		world_transform->SetRotation(Quaternion(rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW()));
	}

	//Write in scale.
	{
		const Vector3<float32> scale{ 1.0f, 1.0f, 1.0f }; //TODO: Do Jolt bodies even have scale?

		world_transform->SetScale(scale);
	}
}

/*
*	Updates the world transform of the given actor on the sub-system.
*/
void PhysicsSystem::SubUpdateWorldTransform(const WorldTransform &world_transform, ActorHandle *const RESTRICT actor_handle) NOEXCEPT
{
	//Retrieve the body interface.
	JPH::BodyInterface &body_interface{ JoltPhysicsSystemData::_System.GetBodyInterface() };

	//Cache the body.
	JPH::Body *const RESTRICT body{ static_cast<JPH::Body *const RESTRICT>(*actor_handle) };

	//Set up the position/rotation.
	JPH::Vec3 position;
	JPH::Quat rotation;

	//Write in the position.
	{
		const Vector3<float32> _position{ world_transform.GetAbsolutePosition() };

		position = JPH::Vec3(_position._X, _position._Y, _position._Z);
	}

	//Write in the rotation.
	{
		const Quaternion &_rotation{ world_transform.GetRotation() };

		rotation = JPH::Quat(_rotation._X, _rotation._Y, _rotation._Z, _rotation._W);
	}

	//Update the body!
	body_interface.SetPositionAndRotation(body->GetID(), position, rotation, JPH::EActivation::DontActivate);
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
	//Fill up the points.
	JPH::Array<JPH::Vec3> points;

	//Calculate the total number of points.
	size_t total_number_of_points{ 0 };

	for (const ModelFile::Mesh &mesh : model_file._Meshes)
	{
		total_number_of_points += mesh._Vertices.Size();
	}

	//Reserve the necessary amount of data for the points.
	points.reserve(total_number_of_points);

	//Now actually add the points!
	for (const ModelFile::Mesh &mesh : model_file._Meshes)
	{
		for (const Vertex &vertex : mesh._Vertices)
		{
			points.emplace_back(JPH::Vec3(vertex._Position._X, vertex._Position._Y, vertex._Position._Z));
		}
	}

	//Just fill the data up with points, cook it on boot. (:
	collision_model_data->_Type = CollisionModelData::Type::CONVEX; //Doesn't really matter.
	collision_model_data->_Data.Upsize<false>(sizeof(JPH::Vec3) * points.size());
	Memory::Copy(collision_model_data->_Data.Data(), points.data(), sizeof(JPH::Vec3) * points.size());
}
#endif