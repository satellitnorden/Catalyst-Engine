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

	//Padding.
	Padding<8> _Padding;

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

DECLARE_COMPONENT
(
	ParticleSystemComponent,
	ParticleSystemInitializationData,
	ParticleSystemInstanceData,
	COMPONENT_INITIALIZE()
	COMPONENT_POST_INITIALIZE()

public:

	/*
	*	Sets default values for initialization data.
	*/
	void DefaultInitializationData(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT override;

	/*
	*	Runs after all components have created their instance for the given entity.
	*	Useful if there is some setup needed involving multiple components.
	*/
	void PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT override;

	/*
	*	Runs after the given update phase.
	*/
	void PostUpdate(const UpdatePhase update_phase) NOEXCEPT override;

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
);