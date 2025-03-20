#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Enumeration.h>
#include <Core/General/Padding.h>

//Components.
#include <Components/Core/Component.h>

//Content.
#include <Content/Assets/MaterialAsset.h>

//World.
#include <World/Core/WorldPosition.h>

CATALYST_ENUMERATION
(
	ParticleEmitterMode,
	uint8,
	SPHERE
);

/*
*	Particle instance class definition.
*/
class ParticleInstance final
{

public:

	//The world position.
	WorldPosition _WorldPosition;

	//The velocity.
	Vector3<float32> _Velocity;

	//The size.
	Vector2<float32> _Size;

	//The age.
	float32 _Age;

	//The lifetime.
	float32 _Lifetime;

};

/*
*	Particle packed instance class definition.
*/
class ParticlePackedInstance final
{

public:

	//The world position.
	Vector3<float32> _WorldPosition;

	//The size.
	Vector2<float32> _Size;

	//The normalized age.
	float32 _NormalizedAge;

	//The fade in time.
	float32 _FadeInTime;

	//The fade out time.
	float32 _FadeOutTime;

};

/*
*	Particle emitter class definition.
*/
class ParticleEmitter final
{

public:

	//The particle emitter mode.
	ParticleEmitterMode _ParticleEmitterMode;

	union
	{
		struct
		{
			//The radius.
			float32 _Radius;
		} _SphereMode;
	};

	//The initial burts.
	uint32 _InitialBurst;

	//The minimum velocity.
	Vector3<float32> _MinimumVelocity;

	//The maximum velocity.
	Vector3<float32> _MaximumVelocity;

	//The minimum size.
	Vector2<float32> _MinimumSize;

	//The maximum size.
	Vector2<float32> _MaximumSize;

	//The minimum lifetime.
	float32 _MinimumLifetime;

	//The maximum lifetime.
	float32 _MaximumLifetime;

	//The spawn rate, expressed in instances per second.
	uint32 _SpawnRate;

	//The position offset.
	Vector3<float32> _PositionOffset;

	//The drag.
	float32 _Drag;

	//Denotes how much gravity affects these particles.
	float32 _GravityAffection;

	//Denotes how much wind affects these particles.
	float32 _WindAffection;

	//The fade in time (normalized).
	float32 _FadeInTime;

	//The fade out time (normalized).
	float32 _FadeOutTime;

	//Denotes whether or not to shrink near the camera.
	bool _ShrinkNearCamera;

};

/*
*	Particle sub emitter class definition.
*/
class ParticleSubEmitter final
{

public:

	//The emitter.
	ParticleEmitter _Emitter;

	//The time since the last particle spawn.
	float32 _TimeSinceLastParticleSpawn;

	//The instances.
	DynamicArray<ParticleInstance> _Instances;

	//The packed instancs.
	DynamicArray<ParticlePackedInstance> _PackedInstances;

	//Denotes if the initial burst has been done.
	bool _HasDoneInitialBurst;

};

/*
*	Particle system shared data class definition.
*/
class ParticleSystemSharedData final
{

public:

	//The packed instancs.
	DynamicArray<ParticlePackedInstance> _PackedInstances;

};

/*
*	Particle system initialization data class definition.
*/
class ParticleSystemInitializationData final : public ComponentInitializationData
{

public:

	//The material.
	AssetPointer<MaterialAsset> _Material;

	//The emitter.
	ParticleEmitter _Emitter;

};

/*
*	Particle system instance data class definition.
*/
class ParticleSystemInstanceData final
{

public:

	//The material.
	AssetPointer<MaterialAsset> _Material;

	//The master emitter.
	ParticleEmitter _MasterEmitter;

	//The sub emitters.
	DynamicArray<ParticleSubEmitter> _SubEmitters;

	//The start instance index.
	uint32 _StartInstanceIndex;

	//The number of instances.
	uint32 _NumberOfInstances;

};

class ParticleSystemComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		ParticleSystem,
		COMPONENT_INITIALIZE()
		COMPONENT_POST_INITIALIZE()
		COMPONENT_POST_CREATE_INSTANCE()
		COMPONENT_DEFAULT_INITIALIZATION_DATA(ParticleSystemInitializationData)
		COMPONENT_PARALLEL_SUB_INSTANCE_UPDATE(UpdatePhase::PRE_RENDER)
		COMPONENT_POST_UPDATE(UpdatePhase::PRE_RENDER)
	);

	/*
	*	Callback for after an editable field change happens.
	*/
	void PostEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT override;

	FORCE_INLINE NO_DISCARD ParticleSystemSharedData &SharedData() NOEXCEPT
	{
		return _SharedData;
	}

private:

	ParticleSystemSharedData _SharedData;

};